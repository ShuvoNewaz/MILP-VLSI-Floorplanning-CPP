#include <string>
#include <tuple>
#include <vector>

using namespace std;

tuple<vector<float>, vector<float>, vector<float>> soft_module_properties(vector<string> lines,
                                                        unsigned short int num_soft_modules)
/*
    Returns a tuple containing the properties
    (area, minimum aspect ratio, maximum aspect ratio)
    of the soft modules.
*/
{
    bool soft_encountered {false};
    tuple<vector<float>, vector<float>, vector<float>> soft_properties;
    if (num_soft_modules != 0)
    {
        unsigned short int i = 0;
        for(string line : lines)
        {
            if(line.substr(0, 4) == "soft")
            {
                soft_encountered = true;
                continue;
            }
            if(soft_encountered)
            {
                unsigned short int comma_index_1 = line.find(",");
                unsigned short int comma_index_2 = line.rfind(",");
                float area, min_aspect, max_aspect;
                area = stof(line.substr(0, comma_index_1+1));
                min_aspect = stof(line.substr(comma_index_1+1, comma_index_2-comma_index_1));
                max_aspect = stof(line.substr(comma_index_2+1, line.size()-comma_index_2));
                get<0>(soft_properties).push_back(area);
                get<1>(soft_properties).push_back(min_aspect);
                get<2>(soft_properties).push_back(max_aspect);
                i += 1;
                if(i >= num_soft_modules)
                {
                    break;
                }
            }
        }
    }
    else
    {
        get<0>(soft_properties).push_back(0);
        get<1>(soft_properties).push_back(0);
        get<2>(soft_properties).push_back(0);
    }

    return soft_properties;
}