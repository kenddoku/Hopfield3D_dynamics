#!/bin/bash
#SBATCH --job-name=set_jupyter
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=1
#SBATCH --mem=8G
#SBATCH --time=00:30:00
#SBATCH --partition=plgrid
#SBATCH --account=plghopkrypt-cpu
#SBATCH --output=/net/pr2/projects/plgrid/plgghopfieldmgr/Masters/logs/jupyter_node_info.out

source ~/.bashrc
conda activate jupyter
jupyter lab --no-browser --ip=0.0.0.0 --port=8888

