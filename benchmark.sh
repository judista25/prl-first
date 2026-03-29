#!/bin/bash


LOG_FILE="test_log.md"


echo "# MPI Benchmark Log - Minimum Extraction Sort" > $LOG_FILE
echo "Datum: $(date)" >> $LOG_FILE
echo "Hardware: Apple M3" >> $LOG_FILE
echo "" >> $LOG_FILE
echo "| n (prvky) | Procesy | Real Time (s) | User Time (s) | Sys Time (s) | Status |" >> $LOG_FILE
echo "| :--- | :--- | :--- | :--- | :--- | :--- |" >> $LOG_FILE

TEST_VALUES=(10 20 50 100 200)

echo "Spouštím benchmark pro n = ${TEST_VALUES[*]}..."

for n in "${TEST_VALUES[@]}"; do
    numProc=$(( 2 * n - 1 ))
    echo "Testování n = $n ($numProc procesů)..."

    stats=$( { TIMEFORMAT="%R %U %S"; time ./runner.sh $n > /dev/null; } 2>&1 )
    
    read real user sys <<< "$stats"

    if [ $? -eq 0 ]; then
        status="OK"
    else
        status="FAILED/LIMIT"
    fi

    echo "| $n | $numProc | ${real}s | ${user}s | ${sys}s | $status |" >> $LOG_FILE
done

echo "Benchmark dokončen. Výsledky uloženy v $LOG_FILE"