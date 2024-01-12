from graphs.graph import *
from SSSP import Compute_SSSP
from TriangleCounting import Compute_TC

# input in input.txt
# first line is max_node (0 to max_node inclusive)
# following lines are edges: src dest weight

filepath = r'./graphs/input.txt'
my_graph = DirGraph(filepath)
my_graph.parseGraph()
src_node = 0

# result = Compute_SSSP(my_graph, src_node)
result = Compute_TC(my_graph)

print(result)