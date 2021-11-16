SRC = driver.c vm.c lex.c parser.c

OBJS = $(SRC:.c=.o)

# GNU C Compiler
GCC = gcc

# GNU C Linker
LINK = gcc

# Compiler Flags
CFLAGS = -Wall -O2 -g

.PHONY: clean

all : compiler

compiler: $(OBJS)
	$(LINK) -o $@ $^ $(LIBS)

clean:
	rm -rf *.o *.d compiler

tar: clean
	tar zcvf PL-0.tgz *.h *.c Makefile README.md

help:
	@echo "	make compiler	- same as make all"
	@echo "	make all		- builds the main target"
	@echo "	make clean		- remove .o .d compiler"
	@echo "	make tar		- make a tarball of the project"
	@echo "	make help		- this message"

include $(SOURCE:.c=.d)

%.d: %.c
	@set -e; rm -rf $@;$(GCC) -MM $< $(CFLAGS) > $@
