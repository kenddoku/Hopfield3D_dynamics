#!/bin/bash
#SBATCH --job-name=plot_tevol
#SBATCH --cpus-per-task=1
#SBATCH --mem=8G
#SBATCH --output=/net/pr2/projects/plgrid/plgghopfieldmgr/Masters/logs/plot_%A_%a.out
#SBATCH --error=/net/pr2/projects/plgrid/plgghopfieldmgr/Masters/errors/plot_%A_%a.err

source "$PROJECT/CONFIG.sh"

# This script plots bifurcation diagrams associated with configuration sets from $1 to $2
# and saves the graphic files as bifur_x_config-XXXXXXX.$3, ..., bifur_z_config-XXXXXXX.$3 
# where $3 is third argument passed by the user being a file's extension e.g. $3="pdf", $3="png", ...

PLOT_BIFUR_DATA_PATHS=($(seq -f "$DATA_DIR/time-evol/$CONTROL_PARAM_NAME/time-evol_config-%07g.csv" $1 $2))
FIGURES_PATH="$PROJECT/figures"
PLOT_BIFUR_X_FIGURE_PATH=$(printf "$FIGURES_PATH/bifurcation/$CONTROL_PARAM_NAME/bifur_x_config-%07g-%07g.$3" $1 $2)
PLOT_BIFUR_Y_FIGURE_PATH=$(printf "$FIGURES_PATH/bifurcation/$CONTROL_PARAM_NAME/bifur_y_config-%07g-%07g.$3" $1 $2)
PLOT_BIFUR_Z_FIGURE_PATH=$(printf "$FIGURES_PATH/bifurcation/$CONTROL_PARAM_NAME/bifur_z_config-%07g-%07g.$3" $1 $2)

FILE_TEMP_DATA_PATHS="$PROJECT/supp_files/bifur_data_paths.txt"

if [[ -f $FILE_TEMP_DATA_PATHS ]]; then
    rm $FILE_TEMP_DATA_PATHS
fi

touch $FILE_TEMP_DATA_PATHS

n_configs=$(( $2 - $1 ))
for i in $(seq 0 $n_configs); do
    PLOT_BIFUR_DATA_PATH="${PLOT_BIFUR_DATA_PATHS[$i]}"

    echo "$PLOT_BIFUR_DATA_PATH" >> $FILE_TEMP_DATA_PATHS
done

module load python/3.9.6
module add matplotlib
module load scipy-bundle/2023.11-gfbf-2023b

srun python3 "$SOURCE_CODE_DIR/plot_bifur.py" "$N_ITER_LAST" "$FILE_TEMP_DATA_PATHS" "$PLOT_BIFUR_X_FIGURE_PATH"\
             "$PLOT_BIFUR_Y_FIGURE_PATH" "$PLOT_BIFUR_Z_FIGURE_PATH" "$3" "$N_CONFIG_SETS" "$CONTROL_PARAM_NAME" "$CONTROL_PARAM_MIN" "$CONTROL_PARAM_STEP"