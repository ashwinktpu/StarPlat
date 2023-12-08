__attribute__((always_inline)
void dsu_find_inline(__global int* d_dsu,
                       __global int* x,
                       __global int* lock,
                       __global int* comp) {
  int tid = get_global_id(0);
  bool unlocked = false;
  while (!unlocked) {
    if (atomic_xchg(lock, 1)) {
      // critical section
      *comp = d_dsu[*x];
      while (*comp != d_dsu[*comp]) {
        *comp = d_dsu[*comp];
      }

      atomic_xchg(lock, 0);
      unlocked = true;
    }
  }
}

__attribute__((always_inline))
void dsu_merge_inline(__global int* d_dsu,
                      __global int* x,
                      __global int* y,
                      __global int* lock,
                      __global int* comp) {
  int x_comp, y_comp;
  dsu_find_inline(d_dsu, x, &x_comp, lock);
  dsu_find_inline(d_dsu, y, &y_comp, lock);
  bool unlocked = false;
  while (!unlocked) {
    if (atomic_xchg(lock, 1)) {
      // critical section
      d_dsu[y_comp] = x_comp;

      atomic_xchg(lock, 0);
      unlocked = true;
    }
  }
}
