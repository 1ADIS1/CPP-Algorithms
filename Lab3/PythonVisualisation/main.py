import sys
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

csv_file = sys.argv[1] if len(sys.argv) > 1 else "benchmark_results.csv"
df = pd.read_csv(csv_file)

# ensure numeric
df = df.sort_values("size")
sizes = df["size"].to_numpy()

qs_avg = df["qs_avg_ns"].to_numpy().astype(np.float64)
is_avg = df["is_avg_ns"].to_numpy().astype(np.float64)

plt.figure(figsize=(9,6))

# Plot average times (log-log)
plt.plot(sizes, qs_avg / 1e6, marker='o', label='Quicksort (no insertion opt) [ms]')
plt.plot(sizes, is_avg / 1e6, marker='s', label='Insertion sort (reversed input) [ms]')

plt.xscale('log', base=2)
plt.yscale('log')
plt.xlabel('Array size (n, log2 scale)')
plt.ylabel('Average time (ms, log scale)')
plt.title('Benchmark: Quicksort (no insertion opt) vs Insertion sort on reversed arrays')
plt.grid(True, which='both', ls='--', lw=0.5)
plt.legend()

plt.tight_layout()
plt.savefig("benchmark_plot.png", dpi=200)
print("Saved benchmark_plot.png")
plt.show()
