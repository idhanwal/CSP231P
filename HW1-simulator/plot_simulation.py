import matplotlib.pyplot as plt
import numpy as np
import os

# Settings
processor_counts = [2, 4, 8, 16, 32, 64]
num_modules = list(range(1, 513))  # 1 to 512
distributions = ['uniform', 'normal']

# Colors for each processor count (to match the original plot)
colors = {
    2: 'green',
    4: 'blue',
    8: 'purple',
    16: 'red',
    32: 'orange',
    64: 'brown'
}

def load_data(dist, procs):
    filename = f"{dist}_{procs}.csv"
    if not os.path.exists(filename):
        print(f"Missing: {filename}")
        return None
    with open(filename) as f:
        values = [float(line.strip()) for line in f if line.strip()]
    return values if len(values) == 512 else None

def plot_distribution(dist):
    plt.figure(figsize=(10, 6))
    for p in processor_counts:
        data = load_data(dist, p)
        if data:
            plt.step(num_modules, data, where='post', label=f'{p} procs', color=colors[p], linewidth=2)

    plt.xlabel('Memory Modules')
    plt.ylabel('Avg Access Time $T_c(S)$')
    plt.xscale('linear')
    plt.yscale('log')
    plt.title(f'{dist.capitalize()} Distribution')
    plt.legend()
    plt.grid(True, which='both', linestyle='--', linewidth=0.5)
    plt.tight_layout()
    plt.savefig(f'{dist}_chart.png')
    print(f"Saved: {dist}_chart.png")

for dist in distributions:
    plot_distribution(dist)