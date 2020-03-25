#!/bin/bash

rm -rf log
rm -rf results
mkdir results

make clean
make build

SCORE1=0
SCORE2=0
SCOREB=0

EXEC=./editor

ZERO=0

IN_FILE="tests/test1"
CORRECT_FILE="tests/test1.ref"
OUT_FILE="results/test1.out"

cat $IN_FILE | $EXEC $OUT_FILE 1>&2
if [ -f $OUT_FILE ];
then
    echo "-0.2: Testul 1"
    echo "Testul 1 picat!"
else
    SCORE1=$(awk -v SCORE1=$SCORE1 'BEGIN { print SCORE1 + 0.2 }')
    echo "Testul 1 trecut!"
fi

for i in {2..20}
do
	IN_FILE="tests/test"$i
	CORRECT_FILE="tests/test"$i".ref"
	OUT_FILE="results/test"$i".out"
    cat $IN_FILE | $EXEC $OUT_FILE 1>&2
    diff "$OUT_FILE" "$CORRECT_FILE" > /dev/null
    EXIT_CODE=$?
    if [[ $EXIT_CODE -eq $ZERO ]]
    then
	SCORE1=$(awk -v SCORE1=$SCORE1 'BEGIN { print SCORE1 + 0.2 }')
	echo "Testul $i trecut!"
    else
	echo "-0.2: Testul $i"
	echo "Testul $i picat!"
    fi
done

for i in {21..36}
do
    IN_FILE="tests/test"$i
    CORRECT_FILE="tests/test"$i".ref"
    OUT_FILE="results/test"$i".out"
    cat $IN_FILE | $EXEC $OUT_FILE 1>&2
    diff "$OUT_FILE" "$CORRECT_FILE" > /dev/null
    EXIT_CODE=$?
    if [[ $EXIT_CODE -eq $ZERO ]]
    then
	SCORE2=$(awk -v SCORE2=$SCORE2 'BEGIN { print SCORE2 + 0.3125 }')
	echo "Testul $i trecut!"
    else
	echo "-0.3125: Testul $i"
	echo "Testul $i picat!"
    fi
done


IN_FILE="tests/test37"
CORRECT_FILE="tests/test37.ref"
OUT_FILE="results/test37.out"

VALGRIND_ARGS="--leak-check=full --error-exitcode=2"

cat $IN_FILE | valgrind $VALGRIND_ARGS $EXEC $OUT_FILE 1>&2
EXIT_CODE=$?

DOI=2
if [[ $EXIT_CODE -eq $DOI ]]
then
    echo "-0.5: Testul 37 (Memory leaks)"
    echo "Testul 37 picat!"
    SCORE2=$(awk -v SCORE2=$SCORE2 'BEGIN { print SCORE2 - 0.5 }')
else
    echo "Testul 37 trecut! (Memory leaks)"
fi

for i in {41..48}
do
    IN_FILE="tests/test"$i
    CORRECT_FILE="tests/test"$i".ref"
    OUT_FILE="results/test"$i".out"
    cat $IN_FILE | $EXEC $OUT_FILE 1>&2
    diff "$OUT_FILE" "$CORRECT_FILE" > /dev/null
    EXIT_CODE=$?
    if [[ $EXIT_CODE -eq $ZERO ]]
    then
	SCOREB=$(awk -v SCOREB=$SCOREB 'BEGIN { print SCOREB + 0.25 }')
	echo "+0.25: Testul $i (BONUS)"
	echo "Testul $i trecut!"
    else
	echo "Testul $i picat!"
    fi
done

echo "Cerinta 1 : "$SCORE1
echo "Cerinta 2 : "$SCORE2
echo "BONUS     : "$SCOREB
