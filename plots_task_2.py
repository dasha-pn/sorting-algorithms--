"""..."""

import csv
import os
from collections import defaultdict
import matplotlib.pyplot as plt
import numpy as np

CSV_PATH = "cmake-build-debug/bench_results.csv"

def load_results(path):
    """..."""

    rows = []
    with open(path, "r", encoding="utf-8") as f:
        reader = csv.DictReader(f)
        for r in reader:
            rows.append({
                "variant": r["variant"],
                "N": int(r["N"]),
                "ops": int(r["ops"]),
                "seconds": int(r.get("seconds", 10)),
                "rss_bytes": int(r.get("rss_bytes", 0)),
            })
    return rows

def plot_ops_grouped(rows, out_path="ops_bar.png"):
    """..."""

    Ns = sorted({r["N"] for r in rows})
    variants = sorted({r["variant"] for r in rows})
    by_N_variant = defaultdict(dict)
    for r in rows:
        by_N_variant[r["N"]][r["variant"]] = r

    ops_per_s = {
        N: [by_N_variant[N][v]["ops"] / max(1, by_N_variant[N][v]["seconds"]) for v in variants]
        for N in Ns
    }

    x = np.arange(len(Ns))
    width = 0.8 / max(1, len(variants))

    plt.figure()
    for i, v in enumerate(variants):
        vals = [ops_per_s[N][i] for N in Ns]
        plt.bar(x + (i - (len(variants)-1)/2)*width, vals, width, label=f"Variant {v}")

    plt.xticks(x, [str(N) for N in Ns])
    plt.xlabel("N (rows)")
    plt.ylabel("Operations per second")
    plt.title("Throughput (ops/s) — grouped bars")
    plt.grid(True, axis="y", linestyle="--", alpha=0.4)
    plt.legend()
    plt.tight_layout()
    plt.savefig(out_path, dpi=180)
    print(f"Saved {out_path}")

def plot_mem_per_N(rows, out_dir=".", Ns_specific=(100, 1000, 10000, 100000)):
    """..."""

    variants = sorted({r["variant"] for r in rows})
    by_N_variant = defaultdict(dict)
    for r in rows:
        by_N_variant[r["N"]][r["variant"]] = r

    for N in Ns_specific:
        if N not in by_N_variant:
            print(f"Skip N={N}: no data in CSV")
            continue
        vals = []
        labels = []
        for v in variants:
            if v in by_N_variant[N]:
                mib = by_N_variant[N][v]["rss_bytes"] / (1024 * 1024)
                vals.append(mib)
                labels.append(f"Variant {v}")

        plt.figure()
        xpos = np.arange(len(vals))
        plt.bar(xpos, vals, width=0.6)
        plt.xticks(xpos, labels)
        plt.xlabel("Variant")
        plt.ylabel("RSS delta (MiB)")
        plt.title(f"Memory usage by variant — N={N}")
        plt.grid(True, axis="y", linestyle="--", alpha=0.4)
        plt.tight_layout()
        outp = os.path.join(out_dir, f"mem_N_{N}.png")
        plt.savefig(outp, dpi=180)
        print(f"Saved {outp}")

def main():
    """..."""

    if not os.path.exists(CSV_PATH):
        raise FileNotFoundError(f"{CSV_PATH} not found")
    rows = load_results(CSV_PATH)
    plot_ops_grouped(rows, "ops_bar.png")
    plot_mem_per_N(rows, ".", (100, 1000, 10000, 100000))

if __name__ == "__main__":
    main()
