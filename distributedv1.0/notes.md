# Approach
- Excesses and labels are kept local
- During push phase excesses to be added up across overlap (proof needed)
- During relabel, it should not pose a problem as an edge relabels itself and itself alone
- A pulse happens after all processes have no local pushes remaining.
- After a pulse, the excesses for the overlapping nodes are added up.



# Correctness check 
- Serial algorithm.
- Test suite.
- Initialization of heights and excesses.
- One global push.
- Relabel.

# Problems
- Cannot calculate the number of vertices
- Vertices with out degree 0 but with non zero indegrees with edges incoming from other processes.
- Reduce and Broadcast all these excesses. Will using map be enough ? Should be .
- Issues with the reverse csr (Khush) .