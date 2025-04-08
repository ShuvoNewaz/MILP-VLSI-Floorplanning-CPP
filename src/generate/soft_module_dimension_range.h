#include <string>
#include <tuple>
#include <vector>
#include <math.h>

using namespace std;

tuple<vector<vector<float>>, vector<vector<float>>> soft_module_dimension_range(bool soft_exists,
                                                                            bool underestimation,
                                                                            unsigned short int num_soft_modules,
                                                                            vector<float> area,
                                                                            vector<float>min_aspect,
                                                                            vector<float>max_aspect)
/*
    Returns a tuple containing the minimum and maximum allowable widths and
    heights for the soft modules.
*/
{
    vector<vector<float>> width_range, height_range;
    if(soft_exists)
    {
        float a, min_w, max_w, min_h, max_h;
        for(unsigned short int i = 0; i < num_soft_modules; i++)
        {
            a = area[i];
            min_w = sqrtf(a * min_aspect[i]);   
            max_w = sqrtf(a * max_aspect[i]);
            width_range.push_back({min_w, max_w});
            if(underestimation)
            {
                min_h = a / max_w;
                max_h = a / max_w + (max_w - min_w) * a / (max_w * max_w);
            }
            else
            {
                min_h = a / max_w;
                max_h = a / min_w;
            }
            height_range.push_back(vector<float> {min_h, max_h});
        }                
    }
    else
    {
        width_range = {{0}, {0}};
        height_range = {{0}, {0}};
    }

    return make_tuple(width_range, height_range);
}