/* Program Header
   Student Name: Cameron Custer
 */
#include <stdio.h>
#include <stdlib.h>

// maximum process address space size
const int MAX_PAS_LENGTH = 501;

// hard coded operation names one-based
const char *opnames[] = {"", "LIT", "OPR", "LOD", "STO", "CAL", "INC", "JMP",
	"JPC", "SYS"};

// print function
void print_execution(int line, char *opname, int *IR, int PC, int BP, int SP,
		int DP, int *pas, int GP) {

	// print out instruction and registers
	printf("%2d\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t", line, opname, IR[1], IR[2], PC,
			BP, SP, DP);

	// print data section
	for (int i = GP; i <= DP; i++)
		printf("%d ", pas[i]);
	printf("\n");

	// print stack
	printf("\tstack : ");
	for (int i = MAX_PAS_LENGTH - 1; i >= SP; i--)
		printf("%d ", pas[i]);
	printf("\n");
}

// Find base L levels down
int base(int *pas, int BP, int L) {
	int arb = BP; // arb = activation record base
	while (L > 0) { // find base L levels down
		arb = pas[arb];
		L--;
	}
	return arb;
}

int main(int argc, char *argv[]) {

	// init program execution (run-time) env
	int PC = 0, BP, SP = 500, DP, GP;
	int *pas = malloc(MAX_PAS_LENGTH * sizeof(int));
	int *IR = malloc(3 * sizeof(int));

	// program input
	FILE *fin = fopen(argv[1], "r");

	// read the program into the text section of the process address space
	while (fscanf(fin, "%d %d %d", &pas[PC], &pas[PC + 1], &pas[PC + 2]) != EOF)
		PC += 3;

	// close program
	fclose(fin);

	// program set-up
	DP = PC;
	GP = PC + 1;
	BP = PC + 1;
	PC = 0;

	// output header and initial values
	printf("\t\t\tPC\tBP\tSP\tDP\tdata\n");
	printf("Initial values:\t\t%d\t%d\t%d\t%d\n", PC, BP, SP, DP);

	int halt_flag = 1;
	while (halt_flag == 0) {

		// FETCH
		int OP = pas[PC], L = pas[PC + 1], M = pas[PC + 2];

		switch (OP) {
			case 1:
				break;

			case 2:
				break;

			case 3:
				break;

			case 4:
				break;

			case 5:
				break;

			case 6:
				break;

			case 7:
				break;

			case 8:
				break;

			case 9:
				break;
		}

		// EXECUTE

		PC += 3;
	}

	// free memory
	free(pas);
	free(IR);
}
