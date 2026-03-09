"""..."""

import csv
from collections import defaultdict
import matplotlib.pyplot as plt
import numpy as np

CSV_PATH = "cmake-build-debug/sorting_benchmark.csv"

rows = []
with open(CSV_PATH, "r", encoding="utf-8") as f:
    reader = csv.DictReader(f)
    for r in reader:
        rows.append({"algo": r["algo"], "N": int(r["N"]), "time_ms": float(r["time_ms"])})

algos = sorted({r["algo"] for r in rows})
Ns = sorted({r["N"] for r in rows})

by_N = defaultdict(dict)
for r in rows:
    by_N[r["N"]][r["algo"]] = r["time_ms"]

x = np.arange(len(Ns))
width = 0.8 / max(1, len(algos))

plt.figure()
for i, algo in enumerate(algos):
    vals = [by_N[N].get(algo, 0.0) for N in Ns]
    plt.bar(x + (i - (len(algos)-1)/2)*width, vals, width, label=algo)

plt.xticks(x, [str(N) for N in Ns])
plt.xlabel("N (rows)")
plt.ylabel("Time (ms) — sort only")
plt.title("Sorting time comparison (std vs radix)")
plt.grid(True, axis="y", linestyle="--", alpha=0.4)
plt.legend()
plt.tight_layout()
plt.savefig("sort_time_bars.png", dpi=180)
print("Saved sort_time_bars.png")

plt.figure()
for algo in algos:
    Ns_line = []
    t_line = []
    for N in Ns:
        if algo in by_N[N]:
            Ns_line.append(N)
            t_line.append(by_N[N][algo])
    plt.plot(Ns_line, t_line, marker="o", label=algo)

plt.xscale("log")
plt.yscale("log")
plt.xlabel("N (rows) [log]")
plt.ylabel("Time (ms) [log]")
plt.title("Sorting time (log-log)")
plt.grid(True, which="both", linestyle="--", alpha=0.4)
plt.legend()
plt.tight_layout()
plt.savefig("sort_time_loglog.png", dpi=180)
print("Saved sort_time_loglog.png")
