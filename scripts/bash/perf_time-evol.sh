#!/bin/bash
sbatch --array=0-$(( $2 - $1 ))%1000 "$PROJECT/scripts/slurm/time-evol.slurm" "$1" "$2"
