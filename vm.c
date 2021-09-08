#include <stdio.h>

int main(int argc, char *argv[]) {
	FILE *fin = fopen(argv[0], "r");

	fclose(fin);
}
