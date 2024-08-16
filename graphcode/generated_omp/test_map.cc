#include"test_map.h"

void Test(graph& g , int src)
{
  Map mp;
  std::vector<std::vector<int>> tn;
  std::vector<int> cn;
  cn.resize(2);

  cn.push_back(2);

  cn.push_back(9);

  tn.push_back(cn);

  cn.resize(2);

  cn.push_back(2);

  cn.push_back(9);

  mp.insertel(2,tn);

  mp.searchPair(2,tn);

  mp.searchKey(2,cn);

  mp.deletePair(2,tn);

  mp.deleteKey(4,cn);


}
