SRC = vm.c

OBJS = $(SRC:.c=.o)

# GNU C Compiler
GCC = gcc

# GNU C Linker
LINK = gcc

# Compiler Flags
CFLAGS = -Wall -O3 -g

.PHONY: clean

all : main

main: $(OBJS)
	$(LINK) -o $@ $^ $(LIBS)

clean:
	rm -rf *.out *.o *.d main output.txt

tar: clean
	tar zcvf P-machine.tgz *.c Makefile README.md

help:
	@echo "	make main	- same as make all"
	@echo "	make all	- builds the main target"
	@echo "	make clean	- remove .o .d main *.out output.txt"
	@echo "	make tar	- make a tarball of the project"
	@echo "	make help	- this message"

include $(SOURCE:.c=.d)

%.d: %.c
	@set -e; rm -rf $@;$(GCC) -MM $< $(CFLAGS) > $@
