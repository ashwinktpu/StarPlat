#ifndef __FILE_READER__
#define __FILE_READER__
#include <vector>
#include <stdio.h>
void readFile (char * fileName, int &size, std::vector<std::vector<int> > &transactions) ;
void writeFile (char * fileName, std::vector<int> result ) ;
#endif
