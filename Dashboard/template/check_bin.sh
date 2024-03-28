#!/bin/bash

FILE=/home/ktb/git/neurachip_deploy/NeuraSim/chippy.bin

if [ -f "$FILE" ]; then
    echo "1"
else
    echo "0"
fi
