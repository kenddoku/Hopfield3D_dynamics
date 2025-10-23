#!/bin/bash

DATA_PATHS=($(seq -f "$PROJECT/data/time-evol/$3/time-evol_config-%07g.csv" $1 $2))

for file in "${DATA_PATHS[@]}"; do
    if [[ ! -f $file ]]; then
        echo "$file does not exist"
    fi
done