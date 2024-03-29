#!/bin/bash

make clean
# You dont need to make only_object; It will automatically make only_object
make -j256 only_target
# ./chippy.bin -n"NeuraChip_V002" -d"Visualizer Testing"
