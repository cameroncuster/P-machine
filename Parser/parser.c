#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "compiler.h"

// preprocessed constants
#define MAX_CODE_LENGTH 1000
#define MAX_SYMBOL_COUNT 100

// class (global) data
instruction *code;
int cIndex;
symbol *table;
int tIndex;
lexeme *tokens;
int tokenIndex;
int level;

// parser members
void program();
void block();
void const_declaration();
int var_declaration();
void procedure_declaration();
void statement();

// helpers
void mark();
lexeme getcurrtoken();
lexeme getnexttoken();
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
	cIndex = 0;
	table = malloc(MAX_SYMBOL_COUNT * sizeof(symbol));
	tIndex = 0;

	tokens = list;
	tokenIndex = 0;

	// parse the program
	program();

	/* this line is EXTREMELY IMPORTANT, you MUST uncomment it
	   when you test your code otherwise IT WILL SEGFAULT in
	   vm.o THIS LINE IS HOW THE VM KNOWS WHERE THE CODE ENDS
	   WHEN COPYING IT TO THE PAS
	 */
	code[cIndex].opcode = -1;

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
	addToSymbolTable(3, "main", 0, -1, 0, 0);
	level = -1;

	// build out the block
	block();

	// assert period ending (poor syntactic structure)
	lexeme token = getnexttoken();
	if (token.type != periodsym)
	{
		printparseerror(1);
		exit(0);
	}

	// HALT (SYS CALL)
	emit(9, 0, 3);

	// restore CALL destination address
	for (int line = 0; line < cIndex; line++)
		if (code[line].opcode == 5)
			code[line].m = table[code[line].m].addr;

	// program start (initial JMP added above)
	code[0].m = table[0].addr;
}

void block()
{
	level++;

	int procedure_idx = tIndex - 1;

	// declarations
	const_declaration();
	int numVars = var_declaration();
	procedure_declaration();

	// addressing
	table[procedure_idx].addr = cIndex * 3;

	if (level == 0)
		// INC to main
		emit(6, level, numVars);
	else
		// INC -> alloc space for the activation record
		// (AR: static link (SL) dynamic link (DL) return address (RA))
		emit(6, level, numVars + 3);

	// statements (non-declarative)
	statement();

	mark();

	level--;
}

void const_declaration()
{
	lexeme token;
	if (getcurrtoken().type == constsym)
	{
		do {
			token = getnexttoken();

			if (token.type != identsym)
			{
				printparseerror(2);
				exit(0);
			}

			int symidx = multipledeclarationcheck(getnexttoken());

			if (symidx != -1)
			{
				printparseerror(18);
				exit(0);
			}

			char *name = getcurrtoken().name;

			token = getnexttoken();

			if (token.type != assignsym)
			{
				printparseerror(2);
				exit(0);
			}

			token = getnexttoken();

			if (token.type != numbersym)
			{
				printparseerror(2);
				exit(0);
			}

			token = getnexttoken();

			addToSymbolTable(1, name, token.value, level, 0, 0);

			token = getnexttoken();

		} while (getcurrtoken().type == commasym);

		if (getcurrtoken().type != semicolonsym)
		{
			if (getcurrtoken().type == identsym)
			{
				printparseerror(13);
				exit(0);
			}
			else
			{
				printparseerror(14);
				exit(0);
			}
		}

		getnexttoken();
	}
}

int var_declaration()
{
	int numVars = 0;
	if (getcurrtoken().type == varsym)
	{
	}
	return numVars;
}

void procedure_declaration()
{
	while (getcurrtoken().type == procsym)
	{
	}
}

void statement()
{
	lexeme token = getcurrtoken();
	if (token.type == identsym)
	{
	}
	else if (token.type == beginsym)
	{
	}
	else if (token.type == ifsym)
	{
	}
	else if (token.type == whilesym)
	{
	}
	else if (token.type == readsym)
	{
	}
	else if (token.type == writesym)
	{
	}
	else if (token.type == callsym)
	{
	}
	else if (token.type == callsym)
	{
	}
}

void mark()
{
	for (int i = tIndex - 1; i >= 0; i--)
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

lexeme getcurrtoken()
{
	return tokens[tokenIndex];
}

lexeme getnexttoken()
{
	return tokens[tokenIndex++];
}

int multipledeclarationcheck(lexeme token)
{
	for (int i = tIndex - 1; i >= 0; i--)
	{
		if (table[i].mark == 0 &&
				table[i].level == level &&
				strcmp(token.name, table[i].name) == 0)
			return i;
		return -1;
	}
}

void emit(int opcode, int l, int m)
{
	code[cIndex].opcode = opcode;
	code[cIndex].l = l;
	code[cIndex].m = m;
	cIndex++;
}

void addToSymbolTable(int k, char n[], int v, int l, int a, int m)
{
	table[tIndex].kind = k;
	strcpy(table[tIndex].name, n);
	table[tIndex].val = v;
	table[tIndex].level = l;
	table[tIndex].addr = a;
	table[tIndex].mark = m;
	tIndex++;
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
	for (i = 0; i < tIndex; i++)
		printf("%4d | %11s | %5d | %5d | %5d | %5d\n", table[i].kind, table[i].name, table[i].val, table[i].level, table[i].addr, table[i].mark);

	free(table);
	table = NULL;
}

void printassemblycode()
{
	int i;
	printf("Line\tOP Code\tOP Name\tL\tM\n");
	for (i = 0; i < cIndex; i++)
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
