import sys
import numpy as np
import matplotlib.pyplot as plt

# -----------------------------
# Command-line arguments
# -----------------------------
# Usage: python plot_time-evol.py N_ITER data_csv x_fig y_fig z_fig ext step_min step_max
_, N_ITER, data_csv, x_fig, y_fig, z_fig, ext, step_min, step_max = sys.argv

step_min = int(step_min)
step_max = int(step_max)

# -----------------------------
# Load data
# -----------------------------
# Assume CSV has 4 columns: index, x, y, z
data = np.loadtxt(data_csv, delimiter=',')
indices = data[:, 0].astype(int)
x_values = data[:, 1]
y_values = data[:, 2]
z_values = data[:, 3]

# -----------------------------
# Select steps
# -----------------------------
# Python uses 0-based indexing
mask = (indices >= step_min) & (indices <= step_max)

x = x_values[mask]
y = y_values[mask]
z = z_values[mask]
steps = indices[mask]

# -----------------------------
# Compute current vs previous
# -----------------------------
def current_vs_previous(arr):
    # y = n+1, x = n
    return arr[:-1], arr[1:]

x_prev, x_curr = current_vs_previous(x)
y_prev, y_curr = current_vs_previous(y)
z_prev, z_curr = current_vs_previous(z)

# -----------------------------
# Plot function
# -----------------------------
def plot_cvsp(x_data, y_data, xlabel, ylabel, title, fig_path):
    plt.figure(figsize=(6,6))
    plt.scatter(x_data, y_data, s=20, color='blue', alpha=0.7)
    plt.plot([min(x_data), max(x_data)], [min(x_data), max(x_data)], 'r--', label='y=x')
    plt.xlabel(xlabel)
    plt.ylabel(ylabel)
    plt.title(title)
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    plt.savefig(fig_path)
    plt.close()

# -----------------------------
# Plot X, Y, Z
# -----------------------------
plot_cvsp(x_prev, x_curr, 'X[n]', 'X[n+1]', '', x_fig)
plot_cvsp(y_prev, y_curr, 'Y[n]', 'Y[n+1]', '', y_fig)
plot_cvsp(z_prev, z_curr, 'Z[n]', 'Z[n+1]', '', z_fig)
