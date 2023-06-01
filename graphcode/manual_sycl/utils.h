#include <iostream>
#include <fstream>
#include <vector>

void load_from_file(std::string filename, std::vector<int> &vec)
{
    std::ifstream input;
    input.open(filename);
    int num;
    while ((input >> num))
    {
        vec.push_back(num);
    }
    input.close();
}

void print_vector(std::vector<int> vec)
{
    for (auto x : vec)
        std::cout << x << " ";
    std::cout << "len = " << vec.size();
    std::cout << std::endl;
}