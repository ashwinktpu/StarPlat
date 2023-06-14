'''
Generates an Erdos-Renyi random graph
Outputs edges list
'''

import igraph as ig

def main():
    N = 5000000
    g = ig.Graph.Erdos_Renyi(n=N, m=N)
    for e in g.es:
        print(e.source, e.target)

if __name__ == "__main__":
    main()
