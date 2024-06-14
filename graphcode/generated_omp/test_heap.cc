#include"test_heap.h"

void Test(graph& g , int src)
{
  int maxSize = 100;
  int siz = 4;
  Heap hp;
  std::vector<int> cn;
  cn.resize(4);

  cn.push_back(2);

  cn.push_back(9);

  cn.push_back(1);

  cn.push_back(0);

  double starttime = rtclock();
  hp.insertE(cn,siz);

  double endtime = rtclock();
  starttime = rtclock();
  std::vector<int> cnn;
  cnn = hp1.deleteElem(2);
  endtime = rtclock();

}
