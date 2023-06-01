from collections import defaultdict
import random


class Graph:
    def __init__(self, N = 0, g = defaultdict(list)):
        self.V = N
        self.graph = g
        self.comps = []
        
    def addEdge(self,u,v):
        self.graph[u].append(v)
        
    def DFSUtil(self, v):
        self.visited[v]= True
        self.curr.append(v)
        for i in self.graph[v]:
            if self.visited[i]==False:
                self.DFSUtil(i)
 
 
    def fillOrder(self, v):
        self.visited[v]= True
        for i in self.graph[v]:
            if self.visited[i]==False:
                self.fillOrder(i)
        self.stack.append(v)

    def getTranspose(self):
        g = Graph(self.V)
        for i in self.graph:
            for j in self.graph[i]:
                g.addEdge(j,i)
        return g

    def printSCCs(self):
        self.stack = []
        self.visited =[False]*(self.V)
        for i in range(self.V):
            if self.visited[i]==False:
                self.fillOrder(i)

        gr = self.getTranspose()
          
        gr.visited =[False]*(self.V)
        while self.stack:
            i = self.stack.pop()
            gr.curr = []
            if not gr.visited[i]:
                gr.DFSUtil(i)
                self.comps.append(gr.curr.copy())

        self.comps.sort(reverse=True, key = lambda x : len(x))
        for comp in self.comps:
            print(len(comp), comp[0])
  
DEBUG = 0
HERE = 1

name = "simple_random"
if HERE:
    filename = f"raw_graphs/{name}.txt"
else:
    filename = f"/lfs/usrhome/btech/ee19b106/graph_algorithms/raw_graphs/{name}.txt"

with open(filename) as f:
    l = f.readlines()

l = l[1:]

for i in range(len(l)):
    l[i] = list(map(int, l[i].strip().split(" ")))

g = defaultdict(list)
seen = set()

for i in range(0, len(l)):
    [u, v] = l[i]
    g[u].append(v)
    seen.add(u)
    seen.add(v)

G = Graph(len(seen), g)
G.printSCCs()