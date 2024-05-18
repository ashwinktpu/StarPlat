#include"test1.h"

void test1(graph& gg)
{
  int var_decl_outside_paralel_used_inside_parallel_only = 1010;
  int var_decl_outside_parallel_used_outside_parallel_only = 8438;
  int var_decl_outside_parallel_used_in_and_out_of_parallel = 483;
  #pragma acc data copyin(gg)
  {
    #pragma acc data copy(var_decl_outside_parallel_used_in_and_out_of_parallel)
    {
      #pragma acc parallel loop
      for (int v = 0; v < gg.num_nodes(); v ++) 
      {
        var_decl_outside_parallel_used_inside_parallel_only++;
        var_decl_outside_parallel_used_in_and_out_of_parallel++;
        int var_decl_in_parallel_used_inside_parallel_only = 1010;
        var_decl_in_parallel_used_inside_parallel_only++;
      }
    }
  }
  var_decl_outside_parallel_used_outside_parallel_only--;
  var_decl_outside_parallel_used_in_and_out_of_parallel++;

}
