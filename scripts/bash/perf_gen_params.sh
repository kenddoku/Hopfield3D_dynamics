source "$PROJECT/CONFIG.sh"

module load gcc/11.3.0 gsl/2.7-gcc-11.3.0

srun -p plgrid -N 1 --ntasks-per-node=1 -n 1 -A plghopkrypt-cpu "$SOURCE_CODE_DIR/gen_params" \
"$PARAMS_DIR" "$N_CONFIG_SETS" "$CONTROL_PARAM_NAME" "$CONTROL_PARAM_MIN" "$CONTROL_PARAM_MAX" "$CONTROL_PARAM_STEP" \
"$NU" "$X0" "$Y0" "$Z0" \
"$W11" "$W12" "$W13" \
"$W21" "$W22" "$W23" \
"$W31" "$W32" "$W33" \
"$N_ITER"

