#!/bin/bash

# $1 - pocet cisel v souboru numbers
# kontrola na pocet argumentu
if [ $# -ne 1 ]; then
    echo "Použití: $0 <počet čísel>"
  exit 1;
fi;

n=$1
# Validace, že n je kladné číslo 
if [[ ! $n =~ ^[0-9]+$ ]]; then
    echo "Error: Vstup '$n' neni cislo nebo kladné!" >&2
    exit 1
fi

if [ "$n" -eq 0 ]; then
    echo "Error: Vstup musi byt cislo vetsi nez 0!" >&2
    exit 1
fi

numProc=$(( 2 * n - 1 ))

# preklad
mpic++ -o mes mes.cpp

# vygenerovani nahodne posloupnosti cisel, pocet dan prvnim parametrem skriptu
dd if=/dev/random bs=1 count=$1 of=numbers 2>/dev/null
# spusteni aplikace (oversubscribe - vice procesu nez fyzicky k dispozici)
mpirun --oversubscribe -np $numProc mes

# uklid
rm -f mes numbers

