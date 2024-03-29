#!/bin/bash

# This script is used to run chippy.bin in a loop

# Path to chippy.bin
CHIPPY_BIN="/home/ktb/git/NeuraChip/NeuraSim/chippy.bin"

TOTAL_RUNS=256

for ((i=1; i<=$TOTAL_RUNS; i++))
do
	echo "Run $i"
	# Make i a 3 digit number
	k=$i
	if [ $i -lt 10 ]
	then
		k="00$i"
	elif [ $i -lt 100 ]
	then
		k="0$i"
	fi
	# Experiment name and description
	EXP_NAME="NeuraChip_V$k"
	EXP_DESC="\"Loop run $k\""
	$CHIPPY_BIN -n${EXP_NAME} -d${EXP_DESC} > /dev/null &
done