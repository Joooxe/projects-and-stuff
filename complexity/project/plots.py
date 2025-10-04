import pandas as pd
import matplotlib
import matplotlib.pyplot as plt

matplotlib.use("Qt5Agg")
df = pd.read_csv("please.csv")


def plot_cover_size(df, dataset_name):
    sub = df[df.dataset == dataset_name]
    plt.figure()
    for algo in sub.algo.unique():
        mean = sub[sub.algo == algo].groupby("n")["cover_size"].mean()
        plt.plot(mean.index, mean.values, marker='o', label=algo)
    plt.title(f"Размер покрытия – {dataset_name}")
    plt.xlabel("|M|  (n)")
    plt.ylabel("Число выбранных подмножеств")
    plt.grid(True)
    plt.legend()
    plt.show()


def plot_quality_ratio(df, dataset_name):
    sub = df[(df.dataset == dataset_name) & (df.opt.notna())]
    plt.figure()
    for algo in sub.algo.unique():
        rows = sub[sub.algo == algo]
        if rows.empty:
            continue
        rows = rows.assign(ratio=rows.cover_size / rows.opt)
        mean = rows.groupby("n")["ratio"].mean()
        plt.plot(mean.index, mean.values, marker='o', label=algo)
    plt.title(f"Коэффициент приближения – {dataset_name}")
    plt.xlabel("|M|  (n)")
    plt.ylabel("ALG / OPT")
    plt.grid(True)
    plt.legend()
    plt.show()


def plot_time(df, dataset_name):
    sub = df[df.dataset == dataset_name]
    plt.figure()
    for algo in sorted(sub.algo.unique()):
        mean = sub[sub.algo == algo].groupby("n")["time"].mean()
        plt.semilogy(mean.index, mean, marker='o', label=algo)
    plt.title(f"Время работы – {dataset_name}")
    plt.xlabel("|M|  (n)")
    plt.ylabel("Время, сек")
    plt.grid(True, which="both")
    plt.legend()
    plt.show()


plot_cover_size(df, "Random(sparse)")
plot_cover_size(df, "Random(sparse), big k")
plot_cover_size(df, "Random, sparse & dense")
plot_cover_size(df, "Bipartite, deg ~ 4")
plot_cover_size(df, "Bipartite, sparse & dense")

plot_quality_ratio(df, "Bipartite, deg ~ 4")
plot_quality_ratio(df, "Bipartite, sparse & dense")

plot_time(df, "Random(sparse)")
plot_time(df, "Random(sparse), big k")
plot_time(df, "Random, sparse & dense")
plot_time(df, "Bipartite, deg ~ 4")
plot_time(df, "Bipartite, sparse & dense")
