#!/bin/bash

# This scripts checks if the user provided valid config_id_<min/max> values
# >>> If not a single already existing config files matches the provided values
# the script will exit with error code 1

# >>> If the config_id_min value does not match any existing config file but the config
# file was found then the value associated with that file will be used for further
# operations i.e. creating a bifurcation plot 

# >>> If the config_id_max value does not match any exisitng config file but the config
# file was found then the value associated with that file will be used for further
# operations i.e. creating a bifurcation plot

# *** example of usage ***
# bash perf_plot_bifur.sh 1127 2500 "png"
#   config file that was found: config-0001000-0001999.sh
# the values used for creating a bifurcation plot will be:
# config_id_min_to_use=1127, config_id_max_to_use=1999
# It means that even though the config file that's getting sourced ranges 
# from 1000 to 1999 only data files from 1127 to 1999 will be used to ensure
# that plotting range does not exeed the user's input values (please note that
# the config_id range provided by the user may be shrinked to avoid errors and
# possible creation of plots not corresponding to ONLY one configuration set)

# -----------------------------------------------------------------------------
config_id_min=$1            # Config ID to start at (inclusive)
config_id_max=$2            # Config ID to finish at (inclusive)
graphic_file_extension=$3   # Extension of the graphic files with bifurcation plots
# n_last_iter=$4            # Number of last iterations to plot !!!!!!!NA RAZIE ODCZYTUJE Z PLIKU CONFIG.sh
# -----------------------------------------------------------------------------

# Checks if user passed exactly 3 arguments
if [ "$#" -ne 3 ]; then
    echo "error: Invalid number of arguments"
    echo "try: bash perf_plot_bifur.sh <config_id_start> <config_id_stop> <graphic_file_extension> (\"png\" is preferable)"
    exit 1
fi

FILE_CONFIG_ID_LIST="$PROJECT/parameters/configs/config_id_list.txt"

# config_id_<min/max>_to_use are initially set to the provided values
config_id_min_to_use=$config_id_min
config_id_max_to_use=$config_id_max

is_config_found=0 # =1 means that the program already found the corresponfing config file

for i in $(seq $config_id_min $config_id_max); do
    if (( $is_config_found==0 )); then
        while read -r val1 val2; do
            if (( $i >= $val1 && $i <= $val2 )); then
                val1_found=$val1
                val2_found=$val2
                val1_found_padded=$(printf "%07g" $val1_found)
                val2_found_padded=$(printf "%07g" $val2_found)
                is_config_found=1
                break
            fi
        done < "$FILE_CONFIG_ID_LIST"
    else
        break
    fi
done

if (( $is_config_found==1 )); then
    if (( $val1_found>$config_id_min )); then
        echo "<config_id_min>=$config_id_min does not match configuration file that was found"
        echo "value of $val1_found will be set as new <config_id_min> to perform further operations"
        config_id_min_to_use=$val1_found
    fi

    if (( $val2_found<$config_id_max )); then
        echo "<config_id_max>=$config_id_max does not match configuration file that was found"
        echo "value of $val2_found will be set as new <config_id_max> to perform further operations"
        config_id_max_to_use=$val2_found
    fi
else
    echo "Not a single configuration file match the values of <config_id_min> <config_id_max> provided by the user"
    echo "Aborting operation..."
    exit 1
fi

CONFIG_FILE_FOUND=$(find "$PROJECT/parameters/configs/" -name "config-$val1_found_padded-$val2_found_padded.sh")

if [[ -f $CONFIG_FILE_FOUND ]]; then
    source $CONFIG_FILE_FOUND
    echo "Matching configuration file was found:"
    echo "$CONFIG_FILE_FOUND"
    echo "Values provided by the user: $config_id_min $config_id_max"
    echo "Values passed for further operations: $config_id_min_to_use $config_id_max_to_use"

    sbatch --time="$PLOT_BIFUR_TIME" "$PROJECT/scripts/slurm/plot_bifur.slurm" "$config_id_min_to_use" "$config_id_max_to_use" "$graphic_file_extension" "$CONFIG_FILE_FOUND"
else
    echo "File $CONFIG_FILE_FOUND does not exist"
    exit 1
fi