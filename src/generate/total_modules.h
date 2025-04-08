#include <string>
#include <tuple>
#include <vector>

using namespace std;

tuple<int, int> total_modules(vector<string> lines)
/*
    Returns a tuple (number of hard modules, number of soft modules)
    found in the input file.
*/        
{
    unsigned short int num_hard_modules{0}, num_soft_modules{0};

    for(string line : lines)
    {
        if(line.substr(0, 4) == "hard")
        {
            num_hard_modules = stoi(line.substr(7, line.size() - 7));
        }
        else if(line.substr(0, 4) == "soft")
        {
            num_soft_modules = stoi(line.substr(7, line.size() - 7));
        }
    }

    return make_tuple(num_hard_modules, num_soft_modules);
}