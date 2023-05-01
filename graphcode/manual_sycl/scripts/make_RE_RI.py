from collections import defaultdict
name = "simple"

f = open(f"csr_graphs/{name}/V")
V = list(map(int, list(f.readlines()[0].strip().split())))

f = open(f"csr_graphs/{name}/I")
I = list(map(int, list(f.readlines()[0].strip().split())))

f = open(f"csr_graphs/{name}/E")
E = list(map(int, list(f.readlines()[0].strip().split())))

rev_graph = defaultdict(list)

ne = len(E)
n = len(V)

for i in range(n):
    u = V[i]
    for j in range(I[i], I[i+1]):
        v = E[j]
        rev_graph[v].append(u)

# print(rev_graph)

RE = []
RI = []
prev = 0
for i in range(n):
    RI.append(prev)
    prev += len(rev_graph[i]) 
    for j in rev_graph[i]:
        RE.append(j)
RI.append(prev)

# print(RI)    
# print(RE)

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


