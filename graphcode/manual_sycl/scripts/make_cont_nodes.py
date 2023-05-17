name = "cleancom-orkutud"
f = open(f"csr_graphs/{name}/V")
V = list(map(int, f.readlines()[0].strip().split()))


f = open(f"csr_graphs/{name}/E")
E = list(map(int, f.readlines()[0].strip().split()))

print("Curr max = ", V[-1])
print("Num nodes = ", len(V))

id_map = {V[i] : i for i in range(len(V))}

new_V = [i for i in range(len(V))]

new_E = [id_map[E[i]] for i in range(len(E))]

print(f"Prev edges = {len(E)}, New edges = {len(new_E)}")

with open(f"csr_graphs/{name}/V", "w") as f:
    f.write(" ".join(list(map(str, new_V))))

with open(f"csr_graphs/{name}/E", "w") as f:
    for i, ele in enumerate(new_E):
        f.write(f"{ele}")
        if i != len(new_E) - 1:
            f.write(" ")
        