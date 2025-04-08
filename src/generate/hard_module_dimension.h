#include <string>
#include <tuple>
#include <vector>

using namespace std;

tuple<vector<float>, vector<float>> hard_module_dimension(vector<string> lines,
                                                    unsigned short int num_hard_modules)
/*
    Returns tuple containing the dimensions (width, height)
    of the hard modules.
*/
{
    bool hard_exists = num_hard_modules > 0;
    tuple<vector<float>, vector<float>> hard_dimension;
    if (hard_exists)
    {
        unsigned short int i = 0;

        for(string line : lines)
        {
            if(line.substr(0, 4) == "hard")
            {
                continue;
            }
            
            unsigned short int comma_index = line.find(",");
            float width, height;
            width = stof(line.substr(0, comma_index+1));
            height = stof(line.substr(comma_index+1, line.size()-comma_index));
            get<0>(hard_dimension).push_back(width);
            get<1>(hard_dimension).push_back(height);
            i += 1;
            if(i >= num_hard_modules)
            {
                break;
            }
        }
    }
    else
    {
        get<0>(hard_dimension).push_back(0);
        get<1>(hard_dimension).push_back(0);;
    }

    return hard_dimension;
}