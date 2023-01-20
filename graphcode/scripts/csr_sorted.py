from collections import defaultdict
import random

DEBUG = 0
HERE = 1

name = "simplebcud"
if HERE:
    filename = f"raw_graphs/{name}.txt"
else:
    filename = f"/lfs/usrhome/btech/ee19b106/graph_algorithms/raw_graphs/{name}.txt"

with open(filename) as f:
    l = f.readlines()

# l = l[1:]

for i in range(len(l)):
    try:
        l[i] = list(map(int, l[i].strip().split(" ")))
    except:
        print(f"[WARN] Unable to parse line {i}")

g = defaultdict(list)
seen = set()

for i in range(0, len(l)):
    try:
        [u, v] = l[i]
        w = random.randint(1, 100)
        if DEBUG:
            print(f"{u} {v} {w}")
        g[u].append([v, w])
        seen.add(u)
        seen.add(v)
    except:
        print(f"[WARN] Skipped line {i} = {l[i]}")

V = sorted(list(seen))
maxEdge = V[-1]
try:
    assert V == [i for i in range(maxEdge + 1)]
except:
    print("Not continuous")
E = []
I = []
W = []
pos = 0
for v in V:
    I.append(pos)
    for [nn, nw] in sorted(g[v]):
        E.append(nn)
        W.append(nw)
        pos += 1
I.append(pos)

if DEBUG:
    print(V)
    print(I)
    print(E)
    print(W)

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

