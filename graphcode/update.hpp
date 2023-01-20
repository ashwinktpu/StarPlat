#include<vector>
#include<set>
#include<algorithm>
#include<string.h>

class update
{
  public:
  char type;
  int32_t source;
  int32_t destination;
  int32_t weight;
};
 
  std::vector<update> parseUpdateFile(char* updateFile)
    { 
     std::vector<update> updates;
     std::ifstream infile;
     infile.open(updateFile);
     std::string line;

     while (std::getline(infile,line))
     {
      
      // std::stringstream(line);
      
       /*if (line.length()==0||line[0] < '0' || line[0] >'9') {
          continue; 

	    	}*/
        
        std::stringstream ss(line);
        
       
        //edgesTotal++; //TO BE REMOVED 
        
    
        char type;
        int32_t source;
        int32_t destination;
        
        ss>>type; 
        ss>>source;
        ss>>destination;

        update u;
        update u1; //only required for undirected graph case.
        u1.type = type;
        u1.source = destination;
        u1.destination = source;
        u.type = type;
        u.source = source;
        u.destination = destination;

        if(u.type=='a')
          {
            u.weight = 1;
            u1.weight = 1;
          }


        updates.push_back(u);
        updates.push_back(u1); //only required for undirected graph case.
     
         
     }
    
     return updates;
    }

    std::vector<update> getDeletions(int updateIndex, int batchSize, std::vector<update>& updateVec)
     {
       std::vector<update> deletions ;
       for(int batchIndex = updateIndex ; batchIndex < (updateIndex+batchSize) && batchIndex < updateVec.size() ; batchIndex++)
          {   
            update u = updateVec[batchIndex];
               if(u.type == 'd')
                 deletions.push_back(u);

          }
      
       return deletions;

     }

   std::vector<update> getAdditions(int updateIndex, int batchSize, std::vector<update> updateVec)
     {
       std::vector<update> additions ;
       for(int batchIndex = updateIndex ; batchIndex < (updateIndex+batchSize) && batchIndex < updateVec.size() ; batchIndex++)
          {   
            update u = updateVec[batchIndex];
               if(u.type == 'a')
                 additions.push_back(u);

          }
      
       return additions;

     }



