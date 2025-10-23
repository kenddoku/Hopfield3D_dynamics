import os
import sys
import matplotlib.pyplot as plt
import numpy as np
from collections import deque

# # Function to read last N of a file 'fname'
# def LastNlines(fname, N):
#     # taking buffer size of 8192 bytes
#     bufsize = 8192
    
#     # calculating size of
#     # file in bytes
#     fsize = os.stat(fname).st_size
    
#     iter = 0
    
#     # opening file using with() method
#     # so that file get closed
#     # after completing work
#     with open(fname) as f:
#         if bufsize > fsize:
                
#             # adjusting buffer size
#             # according to size
#             # of file
#             bufsize = fsize-1
            
#             # list to store 
#             # last N lines
#             fetched_lines = []
            
#             # while loop to
#             # fetch last N lines
#             while True:
#                 iter += 1
                
#                 # moving cursor to
#                 # the last Nth line
#                 # of file
#                 f.seek(fsize-bufsize * iter)
                
#                 # storing each line
#                 # in list upto
#                 # end of file
#                 fetched_lines.extend(f.readlines())
                
#                 # halting the program
#                 # when size of list
#                 # is equal or greater to
#                 # the number of lines requested or
#                 # when we reach end of file
#                 if len(fetched_lines) >= N or f.tell() == 0:
#                         # print(''.join(fetched_lines[-N:]))
#                         return fetched_lines[-N:]
#                         break

def LastNlines(fname, N):
    with open(fname, 'r') as f:
        return list(deque(f, N))

                        
# Driver Code: 
if __name__ == '__main__':
    
    n_iter_last = int(sys.argv[1])
    file_temp_data_paths = sys.argv[2]
    bifur_x_figure_path = sys.argv[3]
    bifur_y_figure_path = sys.argv[4]
    bifur_z_figure_path = sys.argv[5]
    graphic_file_extension = sys.argv[6]

    n_config_sets = int(sys.argv[7])
    control_param_name = sys.argv[8]

    # !!!!!!!! W PLOTOWANIU NIE MA TO AŻ TAK DUŻEGO ZNACZENIA ALE LEPIEJ JAKBY CZYTAŁ
    # TUTAJ int I POTEM KONWERTOWAŁ NA double/float
    control_param_min = float(sys.argv[9]) 
    control_param_step = float(sys.argv[10])
    
    fig_x, ax_x = plt.subplots()
    fig_y, ax_y = plt.subplots()
    fig_z, ax_z = plt.subplots()

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
                last_n_data_lines = LastNlines(f_data_path, n_iter_last)

                control_param_val = control_param_min + i*control_param_step
                control_param_vals = np.ones(n_iter_last) * control_param_val
                # x, y and z values associated with a single parameter configuration (a single value of control parameter)
                x_vals = []
                y_vals = []
                z_vals = []

                for data_line in last_n_data_lines:
                    data_in_row = data_line.strip().split(',')
                    x, y, z = map(float, data_in_row[1:]) # skips the first values (int: n_iter)

                    x_vals.append(x)
                    y_vals.append(y)
                    z_vals.append(z)

                # TUTAJ PLOTUJ DANE DLA POJEDYNCZEJ WARTOSCI PARAMETRU
                all_control_param_vals.extend(control_param_vals)
                all_x.extend(x_vals)
                all_y.extend(y_vals)
                all_z.extend(z_vals)

                # ax_x.scatter(control_param_vals, x_vals, c='r')
                # ax_y.scatter(control_param_vals, y_vals, c='g')
                # ax_z.scatter(control_param_vals, z_vals, c='b')

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

    if graphic_file_extension == "pdf":
        fig_x.savefig(bifur_x_figure_path, format=graphic_file_extension)
        fig_y.savefig(bifur_y_figure_path, format=graphic_file_extension)
        fig_z.savefig(bifur_z_figure_path, format=graphic_file_extension)
    elif graphic_file_extension == "png":
        fig_x.savefig(bifur_x_figure_path, dpi=300)
        fig_y.savefig(bifur_y_figure_path, dpi=300)
        fig_z.savefig(bifur_z_figure_path, dpi=300)
    else:
        print("The graphic file extension is incorrect\n")

