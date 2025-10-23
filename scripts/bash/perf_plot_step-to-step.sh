#!/bin/bash
# Arguments: config_id_start, config_id_finish, graphic_file_extension ("pdf"/"png"), iteration_to_start_at, iteration_to_finish_at
sbatch --array=0-$(( $2-$1 ))%1000 "$PROJECT/scripts/slurm/plot_step-to-step.slurm" "$1" "$2" "$3" "$4" "$5"