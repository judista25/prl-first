#!/bin/bash

# $1 - pocet cisel v souboru numbers
# kontrola na pocet argumentu
if [ $# -ne 1 ]; then
    echo "Použití: $0 <počet čísel>"
  exit 1;
fi;

n=$1
numProc=$(( 2 * n - 1 ))

# preklad
mpic++ -o mes mes.cpp

# vygenerovani nahodne posloupnosti cisel, pocet dan prvnim parametrem skriptu
dd if=/dev/random bs=1 count=$1 of=numbers 2>/dev/null
# spusteni aplikace (oversubscribe - vice procesu nez fyzicky k dispozici)
mpirun --oversubscribe -np $numProc mes

# uklid
rm -f mes numbers

