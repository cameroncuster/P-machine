/* Program Header
   Student Name: Cameron Custer
 */
#include <stdio.h>
#include <stdlib.h>

const int MAX_PAS_LENGTH = 501;

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
	int PC = 0, BP = 60, SP = 500, DP = 59, GP;
	int *pas = (int *)malloc(MAX_PAS_LENGTH * sizeof(int));

	// program input
	FILE *fin = fopen(argv[0], "r");

	printf("\t\tPC\tBP\tSP\tDP\tdata\n");

	// execution
	int OP, L, M;
	while (fscanf(fin, "%d %d %d", &OP, &L, &M) != EOF) {
		// FETCH

		// EXECUTE
	}

	// close program
	fclose(fin);
}
