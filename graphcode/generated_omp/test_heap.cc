#include"test_heap.h"

void Test(graph& g , int src)
{
  Map mp;
  std::vector<std::vector<int>> tn;
  std::vector<int> cn;
  cn.resize(4);

  cn.push_back(2);

  cn.push_back(9);

  cn.push_back(1);

  cn.push_back(0);

  tn.push_back(cn);

  mp.insertel(1,tn);

  mp.searchPair(1,tn);

  mp.searchKey(4,cn);

  mp.deletePair(1,tn);

  mp.deleteKey(4,cn);

  mp.show();


}
