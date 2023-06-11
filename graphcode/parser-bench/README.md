# Benchmarking the Parser


## Current Benchmarks

Binaries created using:
```
g++ -o unopt main.cpp
g++ -O2 -o opt main.cpp
```

### Hyperfine Output

N = 500000

Benchmark 1: ./unopt
  Time (mean ± σ):      2.117 s ±  0.084 s    [User: 2.057 s, System: 0.057 s]
  Range (min … max):    2.050 s …  2.313 s    10 runs

Benchmark 2: ./opt
  Time (mean ± σ):     908.2 ms ±  20.0 ms    [User: 848.7 ms, System: 58.7 ms]
  Range (min … max):   884.2 ms … 941.4 ms    10 runs

Summary
  './opt' ran
    1.02 ± 0.04 times faster than './opt3'
    2.33 ± 0.11 times faster than './unopt'


## Potential Optimizations

### CSR Creation

[Godbolt Link](https://godbolt.org/z/d58of58qr)

The compiler is unable to optimize the prefix sum calculations. In principle we
should be able to take advantage of SIMD registers to get a significant (maybe
even 2x) performance boost on using SIMD instructions.


### Sorting

Not sorting the edges by value will give a small performance boost.
Theoretically, graph algorithms should be independent of the order of traversal
of edges.
