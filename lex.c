#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "compiler.h"
#define MAX_NUMBER_TOKENS 500
#define MAX_IDENT_LEN 11
#define MAX_NUMBER_LEN 5

// hard coded keywords and symbols
const char *keywords[] =
{
	"const",
	"var",
	"procedure",
	"begin",
	"end",
	"while",
	"do",
	"if",
	"then",
	"else",
	"call",
	"write",
	"read",
	"0identifier", // place holder for enum token_type translation
	"0number", // place holder for enum token_type translation
	":=",
	"+",
	"-",
	"*",
	"/",
	"%",
	"==",
	"!=",
	"<",
	"<=",
	">",
	">=",
	"odd",
	"(",
	")",
	",",
	".",
	";"
};

// dynamic list
lexeme *list;
int lex_index;
int size;

// buffer
char buff[12];

// prototypes
void printlexerror(int type);
void printtokens();
void expand();
void nullify();

// tokenizer
lexeme *lexanalyzer(char *input, int printFlag)
{
	// allocate single space and expand as necessary
	list = malloc(sizeof(lexeme));
	lex_index = 0;
	size = 1;

	// iterate the characters in the program
	for (int c = 0; input[c] != '\0';)
	{
		// ignore white space
		if (isspace(input[c]) || iscntrl(input[c]))
			c++;

		else if (isalpha(input[c]))
		{
			// capture the next 12 characters
			for (int buff_ptr = 0; isalpha(input[c]) || isdigit(input[c]); c++, buff_ptr++)
			{
				if (buff_ptr == 11)
				{
					printlexerror(4);
					return NULL;
				}
				buff[buff_ptr] = input[c];
			}

			// check for reserved word and move onto the next token
			int is_keyword = 0;
			for (int i = 0; i < 32 && !is_keyword; i++)
			{
				// add keyword
				if (strcmp(keywords[i], buff) == 0)
				{
					is_keyword = 1;
					strcpy(list[lex_index].name, buff);
					list[lex_index].type = i + 1;
					lex_index++;
					break;
				}
			}

			// add identifier
			if (!is_keyword)
			{
				strcpy(list[lex_index].name, buff);
				list[lex_index].type = identsym;
				lex_index++;
			}
		}

		else if (isdigit(input[c]))
		{
			// capture the next numeral
			for (int buff_ptr = 0; isdigit(input[c]); c++, buff_ptr++)
			{
				if (buff_ptr == 5)
				{
					printlexerror(3);
					return NULL;
				}
				buff[buff_ptr] = input[c];
			}

			// invalid identifier
			if (isalpha(input[c]))
			{
				printlexerror(2);
				return NULL;
			}

			// add literal
			list[lex_index].value = atoi(buff);
			list[lex_index].type = numbersym;
			lex_index++;
		}

		else
		{
			// special characters
			switch (input[c])
			{
				case ':': // ":="
					if (input[c + 1] != '=')
					{
						printlexerror(1);
						return NULL;
					}
					list[lex_index++].type = assignsym;
					c++;
					break;

				case '+':
					list[lex_index++].type = addsym;
					break;

				case '-':
					list[lex_index++].type = subsym;
					break;

				case '*':
					list[lex_index++].type = multsym;
					break;

				case '/':
					// comment
					if (input[c + 1] == '/')
						while (input[c] != '\n')
							c++;
					else
						list[lex_index++].type = divsym;
					break;

				case '%':
					list[lex_index++].type = modsym;
					break;

				case '=': // "=="
					if (input[c + 1] != '=')
					{
						printlexerror(1);
						return NULL;
					}
					list[lex_index++].type = eqlsym;
					c++;
					break;

				case '!': // "!="
					if (input[c + 1] != '=')
					{
						printlexerror(1);
						return NULL;
					}
					list[lex_index++].type = neqsym;
					c++;
					break;

				case '<': // or "<="
					if (input[c + 1] == '=')
					{
						list[lex_index++].type = leqsym;
						c++;
					}
					else
						list[lex_index++].type = lsssym;
					break;

				case '>': // or ">="
					if (input[c + 1] == '=')
					{
						list[lex_index++].type = geqsym;
						c++;
					}
					else
						list[lex_index++].type = gtrsym;
					break;

				case '(':
					list[lex_index++].type = lparensym;
					break;

				case ')':
					list[lex_index++].type = rparensym;
					break;

				case ',':
					list[lex_index++].type = commasym;
					break;

				case '.':
					list[lex_index++].type = periodsym;
					break;

				case ';':
					list[lex_index++].type = semicolonsym;
					break;

				default:
					printlexerror(1);
					return NULL;
			}
			c++;
		}

		// expand the list as necessary the nullify the buffer
		expand();
		nullify();
	}

	if (printFlag)
		printtokens();

	return list;
}

void printtokens()
{
	int i;
	printf("Lexeme Table:\n");
	printf("lexeme\t\ttoken type\n");
	for (i = 0; i < lex_index; i++)
	{
		switch (list[i].type)
		{
			case oddsym:
				printf("%11s\t%d", "odd", oddsym);
				break;
			case eqlsym:
				printf("%11s\t%d", "==", eqlsym);
				break;
			case neqsym:
				printf("%11s\t%d", "!=", neqsym);
				break;
			case lsssym:
				printf("%11s\t%d", "<", lsssym);
				break;
			case leqsym:
				printf("%11s\t%d", "<=", leqsym);
				break;
			case gtrsym:
				printf("%11s\t%d", ">", gtrsym);
				break;
			case geqsym:
				printf("%11s\t%d", ">=", geqsym);
				break;
			case modsym:
				printf("%11s\t%d", "%", modsym);
				break;
			case multsym:
				printf("%11s\t%d", "*", multsym);
				break;
			case divsym:
				printf("%11s\t%d", "/", divsym);
				break;
			case addsym:
				printf("%11s\t%d", "+", addsym);
				break;
			case subsym:
				printf("%11s\t%d", "-", subsym);
				break;
			case lparensym:
				printf("%11s\t%d", "(", lparensym);
				break;
			case rparensym:
				printf("%11s\t%d", ")", rparensym);
				break;
			case commasym:
				printf("%11s\t%d", ",", commasym);
				break;
			case periodsym:
				printf("%11s\t%d", ".", periodsym);
				break;
			case semicolonsym:
				printf("%11s\t%d", ";", semicolonsym);
				break;
			case assignsym:
				printf("%11s\t%d", ":=", assignsym);
				break;
			case beginsym:
				printf("%11s\t%d", "begin", beginsym);
				break;
			case endsym:
				printf("%11s\t%d", "end", endsym);
				break;
			case ifsym:
				printf("%11s\t%d", "if", ifsym);
				break;
			case thensym:
				printf("%11s\t%d", "then", thensym);
				break;
			case elsesym:
				printf("%11s\t%d", "else", elsesym);
				break;
			case whilesym:
				printf("%11s\t%d", "while", whilesym);
				break;
			case dosym:
				printf("%11s\t%d", "do", dosym);
				break;
			case callsym:
				printf("%11s\t%d", "call", callsym);
				break;
			case writesym:
				printf("%11s\t%d", "write", writesym);
				break;
			case readsym:
				printf("%11s\t%d", "read", readsym);
				break;
			case constsym:
				printf("%11s\t%d", "const", constsym);
				break;
			case varsym:
				printf("%11s\t%d", "var", varsym);
				break;
			case procsym:
				printf("%11s\t%d", "procedure", procsym);
				break;
			case identsym:
				printf("%11s\t%d", list[i].name, identsym);
				break;
			case numbersym:
				printf("%11d\t%d", list[i].value, numbersym);
				break;
		}
		printf("\n");
	}
	printf("\n");
	printf("Token List:\n");
	for (i = 0; i < lex_index; i++)
	{
		if (list[i].type == numbersym)
			printf("%d %d ", numbersym, list[i].value);
		else if (list[i].type == identsym)
			printf("%d %s ", identsym, list[i].name);
		else
			printf("%d ", list[i].type);
	}
	printf("\n");
	list[lex_index++].type = -1;
}

void printlexerror(int type)
{
	if (type == 1)
		printf("Lexical Analyzer Error: Invalid Symbol\n");
	else if (type == 2)
		printf("Lexical Analyzer Error: Invalid Identifier\n");
	else if (type == 3)
		printf("Lexical Analyzer Error: Excessive Number Length\n");
	else if (type == 4)
		printf("Lexical Analyzer Error: Excessive Identifier Length\n");
	else
		printf("Implementation Error: Unrecognized Error Type\n");

	free(list);
	return;
}

void expand()
{
	if (lex_index >= size)
	{
		size *= 2;
		list = realloc(list, size * sizeof(lexeme));
	}
}

void nullify()
{
	for (int i = 0; i < 12; i++)
		buff[i] = '\0';
}
