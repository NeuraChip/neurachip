#!/bin/bash

# Args
EXP_VERSION="v101"

# tile_size_array=(4 8 16)
tile_size_array=(4)
# tile_size_array=(4 8 16 32 64)

dataset_array=("cora" "citeseer")

CHIPPY_BIN=./chippy.bin

# Check if chippy.bin exists
if [ ! -f "$CHIPPY_BIN" ]; then
    echo "chippy.bin not found. Please build chippy first."
    exit 1
fi

# Experiment
# Iterate over tile sizes
for dataset_iter in "${dataset_array[@]}"
do
    exp_name="Dataset_Experiment_${EXP_VERSION}_${dataset_iter}"
    results_dir="./results_neurachip/${exp_name}"
    tile_size=4
    rm -rf ${results_dir}
    mkdir -p ${results_dir}
    description="Dataset_${dataset_iter}"
    dataset=${dataset_iter}
    cache_lines_count=16384
    echo "Running experiment ${exp_name} with description ${description}"
    # Run experiment
    ${CHIPPY_BIN} \
                    --dataset ${dataset} \
                    --cache_lines_count ${cache_lines_count} \
                    --tile_size ${tile_size} \
                    --exp_name ${exp_name} \
                    --description ${description} \
                    | tee ${results_dir}/sim_out.txt &


done


