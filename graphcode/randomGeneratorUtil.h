#include<set>
#include<random>
using namespace std;


class randomGeneratorUtil
{   
  private:

  int maxVal;
  int minVal;

  public:
  randomGeneratorUtil(){}
  randomGeneratorUtil(int min,int max)
  {
     maxVal=max;
     minVal=min;
  }

  void feedFromFile(std::set<int>& container, char* fileName)
  {
      std::ifstream infile;
      infile.open(fileName);
      std::string line;
     while (std::getline(infile,line))
     {
     
       if (line.length()==0||line[0] < '0' || line[0] >'9') {
          continue; 

	    	}
        
        std::stringstream ss(line);
        int32_t source;
        ss>>source; 
        container.insert(source);
    }
  }

  void feedRandomUniqueNum(std::set<int>& container, int amount)
  {
    random_device randD;
    mt19937_64 generator(randD());
    uniform_int_distribution<int> dist{minVal,maxVal};

   while(container.size()!=amount)
    {
      
    container.insert(dist(generator));

     }
  
  }
 

};