#!/bin/bash

# compile
make
compiled=$?

# check if code compiles
if [[ $compiled != 0 ]]; then
    echo "does not compile"
    exit
fi

for i in {1..19}; do
    ./compiler error$i.txt > output.txt
    diff -wiB outerror$i.txt output.txt
    correct=$?
    if [[ $correct == 0 ]]; then
        echo "Error $i: PASS"
    fi
done

# symbol table no proc
./compiler sym1.txt -a -s > output.txt
diff -wiB outsym1.txt output.txt
correct=$?
if [[ $correct == 0 ]]; then
    echo "Symbol Table 1: PASS"
fi

# symbol table with proc
./compiler sym2.txt -a -s > output.txt
diff -wiB outsym2.txt output.txt
correct=$?
if [[ $correct == 0 ]]; then
    echo "Symbol Table 2: PASS"
fi

# while
./compiler while.txt -a -s > output.txt
diff -wiB outwhile.txt output.txt
correct=$?
if [[ $correct == 0 ]]; then
    echo "While: PASS"
fi

# if
./compiler if.txt -a -s > output.txt
diff -wiB outif.txt output.txt
correct=$?
if [[ $correct == 0 ]]; then
    echo "If: PASS"
fi

# read/write
./compiler rw.txt -a -s <<< "3 4" > output.txt
diff -wiB outrw.txt output.txt
correct=$?
if [[ $correct == 0 ]]; then
    echo "Read/Write: PASS"
fi

# arithmetic
./compiler arithmetic.txt -a -s > output.txt
diff -wiB outarithmetic.txt output.txt
correct=$?
if [[ $correct == 0 ]]; then
    echo "Arithmetic: PASS"
fi

# procedures
./compiler proc.txt -a -s <<< "12345 4 5" > output.txt
diff -wiB outproc.txt output.txt
correct=$?
if [[ $correct == 0 ]]; then
    echo "Procedures: PASS"
fi
