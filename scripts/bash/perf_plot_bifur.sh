#!/bin/bash

source $PROJECT/CONFIG.sh

# -----------------------------------------------------------------------------
first_conifg_id=$1          # Config ID to start at (inclusive)
second_config_id=$2         # Config ID to finish at (inclusive)
graphic_file_extension=$3   # Extension of the graphic files with bifurcation plots
# n_last_iter=$4            # Number of last iterations to plot !!!!!!!NA RAZIE ODCZYTUJE Z PLIKU CONFIG.sh
# -----------------------------------------------------------------------------

sbatch --time="$PLOT_BIFUR_TIME" "$PROJECT/scripts/slurm/plot_bifur.slurm" "$1" "$2" "$3"
