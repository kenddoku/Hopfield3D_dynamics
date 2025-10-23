#!/bin/bash

FILE_CONFIG_ID_LIST="$PROJECT/parameters/configs/config_id_list.txt"

while read -r config_id_low config_id_high; do
    if (( $1 >= config_id_low && $1 <= config_id_high )); then
        CONFIG_FILE=$(printf "%s/parameters/configs/%s/config-%07g-%07g.sh" \
            "$PROJECT" "$2" "$config_id_low" "$config_id_high")
        number_of_steps=$(( $1 - config_id_low ))
        break
    fi
done < "$FILE_CONFIG_ID_LIST"

if [[ -f "$CONFIG_FILE" ]]; then
    source "$CONFIG_FILE"
else
    echo "Error: Config file '$CONFIG_FILE' not found"
    exit 1
fi

CONTROL_PARAM_VAL=$(echo "$CONTROL_PARAM_MIN + $number_of_steps * $CONTROL_PARAM_STEP" | bc -l)
CONTROL_PARAM_MAX=$(echo "$CONTROL_PARAM_MIN + $N_CONFIG_SETS * $CONTROL_PARAM_STEP" | bc -l)
# CONFIGURATION DISPLAY--------------------------------------
echo "-------------------------------------------------------------------------"
echo ""
echo "CONFIG_ID:    $1"
echo "PART OF:      $CONFIG_FILE"
echo ""
echo "NU=   $NU"
echo ""
echo "X0=   $X0"
echo "Y0=   $Y0"
echo "Z0=   $Z0"
echo ""
echo "W11=$W11   W12=$W12   W13=$W13"
echo "W21=$W21   W22=$W22   W23=$W23"
echo "W31=$W31   W32=$W32   W33=$W33"
echo ""
echo "CONTROL_PARAM_NAME =  $CONTROL_PARAM_NAME"
echo "CONTROL_PARAM_VAL  =  $CONTROL_PARAM_VAL"
echo ""  
echo "CONTROL_PARAM_MIN  =  $CONTROL_PARAM_MIN"
echo "CONTROL_PARAM_MAX  =  $CONTROL_PARAM_MAX"
echo "CONTROL_PARAM_STEP =  $CONTROL_PARAM_STEP"
echo ""