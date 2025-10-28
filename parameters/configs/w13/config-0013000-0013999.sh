# General Setup-------------------------------------------------
# Choose the number of unique params configs to check (!!!FOR NOW 1000 is a max limit - WORK ON IT TO ENABLE MORE):
N_CONFIG_SETS=1000

# Choose actions (does not matter as for now since there is no launcher (RUN.sh) script managing tasks):
GEN_DATA_DIRS="TRUE"
GEN_PARAMS="TRUE"
PERF_TEVOL="TRUE"
PLOT_TEVOL="TRUE"
PLOT_BIFUR="FALSE"

# GEN_PARAMS Setup----------------------------------------------
# Set nu:
NU=0.45

# Set initial state:
X0=0.2
Y0=-0.5
Z0=0.8

# Set initial weights:
W11=2.00; W12=-1.20; W13=0.00
W21=1.90; W22=1.71; W23=1.15
W31=-4.75; W32=0.00; W33=1.10

# Choose the control parameter {nu, x0, y0, z0, w11, w12, ..., w33}:
CONTROL_PARAM_NAME="w13"
# Choose the range and the step for control parameter (currently minimal possible value is 0.001):
CONTROL_PARAM_MIN=-5.000
CONTROL_PARAM_MAX=5.000 # currently this value does not matter, the max value will be set automatically based on N_CONFIG_SETS and CONTROL_PARAM_STEP
CONTROL_PARAM_STEP=0.010

# PERF_TEVOL Setup----------------------------------------------
# Choose the number of time-evol iterations 
N_ITER=500000
# Choose the maximum time of calculations (single proccess) hh:mm:ss
PERF_TEVOL_TIME="00:30:00"

#PLOT_TEVOL Setup-----------------------------------------------
# Choose the maximum time for plotting (single proccess) hh:mm:ss
PLOT_TEVOL_TIME="00:30:00"

# PLOT_BIFUR Setup----------------------------------------------
# Choose the number of iterations to plot for a single value of control parameter
N_ITER_LAST=300
PLOT_BIFUR_TIME="00:30:00"

# Paths---------------------------------------------------------
DATA_DIR="$PROJECT/data"
SCRIPTS_DIR="$PROJECT/scripts"
SOURCE_CODE_DIR="$PROJECT/code/src"
PARAMS_DIR="$PROJECT/parameters"
LOGS_DIR="$PROJECT/logs"
ERRORS_DIR="$PROJECT/errors"
