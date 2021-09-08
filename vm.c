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
	int PC = 0, BP = 60, SP = 500, DP = 59, GP = 60;
	int *pas = malloc(MAX_PAS_LENGTH * sizeof(int));
	int *IR = malloc(3 * sizeof(int));

	// program input
	FILE *fin = fopen(argv[1], "r");

	// read the program into the text section of the process address space
	while (fscanf(fin, "%d %d %d", &pas[PC], &pas[PC + 1], &pas[PC + 2]) != EOF)
		PC += 3;

	// close program
	fclose(fin);

	// start at the beginning of the program
	PC = 0;

	// header output
	printf("\t\tPC\tBP\tSP\tDP\tdata\n");

	int halt_flag = 1;
	while (halt_flag == 0) {
		// FETCH

		// EXECUTE
	}

	// free memory
	free(pas);
	free(IR);
}
