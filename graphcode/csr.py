from collections import defaultdict
import sys
import os

DEBUG = 0

def log(x):
    if DEBUG:
        print(x)
name = sys.argv[1]

try:
    isWeighted = int(sys.argv[2])
except:
    isWeighted = 0

filename = f"raw_graphs/udwt_graphs/{name}.txt"

os.system(f"cd csr_graphs; rm -r {name}; mkdir {name}")

with open(filename) as f:
    l = f.readlines()

for i in range(len(l)):
    try:
        l[i] = list(map(int, l[i].strip().split()))
    except:
        log(f"[WARN] Unable to parse line {i}")

g = defaultdict(dict)
seen = set()

for i in range(0, len(l)):
    try:
        if isWeighted:
            [u, v, w] = l[i]
        else:
            [u, v] = l[i]
            w = 1

        if DEBUG:
            log(f"{u} {v} {w}")

        if u == v:
            log(f"[WARN] found self loop {u} -> {v}")
            continue

        if v in g[u]:
            log(f"[WARN] found duplicate edge {u} -> {v}")
            g[u][v] = min(g[u][v], w)
        else:
            g[u][v] = w

        seen.add(u)
        seen.add(v)
    except:
        log(f"[WARN] Skipped line {i} = {l[i]}")

V = sorted(list(seen))
print(len(V))
maxEdge = V[-1]
make_cont = False

try:
    assert V == [i for i in range(maxEdge + 1)]
except:
    print("Not continuous")
    make_cont = True
    
E = []
I = []
W = []
pos = 0
for v in V:
    I.append(pos)
    for nn in sorted(g[v].keys()):
        nw = g[v][nn]
        E.append(nn)
        W.append(nw)
        pos += 1
I.append(pos)

if DEBUG:
    log(V)
    log(I)
    log(E)
    log(W)

if make_cont:
    print("Curr max = ", V[-1])
    print("Num nodes = ", len(V))

    id_map = {V[i] : i for i in range(len(V))}

    new_V = [i for i in range(len(V))]

    new_E = [id_map[E[i]] for i in range(len(E))]

    print(f"Prev edges = {len(E)}, New edges = {len(new_E)}")

    E = new_E
    V = new_V
    
with open(f"csr_graphs/{name}/V", "w") as f:
    f.write(" ".join(list(map(str, V))))

with open(f"csr_graphs/{name}/I", "w") as f:
    f.write(" ".join(list(map(str, I))))

with open(f"csr_graphs/{name}/E", "w") as f:
    for i, ele in enumerate(E):
        f.write(f"{ele}")
        if i != len(E) - 1:
            f.write(" ")
        

with open(f"csr_graphs/{name}/W", "w") as f:
    for i, ele in enumerate(W):
        f.write(f"{ele}")
        if i != len(W) - 1:
            f.write(" ")

rev_graph = defaultdict(list)

ne = len(E)
n = len(V)

for i in range(n):
    u = V[i]
    for j in range(I[i], I[i+1]):
        v = E[j]
        rev_graph[v].append(u)

RE = []
RI = []
prev = 0

for i in range(n):
    RI.append(prev)
    prev += len(rev_graph[i])
    for j in rev_graph[i]:
        RE.append(j)
RI.append(prev)

with open(f"csr_graphs/{name}/RE", "w") as f:
    for i, ele in enumerate(RE):
        f.write(f"{ele}")
        if i != len(RE) - 1:
            f.write(" ")

with open(f"csr_graphs/{name}/RI", "w") as f:
    for i, ele in enumerate(RI):
        f.write(f"{ele}")
        if i != len(RI) - 1:
            f.write(" ")

