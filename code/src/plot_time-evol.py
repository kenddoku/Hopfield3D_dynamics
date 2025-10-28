import sys
import os
import pandas as pd
import matplotlib.pyplot as plt

N_ITER = int(sys.argv[1])
data_path = sys.argv[2]
xyz_figure_path = sys.argv[3]
graphic_file_extension = sys.argv[4]
n_iter_init = int(sys.argv[5])  # Initial iteration on x-axis that will get displayed
n_iter_fin = int(sys.argv[6])   # Final iteration on x-axis that will get displayed

if not os.path.isfile(data_path):
    print(f"[SKIP] Data was not found in {data_path}")
    sys.exit(0)

data = pd.read_csv(data_path)

n_col = data.columns[0]
x_col = data.columns[1]
y_col = data.columns[2]
z_col = data.columns[3]

n = data[n_col]
x = data[x_col]
y = data[y_col]
z = data[z_col]

num_ranges = 4
range_step = (n_iter_fin - n_iter_init + 1) // num_ranges  # e.g. 99 - 0 + 1 = 100
ranges = [(n_iter_init + i * range_step, n_iter_init + (i+1) * range_step) for i in range(num_ranges)]

fig, axs = plt.subplots(1, 3, figsize=(18, 5), dpi=150)  # 1 row, 3 columns

variables = [(x, x_col, 'red'), (y, y_col, 'green'), (z, z_col, 'blue')]

for ax, (variable, var_name, color) in zip(axs, variables):
    y_min, y_max = variable.min(), variable.max()

    # Divide into 4 ranges and plot
    for i, (start, end) in enumerate(ranges):
        ax.scatter(n[start:end], variable[start:end], s=5, color=color, label=f"[{start}–{end}]")

    ax.set_title(f"{var_name} evolution")
    ax.set_xlabel(n_col)
    ax.set_ylabel(var_name)
    ax.set_ylim(y_min, y_max)
    ax.legend(fontsize=8, loc='upper right')

# Optional: global title
fig.suptitle(f"Time evolution of x, y, z (N_ITER={N_ITER})", fontsize=14)

plt.tight_layout(rect=[0, 0, 1, 0.95])

# Save combined figure
if graphic_file_extension == "pdf":
    fig.savefig(xyz_figure_path, format=graphic_file_extension)
elif graphic_file_extension == "png":
    fig.savefig(xyz_figure_path, dpi=300)
else:
    print("The graphic file extension is incorrect\n")

plt.close()
