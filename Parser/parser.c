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
int Level;
int token_idx;

// parser members
void program(lexeme *list);
void block(lexeme *list);
void const_declaration(lexeme *list);
int var_declaration(lexeme *list);
void procedure_declaration(lexeme *list);
void statement(lexeme *list);

// helpers
void mark();
void emit(int opname, int level, int mvalue);
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

	// parse the program
	program(list);

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

void program(lexeme *list)
{
	// start at the beginning
	token_idx = 0;

	// JMP
	emit(7, 0, 0);

	// main function
	addToSymbolTable(3, "main", 0, -1, 0, 0);
	Level = -1;

	// build out the block
	block(list);

	// assert period ending (poor syntactic structure)
	if (list[token_idx].type != periodsym)
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

void block(lexeme *list)
{
	Level++;

	int procedure_idx = tIndex - 1;

	// declarations
	const_declaration(list);
	int numVars = var_declaration(list);
	procedure_declaration(list);

	// addressing
	table[procedure_idx].addr = cIndex * 3;

	if (Level == 0)
		// INC to main
		emit(6, Level, numVars);
	else
		// INC -> alloc space for the activation record
		// (AR: static link (SL) dynamic link (DL) return address (RA))
		emit(6, Level, numVars + 3);

	// statements (non-declarative)
	statement(list);

	mark();

	Level--;
}

void const_declaration(lexeme *list) {
	if (list[token_idx].type == constsym) {
	}
}

int var_declaration(lexeme *list) {
	int numVars = 0;
	if (list[token_idx].type == varsym) {
	}
	return numVars;
}

void procedure_declaration(lexeme *list) {
	while (list[token_idx].type == procsym) {
	}
}

void statement(lexeme *list) {
	if (list[token_idx].type == identsym) {
	}
	else if (list[token_idx].type == beginsym) {
	}
	else if (list[token_idx].type == ifsym) {
	}
	else if (list[token_idx].type == whilesym) {
	}
	else if (list[token_idx].type == readsym) {
	}
	else if (list[token_idx].type == writesym) {
	}
	else if (list[token_idx].type == callsym) {
	}
	else if (list[token_idx].type == callsym) {
	}
}

void mark() {
	for (int i = tIndex - 1; i >= 0; i--) {
		if (!table[i].mark) {
			if (table[i].level == Level)
				table[i].mark = 1;
			else if (table[i].level < Level)
				break;
		}
	}
}

void emit(int opname, int level, int mvalue)
{
	code[cIndex].opcode = opname;
	code[cIndex].l = level;
	code[cIndex].m = mvalue;
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
