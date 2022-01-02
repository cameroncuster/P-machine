#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "compiler.h"

// preprocessed constants
#define MAX_CODE_LENGTH 1000
#define MAX_SYMBOL_COUNT 100

// class (global) data
instruction *code;
int codeIndex;
symbol *table;
int tableIndex;
lexeme *tokens;
lexeme token;
int tokenIndex;
int level;

// parser members
void program();
void block();
void const_declaration();
int var_declaration();
void procedure_declaration();
void statement();
void condition();
void expression();
void term();
void factor();

// helpers
lexeme getnexttoken();
void mark();
int findsymbol(lexeme token, int kind);
int multipledeclarationcheck(lexeme token);
void emit(int opcode, int l, int m);
void addToSymbolTable(int k, char n[], int v, int l, int a, int m);
void printparseerror(int err_code);
void printsymboltable();
void printassemblycode();

instruction *parse(lexeme *list, int printTable, int printCode)
{
	// allocate the space for the parsed program
	code = malloc(MAX_CODE_LENGTH * sizeof(instruction));
	codeIndex = 0;
	table = malloc(MAX_SYMBOL_COUNT * sizeof(symbol));
	tableIndex = 0;

	tokens = list;
	token = tokens[0];
	tokenIndex = 0;

	// parse the program
	program();

	code[codeIndex].opcode = -1;

	// print table if specified
	if (printTable)
		printsymboltable();

	// print code if specified
	if (printCode)
		printassemblycode();

	return code;
}

void program()
{
	// JMP
	emit(7, 0, 0);

	// main function
	addToSymbolTable(3, "main", 0, 0, 0, 0);
	level = -1;

	// build out the block
	block();

	// assert period ending
	if (token.type != periodsym)
	{
		printparseerror(1);
		exit(0);
	}

	// HALT (SYS CALL)
	emit(9, 0, 3);

	// restore CALL destination address
	for (int line = 0; line < codeIndex; line++)
		if (code[line].opcode == 5)
			code[line].m = table[code[line].m].addr;

	// program start (initial JMP added above)
	code[0].m = table[0].addr;
}

void block()
{
	// next level (new block)
	level++;

	int procedure_idx = tableIndex - 1;

	// declarations
	const_declaration();
	int numVars = var_declaration();
	procedure_declaration();

	// addressing
	table[procedure_idx].addr = codeIndex * 3;

	if (level == 0)
		// INC to main
		emit(6, 0, numVars);
	else
		// INC -> alloc space for the activation record
		// (AR: static link (SL) dynamic link (DL) return address (RA))
		emit(6, 0, numVars + 3);

	// statements (non-declarative)
	statement();

	// variables in block will be out of scope on return
	mark();

	// restore level (return to parent)
	level--;
}

void const_declaration()
{
	char name[12];
	// declaring constants
	if (token.type == constsym)
	{
		do {
			getnexttoken();

			// identifier?
			if (token.type != identsym)
			{
				printparseerror(2);
				exit(0);
			}

			int symidx = multipledeclarationcheck(token);

			// check for multiple declarations
			if (symidx != -1)
			{
				printparseerror(18);
				exit(0);
			}

			// save ident name
			strcpy(name, token.name);

			getnexttoken();

			// must be assigned (const)
			if (token.type != assignsym)
			{
				printparseerror(2);
				exit(0);
			}

			getnexttoken();

			// must be an integer
			if (token.type != numbersym)
			{
				printparseerror(2);
				exit(0);
			}

			addToSymbolTable(1, name, token.value, level, 0, 0);

			getnexttoken();

			// do more declarations
		} while (token.type == commasym);

		// declarations must end with semicolon
		if (token.type != semicolonsym)
		{
			// if identifier symbol error
			if (token.type == identsym)
			{
				printparseerror(13);
				exit(0);
			}
			// other error
			else
			{
				printparseerror(14);
				exit(0);
			}
		}

		// move past the semicolon
		getnexttoken();
	}
}

int var_declaration()
{
	int numVars = 0;

	if (token.type == varsym)
	{
		do {
			// new declaration
			numVars++;

			getnexttoken();

			// variable declaration must lead with identifier
			if (token.type != identsym)
			{
				printparseerror(3);
				exit(0);
			}

			int symidx = multipledeclarationcheck(token);

			// check for multiple declarations
			if (symidx != -1)
			{
				printparseerror(18);
				exit(0);
			}

			// add to symbol table
			// if outside main (no control information)
			if (level == 0)
				addToSymbolTable(2, token.name, 0, level, numVars - 1,
						0);
			else
				// buffer for control information
				addToSymbolTable(2, token.name, 0, level, numVars + 2,
						0);

			getnexttoken();

		} while (token.type == commasym);

		// declarations must end with semicolon
		if (token.type != semicolonsym)
		{
			// if identifier symbol error
			if (token.type == identsym)
			{
				printparseerror(13);
				exit(0);
			}
			// other error
			else
			{
				printparseerror(14);
				exit(0);
			}
		}

		getnexttoken();
	}

	return numVars;
}

void procedure_declaration()
{
	while (token.type == procsym)
	{
		getnexttoken();

		// procedure declaration must begin with an identifier
		if (token.type != identsym) {
			printparseerror(4);
			exit(0);
		}

		int symidx = multipledeclarationcheck(token);

		// check for multiple declarations
		if (symidx != -1)
		{
			printparseerror(18);
			exit(0);
		}

		// add to symbol table
		addToSymbolTable(3, token.name, 0, level, 0, 0);

		getnexttoken();

		// procedure declarations must be followed by a semicolon symbol
		if (token.type != semicolonsym)
		{
			printparseerror(4);
			exit(0);
		}

		getnexttoken();

		// parse the new block (procedure)
		block();

		// symbol declarations should close with a semicolon symbol
		if (token.type != semicolonsym)
		{
			printparseerror(14);
			exit(0);
		}

		getnexttoken();

		// emit RTN
		emit(2, 0, 0);
	}
}

void statement()
{
	if (token.type == identsym)
	{
		int symIdx = findsymbol(token, 2);

		// not found as an identifier
		if (symIdx == -1)
		{
			// not a variable
			if (findsymbol(token, 1) != findsymbol(token, 3))
				printparseerror(6);
			// undeclared
			else
				printparseerror(19);
			exit(0);
		}

		getnexttoken();

		// identifier must be followed by assignment symbol
		if (token.type != assignsym)
		{
			printparseerror(5);
			exit(0);
		}

		getnexttoken();

		expression();

		// STO
		emit(4, level - table[symIdx].level, table[symIdx].addr);
	}
	else if (token.type == beginsym)
	{
		// parse statements
		do {
			getnexttoken();

			statement();
		} while (token.type == semicolonsym);

		// begin must be closed with end symbol
		if (token.type != endsym)
		{
			switch (token.type)
			{
				case identsym:
				case beginsym:
				case ifsym:
				case whilesym:
				case readsym:
				case writesym:
				case callsym:
					// must separate with semicolon symbol
					printparseerror(15);
					exit(0);
				default:
					// begin must be followed by end
					printparseerror(16);
					exit(0);
			}
		}

		getnexttoken();
	}
	else if (token.type == ifsym)
	{
		getnexttoken();

		condition();

		// if symbol must be followed by then symbol
		if (token.type != thensym)
		{
			printparseerror(8);
			exit(0);
		}

		int jpcIdx = codeIndex;

		// EMIT JPC
		emit(8, 0, 0);

		getnexttoken();

		statement();

		// handle the else statement
		if (token.type == elsesym)
		{
			int jmpIdx = codeIndex;
			// EMIT JMP
			emit(7, 0, 0);

			// JPC/JMP to the end
			code[jpcIdx].m = codeIndex * 3;

			getnexttoken();

			statement();

			code[jmpIdx].m = codeIndex * 3;
		}
		else
		{
			// JPC to the end
			code[jpcIdx].m = codeIndex * 3;
		}
	}
	else if (token.type == whilesym)
	{
		getnexttoken();

		int loopIdx = codeIndex;

		condition();

		// while symbol must be followed by do symbol
		if (token.type != dosym)
		{
			printparseerror(9);
			exit(0);
		}

		getnexttoken();

		int jpcIdx = codeIndex;

		// emit JPC
		emit(8, 0, 0);

		statement();

		// emit JMP
		emit(7, 0, loopIdx * 3);

		code[jpcIdx].m = codeIndex * 3;
	}
	else if (token.type == readsym)
	{
		getnexttoken();

		// read symbol must be followed by indentifier symbol
		if (token.type != identsym)
		{
			printparseerror(6);
			exit(0);
		}

		int symIdx = findsymbol(token, 2);

		// variable identifier not found in symbol table
		if (symIdx == -1)
		{
			// identifier is not a variable
			if (findsymbol(token, 1) != findsymbol(token, 3))
				printparseerror(6);
			// identifier is undeclared
			else
				printparseerror(19);
			exit(0);
		}

		getnexttoken();

		// emit READ
		emit(9, 0, 2);

		// emit STO
		emit(4, level - table[symIdx].level, table[symIdx].addr);
	}
	else if (token.type == writesym)
	{
		getnexttoken();
		expression();
		// emit WRITE
		emit(9, 0, 1);
	}
	else if (token.type == callsym)
	{
		getnexttoken();

		if (token.type != identsym)
		{
			printparseerror(7);
			exit(0);
		}

		int symIdx = findsymbol(token, 3);

		// procedure identifier not found in symbol table
		if (symIdx == -1)
		{
			// identifier is not a procedure
			if (findsymbol(token, 1) != findsymbol(token, 2))
				printparseerror(7);
			// identifier is undeclared
			else
				printparseerror(19);
			exit(0);
		}

		getnexttoken();

		// emit CAL
		emit(5, level - table[symIdx].level, symIdx);
	}
}

void condition()
{
	if (token.type == oddsym)
	{
		getnexttoken();
		expression();
		// emit ODD
		emit(2, 0, 6);
	}
	else
	{
		expression();
		if (token.type == eqlsym)
		{
			getnexttoken();
			expression();
			// emit EQL
			emit(2, 0, 8);
		}
		else if (token.type == neqsym)
		{
			getnexttoken();
			expression();
			// emit NEQ
			emit(2, 0, 9);
		}
		else if (token.type == lsssym)
		{
			getnexttoken();
			expression();
			// emit LSS
			emit(2, 0, 10);
		}
		else if (token.type == leqsym)
		{
			getnexttoken();
			expression();
			// emit LEQ
			emit(2, 0, 11);
		}
		else if (token.type == gtrsym)
		{
			getnexttoken();
			expression();
			// emit GTR
			emit(2, 0, 12);
		}
		else if (token.type == geqsym)
		{
			getnexttoken();
			expression();
			// emit GEQ
			emit(2, 0, 13);
		}
		else
		{
			// relational operator missing from condition
			printparseerror(10);
			exit(0);
		}
	}
}

void expression()
{
	if (token.type == subsym)
	{
		getnexttoken();
		term();

		// emit NEG
		emit(2, 0, 1);

		while (token.type == addsym || token.type == subsym)
		{
			if (token.type == addsym)
			{
				getnexttoken();
				term();
				// emit ADD
				emit(2, 0, 2);
			}
			else
			{
				getnexttoken();
				term();
				// emit SUB
				emit(2, 0, 3);
			}
		}
	}
	else
	{
		if (token.type == addsym)
			getnexttoken();

		term();
		while (token.type == addsym || token.type == subsym)
		{
			if (token.type == addsym)
			{
				getnexttoken();
				term();
				// emit ADD
				emit(2, 0, 2);
			}
			else
			{
				getnexttoken();
				term();
				// emit SUB
				emit(2, 0, 3);
			}
		}
	}

	// expression should not be followed by these symbols
	if (token.type == addsym ||
			token.type == subsym ||
			token.type == multsym ||
			token.type == divsym ||
			token.type == modsym ||
			token.type == lparensym ||
			token.type == identsym ||
			token.type == numbersym ||
			token.type == oddsym)
	{
		printparseerror(17);
		exit(0);
	}
}

void term()
{
	factor();
	while (token.type == multsym ||
			token.type == divsym ||
			token.type == modsym)
	{
		if (token.type == multsym)
		{
			getnexttoken();
			factor();
			// emit MUL
			emit(2, 0, 4);
		}
		else if (token.type == divsym)
		{
			getnexttoken();
			factor();
			// emit DIV
			emit(2, 0, 5);
		}
		else
		{
			getnexttoken();
			factor();
			// emit MOD
			emit(2, 0, 7);
		}
	}
}

void factor()
{
	if (token.type == identsym)
	{
		int symIdx_var = findsymbol(token, 2);
		int symIdx_const = findsymbol(token, 1);

		// identifier is neither a declared variable or a declared constant
		if (symIdx_var == -1 && symIdx_const == -1)
		{
			// identifier is a procedure in an arithmetic expression (incorrect)
			if (findsymbol(token, 3) != -1)
				printparseerror(11);
			// not declared
			else
				printparseerror(19);
			exit(0);
		}

		// const case
		if (symIdx_var == -1)
			// emit LIT
			emit(1, 0, table[symIdx_const].val);
		// var case
		else if (symIdx_const == -1 ||
				table[symIdx_var].level > table[symIdx_const].level)
			// emit LOD
			emit(3, level - table[symIdx_var].level, table[symIdx_var].addr);
		// const case
		else
			// emit LIT
			emit(1, 0, table[symIdx_const].val);

		getnexttoken();
	}
	else if (token.type == numbersym)
	{
		// emit LIT
		emit(1, 0, token.value);
		getnexttoken();
	}
	else if (token.type == lparensym)
	{
		getnexttoken();

		// parse expression
		expression();

		// expression must be closed with )
		if (token.type != rparensym)
		{
			printparseerror(12);
			exit(0);
		}

		getnexttoken();
	}
	else
	{
		printparseerror(11);
		exit(0);
	}
}

lexeme getnexttoken()
{
	return token = tokens[++tokenIndex];
}

void mark()
{
	for (int i = tableIndex - 1; i >= 0; i--)
	{
		if (!table[i].mark)
		{
			if (table[i].level == level)
				table[i].mark = 1;
			else if (table[i].level < level)
				break;
		}
	}
}

int findsymbol(lexeme token, int kind)
{
	int symIdx = -1, mxLevel = -1;

	for (int i = tableIndex - 1; i >= 0; i--)
	{
		if (table[i].mark == 0 &&
				table[i].kind == kind &&
				strcmp(token.name, table[i].name) == 0 &&
				table[i].level > mxLevel)
		{
			symIdx = i;
			mxLevel = table[i].level;
		}
	}

	return symIdx;
}

int multipledeclarationcheck(lexeme token)
{
	for (int i = tableIndex - 1; i >= 0; i--)
	{
		if (table[i].mark == 0 &&
				table[i].level == level &&
				strcmp(token.name, table[i].name) == 0)
			return i;
	}
	return -1;
}

void emit(int opcode, int l, int m)
{
	code[codeIndex].opcode = opcode;
	code[codeIndex].l = l;
	code[codeIndex].m = m;
	codeIndex++;
}

void addToSymbolTable(int k, char n[], int v, int l, int a, int m)
{
	table[tableIndex].kind = k;
	strcpy(table[tableIndex].name, n);
	table[tableIndex].val = v;
	table[tableIndex].level = l;
	table[tableIndex].addr = a;
	table[tableIndex].mark = m;
	tableIndex++;
}


void printparseerror(int err_code)
{
	switch (err_code)
	{
		case 1:
			printf("Parser Error: Program must be closed by a period\n");
			break;
		case 2:
			printf("Parser Error: Constant declarations should follow the pattern 'ident := number {, ident := number}'\n");
			break;
		case 3:
			printf("Parser Error: Variable declarations should follow the pattern 'ident {, ident}'\n");
			break;
		case 4:
			printf("Parser Error: Procedure declarations should follow the pattern 'ident ;'\n");
			break;
		case 5:
			printf("Parser Error: Variables must be assigned using :=\n");
			break;
		case 6:
			printf("Parser Error: Only variables may be assigned to or read\n");
			break;
		case 7:
			printf("Parser Error: call must be followed by a procedure identifier\n");
			break;
		case 8:
			printf("Parser Error: if must be followed by then\n");
			break;
		case 9:
			printf("Parser Error: while must be followed by do\n");
			break;
		case 10:
			printf("Parser Error: Relational operator missing from condition\n");
			break;
		case 11:
			printf("Parser Error: Arithmetic expressions may only contain arithmetic operators, numbers, parentheses, constants, and variables\n");
			break;
		case 12:
			printf("Parser Error: ( must be followed by )\n");
			break;
		case 13:
			printf("Parser Error: Multiple symbols in variable and constant declarations must be separated by commas\n");
			break;
		case 14:
			printf("Parser Error: Symbol declarations should close with a semicolon\n");
			break;
		case 15:
			printf("Parser Error: Statements within begin-end must be separated by a semicolon\n");
			break;
		case 16:
			printf("Parser Error: begin must be followed by end\n");
			break;
		case 17:
			printf("Parser Error: Bad arithmetic\n");
			break;
		case 18:
			printf("Parser Error: Confliciting symbol declarations\n");
			break;
		case 19:
			printf("Parser Error: Undeclared identifier\n");
			break;
		default:
			printf("Implementation Error: unrecognized error code\n");
			break;
	}

	free(code);
	free(table);
}

void printsymboltable()
{
	int i;
	printf("Symbol Table:\n");
	printf("Kind | Name        | Value | Level | Address | Mark\n");
	printf("---------------------------------------------------\n");
	for (i = 0; i < tableIndex; i++)
		printf("%4d | %11s | %5d | %5d | %5d | %5d\n", table[i].kind, table[i].name, table[i].val, table[i].level, table[i].addr, table[i].mark);

	free(table);
	table = NULL;
}

void printassemblycode()
{
	int i;
	printf("Line\tOP Code\tOP Name\tL\tM\n");
	for (i = 0; i < codeIndex; i++)
	{
		printf("%d\t", i);
		printf("%d\t", code[i].opcode);
		switch (code[i].opcode)
		{
			case 1:
				printf("LIT\t");
				break;
			case 2:
				switch (code[i].m)
				{
					case 0:
						printf("RTN\t");
						break;
					case 1:
						printf("NEG\t");
						break;
					case 2:
						printf("ADD\t");
						break;
					case 3:
						printf("SUB\t");
						break;
					case 4:
						printf("MUL\t");
						break;
					case 5:
						printf("DIV\t");
						break;
					case 6:
						printf("ODD\t");
						break;
					case 7:
						printf("MOD\t");
						break;
					case 8:
						printf("EQL\t");
						break;
					case 9:
						printf("NEQ\t");
						break;
					case 10:
						printf("LSS\t");
						break;
					case 11:
						printf("LEQ\t");
						break;
					case 12:
						printf("GTR\t");
						break;
					case 13:
						printf("GEQ\t");
						break;
					default:
						printf("err\t");
						break;
				}
				break;
			case 3:
				printf("LOD\t");
				break;
			case 4:
				printf("STO\t");
				break;
			case 5:
				printf("CAL\t");
				break;
			case 6:
				printf("INC\t");
				break;
			case 7:
				printf("JMP\t");
				break;
			case 8:
				printf("JPC\t");
				break;
			case 9:
				switch (code[i].m)
				{
					case 1:
						printf("WRT\t");
						break;
					case 2:
						printf("RED\t");
						break;
					case 3:
						printf("HAL\t");
						break;
					default:
						printf("err\t");
						break;
				}
				break;
			default:
				printf("err\t");
				break;
		}
		printf("%d\t%d\n", code[i].l, code[i].m);
	}
	if (table != NULL)
		free(table);
}
