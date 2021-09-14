// Student Name: Cameron Custer
#include <stdio.h>

// maximum process address space size
const int MAX_PAS_LENGTH = 500;

// hard coded operation names
const char *opnames[] = {"", "LIT", "OPR", "LOD", "STO", "CAL", "INC", "JMP",
	"JPC", "SYS"};

const char *opr_opnames[] = {"RTN", "NEG", "ADD", "SUB", "MUL", "DIV", "ODD",
	"MOD", "EQL", "NEQ", "LSS", "LEQ", "GTR", "GEQ"};

// print function
void print_execution(int line, const char *opname, int *IR, int PC, int BP,
		int SP, int DP, int *pas, int GP) {

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
	int pas[MAX_PAS_LENGTH];
	for (int i = 0; i < MAX_PAS_LENGTH; i++)
		pas[i] = 0;
	int IR[3];

	// program input
	FILE *fin = fopen(argv[1], "r");

	// read the program into the text section of the process address space
	while (fscanf(fin, "%d %d %d", &pas[PC], &pas[PC + 1], &pas[PC + 2]) != EOF)
		PC += 3;

	// close program
	fclose(fin);

	// program set-up
	DP = PC - 1;
	GP = PC;
	BP = PC;
	PC = 0;

	// output header and initial values
	printf("\t\t\t\tPC\tBP\tSP\tDP\tdata\n");
	printf("Initial values:\t\t\t%d\t%d\t%d\t%d\n", PC, BP, SP, DP);

	int halt_flag = 1;
	while (halt_flag == 1) {

		// FETCH
		// one instruction (3 values) per line
		int line = PC / 3;

		IR[0] = pas[PC], IR[1] = pas[PC + 1], IR[2] = pas[PC + 2];

		// increment the program counter to the next instruction
		PC += 3;

		// EXECUTE
		switch (IR[0]) {
			// LIT
			case 1:
				if (BP == GP) {
					DP += 1;
					pas[DP] = IR[2];
				}
				else {
					SP -= 1;
					pas[SP] = IR[2];
				}
				break;

				// OPR
			case 2:
				switch (IR[2]) {
					// RTN
					case 0:
						SP = BP + 1;
						BP = pas[SP - 2];
						PC = pas[SP - 3];
						break;

						// NEG
					case 1:
						if (BP == GP)
							pas[DP] *= -1;
						else
							pas[SP] *= -1;
						break;

						// ADD
					case 2:
						if (BP == GP)
							pas[DP] += pas[DP + 1];
						else {
							SP++;
							pas[SP] += pas[SP - 1];
						}
						break;

						// SUB
					case 3:
						if (BP == GP) {
							DP--;
							pas[DP] -= pas[DP + 1];
						}
						else {
							SP++;
							pas[SP] -= pas[SP - 1];
						}
						break;

						// MUL
					case 4:
						if (BP == GP) {
							DP--;
							pas[DP] *= pas[DP + 1];
						}
						else {
							SP++;
							pas[SP] *= pas[SP - 1];
						}
						break;

						// DIV
					case 5:
						if (BP == GP) {
							DP--;
							pas[DP] /= pas[DP + 1];
						}
						else {
							SP++;
							pas[SP] /= pas[SP - 1];
						}
						break;

						// ODD
					case 6:
						if (BP == GP)
							pas[DP] %= 2;
						else
							pas[SP] %= 2;
						break;

						// MOD
					case 7:
						if (BP == GP) {
							DP--;
							pas[DP] %= pas[DP + 1];
						}
						else {
							SP++;
							pas[SP] %= pas[SP - 1];
						}
						break;

						// EQL
					case 8:
						if (BP == GP) {
							DP--;
							pas[DP] = pas[DP] == pas[DP + 1];
						}
						else {
							SP++;
							pas[SP] = pas[SP] == pas[SP - 1];
						}
						break;

						// NEQ
					case 9:
						if (BP == GP) {
							DP--;
							pas[DP] = pas[DP] != pas[DP + 1];
						}
						else {
							SP++;
							pas[SP] = pas[SP] != pas[SP - 1];
						}
						break;

						// LSS
					case 10:
						if (BP == GP) {
							DP--;
							pas[DP] = pas[DP] < pas[DP + 1];
						}
						else {
							SP++;
							pas[SP] = pas[SP] < pas[SP - 1];
						}
						break;

						// LEQ
					case 11:
						if (BP == GP) {
							DP--;
							pas[DP] = pas[DP] <= pas[DP + 1];
						}
						else {
							SP++;
							pas[SP] = pas[SP] <= pas[SP - 1];
						}
						break;

						// GTR
					case 12:
						if (BP == GP) {
							DP--;
							pas[DP] = pas[DP] > pas[DP + 1];
						}
						else {
							SP++;
							pas[SP] = pas[SP] > pas[SP - 1];
						}
						break;

						// GEQ
					case 13:
						if (BP == GP) {
							DP--;
							pas[DP] = pas[DP] >= pas[DP + 1];
						}
						else {
							SP++;
							pas[SP] = pas[SP] >= pas[SP - 1];
						}
						break;
				}
				break;

				// LOD
			case 3:
				if (BP == GP) {
					DP++;
					pas[DP] = pas[GP + IR[2]];
				}
				else {
					SP--;
					if (base(pas, BP, IR[1]) == GP)
						pas[SP] = pas[GP + IR[2]];
					else
						pas[SP] = pas[base(pas, BP, IR[1]) - IR[2]];
				}
				break;

				// STO
			case 4:
				if (BP == GP) {
					pas[GP + IR[2]] = pas[DP];
					DP--;
				}
				else {
					if (base(pas, BP, IR[1]) == GP)
						pas[GP + IR[2]] = pas[SP];
					else
						pas[base(pas, BP, IR[1]) - IR[2]] = pas[SP];
					SP++;
				}
				break;

				// CAL
			case 5:
				pas[SP - 1] = base(pas, BP, IR[1]); // static link (SL)
				pas[SP - 2] = BP; // dynamic link (DL)
				pas[SP - 3] = PC; // return address (RA)
				BP = SP - 1;
				PC = IR[2];
				break;

				// INC
			case 6:
				if (BP == GP)
					DP += IR[2];
				else
					SP -= IR[2];
				break;

				// JMP
			case 7:
				PC = IR[2];
				break;

				// JPC
			case 8:
				if (BP == GP) {
					if (pas[DP] == 0)
						PC = IR[2];
					DP--;
				}
				else {
					if (pas[SP] == 0)
						PC = IR[2];
					SP++;
				}
				break;

				// SYS
			case 9:
				switch (IR[2]) {
					// SYS 0, 1
					case 1:
						if (BP == GP) {
							printf("Top of Stack Value: %d\n", pas[DP]);
							DP--;
						}
						else {
							printf("Top of Stack Value: %d\n", pas[SP]);
							SP++;
						}
						break;

						// SYS 0, 2
					case 2:
						printf("Please Enter an Integer: ");
						if (BP == GP) {
							DP++;
							scanf("%d", &pas[DP]);
						}
						else {
							SP--;
							scanf("%d", &pas[SP]);
						}
						printf("\n");
						break;

						// SYS 0, 3
					case 3:
						halt_flag = 0;
						break;
				}
				break;
		}

		// print the state of the executing program
		if (IR[0] == 2)
			print_execution(line, opr_opnames[IR[2]], IR, PC, BP, SP, DP, pas,
					GP);
		else
			print_execution(line, opnames[IR[0]], IR, PC, BP, SP, DP, pas, GP);
	}
}
