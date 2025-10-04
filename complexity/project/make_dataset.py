import random
import time
import pandas as pd
import networkx as nx
from ortools.linear_solver import pywraplp

random.seed(42)

def lp_set_cover(universe, subsets, k):
    m = len(subsets)
    solver = pywraplp.Solver.CreateSolver('GLOP')
    x = [solver.NumVar(0, 1, f'x_{j}') for j in range(m)]

    for e in universe:
        solver.Add(sum(x[j] for j in range(m) if e in subsets[j]) >= 1)

    solver.Minimize(sum(x[j] for j in range(m)))
    solver.Solve()

    selected = [j for j in range(m) if x[j].solution_value() >= 1 / k]
    return selected


def choose_best_subset(remaining, uncovered):
    best_index, best_gain = None, -1
    for i, subset in enumerate(remaining):
        gain = len(subset & uncovered)
        if gain > best_gain:
            best_gain = gain
            best_index = i
    return best_index


def greedy_set_cover(universe, subsets):
    U = set(universe)
    remaining = [set(s) for s in subsets]
    selected = []
    covered = set()
    while covered != U:
        best = choose_best_subset(remaining, U - covered)
        selected.append(best)
        covered |= remaining[best]
        for s in remaining:
            s -= remaining[best]
    return selected


def gen_random_set_cover(n, m, p):
    U = list(range(n))
    subsets = []
    for _ in range(m):
        S = {e for e in U if random.random() < p}
        if not S:
            S.add(random.choice(U))
        subsets.append(S)
    # print(len(S))
    missing = set(U) - set().union(*subsets)
    for e in missing:
        random.choice(subsets).add(e)
    return U, subsets


def graph_to_set_cover(G):
    edges = list(G.edges())
    U = list(range(len(edges)))
    edge_idx = {e: i for i, e in enumerate(edges)}
    n = G.number_of_nodes()
    subsets = [set() for _ in range(n)]
    for i, (u, v) in enumerate(edges):
        subsets[u].add(i)
        subsets[v].add(i)
    return U, subsets


def run_suite():
    results = []
    print("stage1")
    for n in range(20, 450, 10):
        print(n)
        m = 2*n
        p = 0.05
        for _ in range(5):
            U, subsets = gen_random_set_cover(n, m, p)
            k_elem = max(sum(e in S for S in subsets) for e in U)

            t0 = time.perf_counter()
            greedy_sel = greedy_set_cover(U, subsets)
            tg = time.perf_counter() - t0

            t0 = time.perf_counter()
            lp_sel = lp_set_cover(U, subsets, k=k_elem)
            tlp = time.perf_counter() - t0

            results.append(dict(
                dataset="Random(sparse)",
                n=len(U),
                k=k_elem,
                algo="Greedy",
                cover_size=len(greedy_sel),
                time=tg,
                opt=None
            ))
            results.append(dict(
                dataset="Random(sparse)",
                n=len(U),
                k=k_elem,
                algo=f"LP, k=rng",
                cover_size=len(lp_sel),
                time=tlp,
                opt=None
            ))

    print("stage2")
    for n in range(20, 450, 10):
        print(n)
        m = n
        p = 0.05
        for _ in range(5):
            U, subsets = gen_random_set_cover(n, m, p)

            t0 = time.perf_counter()
            greedy_sel = greedy_set_cover(U, subsets)
            tg = time.perf_counter() - t0

            for i in subsets:
                i.add(U[0])
            k_elem = len(subsets)

            t0 = time.perf_counter()
            lp_sel = lp_set_cover(U, subsets, k=k_elem)
            tlp = time.perf_counter() - t0

            results.append(dict(
                dataset="Random(sparse), big k",
                n=len(U),
                k=k_elem,
                algo="Greedy",
                cover_size=len(greedy_sel),
                time=tg,
                opt=None
            ))
            results.append(dict(
                dataset="Random(sparse), big k",
                n=len(U),
                k=k_elem,
                algo=f"LP, k = n",
                cover_size=len(lp_sel),
                time=tlp,
                opt=None
            ))

    print("stage3")
    for n in range(20, 450, 10):
        print(n)
        m = 2 * n
        p1 = 0.05
        p2 = 0.3
        for _ in range(5):
            U, subsets = gen_random_set_cover(n, m, p1)
            k_elem = max(sum(e in S for S in subsets) for e in U)

            t0 = time.perf_counter()
            lp_sel1 = lp_set_cover(U, subsets, k=k_elem)
            tlp1 = time.perf_counter() - t0
            t0 = time.perf_counter()
            greedy_sel1 = greedy_set_cover(U, subsets)
            tg1 = time.perf_counter() - t0

            U, subsets = gen_random_set_cover(n, m, p2)
            k_elem = max(sum(e in S for S in subsets) for e in U)

            t0 = time.perf_counter()
            lp_sel2 = lp_set_cover(U, subsets, k=k_elem)
            tlp2 = time.perf_counter() - t0
            t0 = time.perf_counter()
            greedy_sel2 = greedy_set_cover(U, subsets)
            tg2 = time.perf_counter() - t0

            results.append(dict(
                dataset="Random, sparse & dense",
                n=len(U),
                k=k_elem,
                algo=f"LP(sparse), k=rng",
                cover_size=len(lp_sel1),
                time=tlp1,
                opt=None
            ))
            results.append(dict(
                dataset="Random, sparse & dense",
                n=len(U),
                k=k_elem,
                algo=f"LP(dense), k=rng",
                cover_size=len(lp_sel2),
                time=tlp2,
                opt=None
            ))
            results.append(dict(
                dataset="Random, sparse & dense",
                n=len(U),
                k=None,
                algo=f"Greedy(sparse)",
                cover_size=len(greedy_sel1),
                time=tg1,
                opt=None
            ))
            results.append(dict(
                dataset="Random, sparse & dense",
                n=len(U),
                k=None,
                algo=f"Greedy(dense)",
                cover_size=len(greedy_sel2),
                time=tg2,
                opt=None
            ))

    print("stage4")
    for N in range(20, 450, 10):
        print(N)
        p = 4 / N
        for _ in range(5):
            G = nx.bipartite.random_graph(N//2, N - N//2, p)
            U, subsets = graph_to_set_cover(G)
            opt = nx.algorithms.matching.maximal_matching(G)
            opt_size = len(opt)

            t0 = time.perf_counter()
            greedy_sel = greedy_set_cover(U, subsets)
            tg = time.perf_counter() - t0

            t0 = time.perf_counter()
            lp_sel = lp_set_cover(U, subsets, k=2)
            tlp = time.perf_counter() - t0

            for name, sel, t in [("Greedy", greedy_sel, tg),
                                 ("LP, k=2", lp_sel, tlp)]:
                results.append(dict(
                    dataset="Bipartite, deg ~ 4",
                    n=N,
                    k=2,
                    algo=name,
                    cover_size=len(sel),
                    time=t,
                    opt=opt_size
                ))

    print("stage5")
    for N in range(20, 450, 10):
        print(N)
        p1 = 2 / N
        p2 = 10/N
        for _ in range(5):
            G = nx.bipartite.random_graph(N // 2, N - N // 2, p1)
            U, subsets = graph_to_set_cover(G)
            opt = nx.algorithms.matching.maximal_matching(G)
            opt_sparse = len(opt)

            t0 = time.perf_counter()
            lp_sel1 = lp_set_cover(U, subsets, k=2)
            tlp1 = time.perf_counter() - t0
            t0 = time.perf_counter()
            greedy_sel1 = greedy_set_cover(U, subsets)
            tg1 = time.perf_counter() - t0


            G = nx.bipartite.random_graph(N // 2, N - N // 2, p2)
            U, subsets = graph_to_set_cover(G)
            opt = nx.algorithms.matching.maximal_matching(G)
            opt_dense = len(opt)

            t0 = time.perf_counter()
            lp_sel2 = lp_set_cover(U, subsets, k=2)
            tlp2 = time.perf_counter() - t0
            t0 = time.perf_counter()
            greedy_sel2 = greedy_set_cover(U, subsets)
            tg2 = time.perf_counter() - t0

            results.append(dict(
                dataset="Bipartite, sparse & dense",
                n=N,
                k=2,
                algo=f"LP(sparse), k=2",
                cover_size=len(lp_sel1),
                time=tlp1,
                opt=opt_sparse
            ))
            results.append(dict(
                dataset="Bipartite, sparse & dense",
                n=N,
                k=2,
                algo=f"LP(dense), k=2",
                cover_size=len(lp_sel2),
                time=tlp2,
                opt=opt_dense
            ))
            results.append(dict(
                dataset="Bipartite, sparse & dense",
                n=N,
                k=None,
                algo=f"Greedy(sparse)",
                cover_size=len(greedy_sel1),
                time=tg1,
                opt=opt_sparse
            ))
            results.append(dict(
                dataset="Bipartite, sparse & dense",
                n=N,
                k=None,
                algo=f"Greedy(dense)",
                cover_size=len(greedy_sel2),
                time=tg2,
                opt=opt_dense
            ))
    return pd.DataFrame(results)


df = run_suite()
df.to_csv("please.csv", index=False)
