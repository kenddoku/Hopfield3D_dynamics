#!/bin/bash
sbatch --array=0-$(( $2-$1 ))%1000 "$PROJECT/scripts/slurm/plot_time-evol.slurm" "$1" "$2" "$3" "$4" "$5"
