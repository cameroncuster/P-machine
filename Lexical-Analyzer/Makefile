SRC = driver.c lex.c

OBJS = $(SRC:.c=.o)

# GNU C Compiler
GCC = gcc

# GNU C Linker
LINK = gcc

# Compiler Flags
CFLAGS = -Wall -O3 -g

.PHONY: clean

all : tokenize

tokenize: $(OBJS)
	$(LINK) -o $@ $^ $(LIBS)

clean:
	rm -rf *.out *.o *.d tokenize

tar: clean
	tar zcvf Lexical-Analyzer.tgz *.h *.c Makefile README.md

help:
	@echo "	make tokenize	- same as make all"
	@echo "	make all		- builds the main target"
	@echo "	make clean		- remove .o .d tokenize *.out"
	@echo "	make tar		- make a tarball of the project"
	@echo "	make help		- this message"

include $(SOURCE:.c=.d)

%.d: %.c
	@set -e; rm -rf $@;$(GCC) -MM $< $(CFLAGS) > $@
