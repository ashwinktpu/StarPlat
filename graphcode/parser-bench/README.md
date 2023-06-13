# Benchmarking the Parser

## Potential Optimizations

### Improve File Reading

Currently using the most naive way to read input file. Optimizing that should
lead to a significant speedup.

### Don't Hold All Representations

Currently the `graph` class carries with it all representations of the graph:
- Edge list
- Adjacency list
- CSR
- Reversed CSR

It only needs to hold the adjacency list. We can generate the others on demand.
This would save a lot of memory. For the others, we can generate them on the fly
and create iterators for them.

It currently also generates all these representations while parsing the graph.
Not all of them will always be required. The StarPlat compiler might be able to
detect which ones are required, and generate those accordingly.

**NOTE:** These are breaking changes to the API.


### CSR Calculations

[Godbolt Link](https://godbolt.org/z/d58of58qr)

The compiler is unable to optimize the prefix sum calculations. In principle we
should be able to take advantage of SIMD registers to get a significant (maybe
even 2x) performance boost on using SIMD instructions.


### Sorting

Not sorting the edges by value will give a small performance boost.
Theoretically, graph algorithms should be independent of the order of traversal
of edges.


### Non-Temporal Writes

Since we're working with very large arrays, bypassing the cache and using
non-temporal read/write operations will give us a good speedup (for large
arrays). It might even be able to double the throughput for writes.
