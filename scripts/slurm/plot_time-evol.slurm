#!/bin/bash
#SBATCH --job-name=plot_tevol
#SBATCH --cpus-per-task=1
#SBATCH --mem=8G
#SBATCH --output=/net/pr2/projects/plgrid/plgghopfieldmgr/Masters/logs/plot_time-evol.out
#SBATCH --error=/net/pr2/projects/plgrid/plgghopfieldmgr/Masters/errors/plot_time-evol.err

## ZMIEN TO BO TERAZ TO TO W OGOLE NIE DZIALA JAK PROGRAM NIE JEST W STANIE ZNALEZC DOBREGO PLIKU KONFIGURACYJNEEGO
source "$PROJECT/CONFIG.sh"

# This script plots data associated with parameters set from $1 to $2
# and saves the graphic files as time-evol_x_config-XXXXXXX.$3, ..., time-evol_z_config-XXXXXXX.$3 
# where $3 is third argument passed by the user being a file's extension e.g. $3="pdf", $3="png", ...

PLOT_TEVOL_DATA_PATHS=($(seq -f "$DATA_DIR/time-evol/$CONTROL_PARAM_NAME/time-evol_config-%07g.csv" $1 $2))
FIGURES_PATH="$PROJECT/figures"
# PLOT_TEVOL_X_FIGURE_FILE_PATHS=($(seq -f "$FIGURES_PATH/time-evol/$CONTROL_PARAM_NAME/time-evol_x_config-%07g.$3" $1 $2))
# PLOT_TEVOL_Y_FIGURE_FILE_PATHS=($(seq -f "$FIGURES_PATH/time-evol/$CONTROL_PARAM_NAME/time-evol_y_config-%07g.$3" $1 $2))
# PLOT_TEVOL_Z_FIGURE_FILE_PATHS=($(seq -f "$FIGURES_PATH/time-evol/$CONTROL_PARAM_NAME/time-evol_z_config-%07g.$3" $1 $2))
PLOT_TEVOL_XYZ_FIGURE_FILE_PATHS=($(seq -f "$FIGURES_PATH/time-evol/$CONTROL_PARAM_NAME/time-evol_xyz_config-%07g.$3" $1 $2))

PLOT_TEVOL_INDEX=$((SLURM_ARRAY_TASK_ID))

PLOT_TEVOL_DATA_PATH="${PLOT_TEVOL_DATA_PATHS[$PLOT_TEVOL_INDEX]}"
# PLOT_TEVOL_X_FIGURE_PATH="${PLOT_TEVOL_X_FIGURE_FILE_PATHS[$PLOT_TEVOL_INDEX]}"
# PLOT_TEVOL_Y_FIGURE_PATH="${PLOT_TEVOL_Y_FIGURE_FILE_PATHS[$PLOT_TEVOL_INDEX]}"
# PLOT_TEVOL_Z_FIGURE_PATH="${PLOT_TEVOL_Z_FIGURE_FILE_PATHS[$PLOT_TEVOL_INDEX]}"
PLOT_TEVOL_XYZ_FIGURE_PATH="${PLOT_TEVOL_XYZ_FIGURE_FILE_PATHS[$PLOT_TEVOL_INDEX]}"

module load python/3.9.6
module add matplotlib
module load scipy-bundle/2023.11-gfbf-2023b

srun python3 "$SOURCE_CODE_DIR/plot_time-evol.py" "$N_ITER" "$PLOT_TEVOL_DATA_PATH" "$PLOT_TEVOL_XYZ_FIGURE_PATH"\
            "$3" "$4" "$5"
