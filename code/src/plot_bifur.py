import os
import sys
import matplotlib.pyplot as plt
import numpy as np
from collections import deque

# Returns N last lines of a file (emulating 'tail' bash command)
def tail(fname, N):
    with open(fname, 'r') as f:
        return list(deque(f, N))

if __name__ == '__main__':
    
    n_iter_last = int(sys.argv[1])
    file_temp_data_paths = sys.argv[2]
    bifur_xyz_figure_path = sys.argv[3]
    graphic_file_extension = sys.argv[4]

    n_config_sets = int(sys.argv[5])
    control_param_name = sys.argv[6]

    # !!!!!!!! W PLOTOWANIU NIE MA TO AŻ TAK DUŻEGO ZNACZENIA ALE LEPIEJ JAKBY CZYTAŁ
    # TUTAJ int I POTEM KONWERTOWAŁ NA double/float
    control_param_min = float(sys.argv[7]) 
    control_param_step = float(sys.argv[8])

    n_iter = int(sys.argv[9])
    
    fig, (ax_x, ax_y, ax_z) = plt.subplots(3, 1, figsize=(8, 10), sharex=True)

    all_control_param_vals = []
    all_x = []
    all_y = []
    all_z = []

    i=0

    with open(file_temp_data_paths, 'r') as f_data_paths:
            for f_data_path in f_data_paths:
                if not f_data_path:
                    continue

                f_data_path = f_data_path.strip()
                last_n_data_lines = tail(f_data_path, n_iter_last)

                control_param_val = control_param_min + i*control_param_step
                control_param_vals = np.ones(n_iter_last) * control_param_val
                # x, y and z values associated with a single parameter configuration (a single value of control parameter)
                x_vals = []
                y_vals = []
                z_vals = []

                for data_line in last_n_data_lines:
                    data_in_row = data_line.strip().split(',')
                    x, y, z = map(float, data_in_row[1:]) # skips the first value (int: n_iter)

                    x_vals.append(x)
                    y_vals.append(y)
                    z_vals.append(z)

                # TUTAJ PLOTUJ DANE DLA POJEDYNCZEJ WARTOSCI PARAMETRU
                all_control_param_vals.extend(control_param_vals)
                all_x.extend(x_vals)
                all_y.extend(y_vals)
                all_z.extend(z_vals)

                i+=1

    # Formatting and saving figures -------------------------------------------
    ax_x.scatter(all_control_param_vals, all_x, c='r', s=0.01)
    ax_y.scatter(all_control_param_vals, all_y, c='g', s=0.01)
    ax_z.scatter(all_control_param_vals, all_z, c='b', s=0.01)
    
    ax_x.set_xlabel(control_param_name)
    ax_y.set_xlabel(control_param_name)
    ax_z.set_xlabel(control_param_name)

    ax_x.set_ylabel('x')
    ax_y.set_ylabel('y')
    ax_z.set_ylabel('z')

    fig.tight_layout()

    fig.suptitle(f"N_ITER={n_iter}, N_ITER_LAST={n_iter_last}")

    if graphic_file_extension == "pdf":
        fig.savefig(bifur_xyz_figure_path, format=graphic_file_extension)
    elif graphic_file_extension == "png":
        fig.savefig(bifur_xyz_figure_path, dpi=400)
    else:
        print("The graphic file extension is incorrect\n")


