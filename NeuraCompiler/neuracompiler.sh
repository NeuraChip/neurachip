#!/bin/bash

# This script is used to compile the program for NeuraChip

# cora, citeseer, pubmed
DATASET_NAME=$1
WORKING_THREADS=128

# Create a directory to store the compiled programs called bin
# mkdir -p programs/$DATASET_NAME/bin

python3 precompile.py $DATASET_NAME

# Get total rows of the dataset
total_rows=$(python3 get_total_rows.py $DATASET_NAME)

# Run a loop to compile the program for each row
count=0
for ((i=0; i<$total_rows; i++))
do
    # If 256 threads are running, wait for one to finish
    if [[ $count -eq $WORKING_THREADS ]]; then
        wait -n
        count=$((count-1)) # Decrement count as one thread has completed
    fi

    # Compile the program
    nice -n 10 python3 single_row_spmm_compiler.py $DATASET_NAME $i &

    # Increment the counter
    count=$((count+1))
done

# Wait for any remaining processes
wait

python3 combine_df.py $DATASET_NAME
