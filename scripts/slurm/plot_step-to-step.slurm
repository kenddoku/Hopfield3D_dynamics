#!/bin/bash
#SBATCH --job-name=plot2step
#SBATCH --cpus-per-task=1
#SBATCH --mem=8G
#SBATCH --output=/net/pr2/projects/plgrid/plgghopfieldmgr/Masters/logs/plot_step-to-step.out
#SBATCH --error=/net/pr2/projects/plgrid/plgghopfieldmgr/Masters/errors/plot_step-to-step.err

source "$PROJECT/CONFIG.sh"

# This script plots step-to-step diagrams (i.e. current step vs previous step) associated with parameters set from $1 to $2
# and saves the graphic files as step-to-step_x_config-XXXXXXX.$3, ..., time-evol_z_config-XXXXXXX.$3 
# where $3 is third argument passed by the user being a file's extension e.g. $3="pdf", $3="png", ...

PLOT_STEP2_DATA_PATHS=($(seq -f "$DATA_DIR/time-evol/$CONTROL_PARAM_NAME/time-evol_config-%07g.csv" $1 $2))
FIGURES_PATH="$PROJECT/figures"
PLOT_STEP2_X_FIGURE_FILE_PATHS=($(seq -f "$FIGURES_PATH/step-to-step/$CONTROL_PARAM_NAME/step-to-step_x_config-%07g.$3" $1 $2))
PLOT_STEP2_Y_FIGURE_FILE_PATHS=($(seq -f "$FIGURES_PATH/step-to-step/$CONTROL_PARAM_NAME/step-to-step_y_config-%07g.$3" $1 $2))
PLOT_STEP2_Z_FIGURE_FILE_PATHS=($(seq -f "$FIGURES_PATH/step-to-step/$CONTROL_PARAM_NAME/step-to-step_z_config-%07g.$3" $1 $2))

PLOT_STEP2_INDEX=$((SLURM_ARRAY_TASK_ID))

PLOT_STEP2_DATA_PATH="${PLOT_STEP2_DATA_PATHS[$PLOT_STEP2_INDEX]}"
PLOT_STEP2_X_FIGURE_PATH="${PLOT_STEP2_X_FIGURE_FILE_PATHS[$PLOT_STEP2_INDEX]}"
PLOT_STEP2_Y_FIGURE_PATH="${PLOT_STEP2_Y_FIGURE_FILE_PATHS[$PLOT_STEP2_INDEX]}"
PLOT_STEP2_Z_FIGURE_PATH="${PLOT_STEP2_Z_FIGURE_FILE_PATHS[$PLOT_STEP2_INDEX]}"

module load python/3.9.6
module add matplotlib
module load scipy-bundle/2023.11-gfbf-2023b

srun python3 "$SOURCE_CODE_DIR/plot_step-to-step.py" "$N_ITER" "$PLOT_STEP2_DATA_PATH" "$PLOT_STEP2_X_FIGURE_PATH"\
             "$PLOT_STEP2_Y_FIGURE_PATH" "$PLOT_STEP2_Z_FIGURE_PATH" "$3" "$4" "$5"

