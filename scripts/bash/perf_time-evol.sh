#!/bin/bash

module load gcc/11.3.0 gsl/2.7-gcc-11.3.0

config_id_min=$1
config_id_max=$2

for ((config_id=$config_id_min; config_id<=$config_id_max; config_id++)); do

FILE_CONFIG_ID_LIST="$PROJECT/parameters/configs/config_id_list.txt"

while read -r config_id_low config_id_high; do
    if (( $config_id >= config_id_low && $config_id <= config_id_high )); then
        config_id_low_padded=$(printf "%07g" $config_id_low)
        config_id_high_padded=$(printf "%07g" $config_id_high)
        CONFIG_FILE=$(find $PROJECT/parameters/configs/ -name "config-$config_id_low_padded-$config_id_high_padded.sh")
        break
    fi
done < "$FILE_CONFIG_ID_LIST"

if [[ -f "$CONFIG_FILE" ]]; then
    source "$CONFIG_FILE"
else
    echo "Error: Config file '$CONFIG_FILE' not found"
    exit 1
fi

slurm_script_path=$(printf "$PROJECT/supp_files/time-evol_config-%07g.slurm" $config_id)

exec 3>$slurm_script_path
    
cat >&3 <<'EOF'
#!/bin/bash -l
#SBATCH --job-name=tevol
#SBATCH --partition plgrid
#SBATCH --account=plghopkrypt-cpu
#SBATCH --time=10:00:00
#SBATCH --output=/net/pr2/projects/plgrid/plgghopfieldmgr/Masters/logs/time-evol_output.out
#SBATCH --error=/net/pr2/projects/plgrid/plgghopfieldmgr/Masters/errors/time-evol_error.err

source $2

PERF_TEVOL_PARAM_PATH=$(printf "$PARAMS_DIR/wparams/$CONTROL_PARAM_NAME/wparams_config-%07g.txt" $1)
PERF_TEVOL_OUTPUT_PATH=$(printf "$DATA_DIR/time-evol/$CONTROL_PARAM_NAME/time-evol_config-%07g.csv" $1)

echo "$2 $PERF_TEVOL_PARAM_PATH $PERF_TEVOL_OUTPUT_PATH"

srun "$SOURCE_CODE_DIR/time-evol" "$PERF_TEVOL_PARAM_PATH" "$PERF_TEVOL_OUTPUT_PATH"
EOF

exec 3>&-

chmod +x "$slurm_script_path"

sbatch "$slurm_script_path" "$config_id" "$CONFIG_FILE"

done
