#!/bin/bash -l
#SBATCH --job-name=tevol
#SBATCH --partition plgrid
#SBATCH --account=plghopkrypt-cpu
#SBATCH --error=/net/pr2/projects/plgrid/plgghopfieldmgr/Masters/errors/error_%A_%a.err
#SBATCH --output=/net/pr2/projects/plgrid/plgghopfieldmgr/Masters/logs/log_%A_%a.out

source $PROJECT/CONFIG.sh

PERF_TEVOL_PARAM_PATHS=($(seq -f "$PARAMS_DIR/wparams/$CONTROL_PARAM_NAME/wparams_config-%07g.txt" $1 $2))
PERF_TEVOL_OUTPUT_PATHS=($(seq -f "$DATA_DIR/time-evol/$CONTROL_PARAM_NAME/time-evol_config-%07g.csv" $1 $2))

PERF_TEVOL_INDEX=$(( SLURM_ARRAY_TASK_ID ))

PERF_TEVOL_PARAM_PATH="${PERF_TEVOL_PARAM_PATHS[$PERF_TEVOL_INDEX]}"
PERF_TEVOL_OUTPUT_PATH="${PERF_TEVOL_OUTPUT_PATHS[$PERF_TEVOL_INDEX]}"

echo "SLURM_ARRAY_TASK_ID = $SLURM_ARRAY_TASK_ID"
echo "Using param path: $PERF_TEVOL_PARAM_PATH"
echo "Using output path: $PERF_TEVOL_OUTPUT_PATH"

module load gcc/11.3.0 gsl/2.7-gcc-11.3.0

srun "$SOURCE_CODE_DIR/perf_time-evol" "$PERF_TEVOL_PARAM_PATH" "$PERF_TEVOL_OUTPUT_PATH"
