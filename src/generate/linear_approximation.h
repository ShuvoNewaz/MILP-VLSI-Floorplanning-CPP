#include <tuple>
#include <vector>

using namespace std;

tuple<vector<float>, vector<float>> linear_approximation(bool soft_exists,
                                                        unsigned short int num_soft_modules,
                                                        bool underestimation,
                                                        vector<float> area,
                                                        vector<vector<float>> soft_module_width_range,
                                                        vector<vector<float>> soft_module_height_range)
{
    float a, g, c;
    float min_w, max_w, min_h, max_h;
    vector<float> gradients, intercepts;

    if(soft_exists)
    {
        for(short unsigned int i = 0; i < num_soft_modules; i++)
        {
            a = area[i];
            min_w = soft_module_width_range[i][0];
            max_w = soft_module_width_range[i][1];
            min_h = soft_module_height_range[i][0];
            max_h = soft_module_height_range[i][1];
            if(underestimation)
            {
                g = - a / (max_w * max_w);
                c = 2 * a / max_w;
            }
            else
            {
                g = (max_h - min_h) / (min_w - max_w);
                c = max_h - g * min_w;
            }
            gradients.push_back(g);
            intercepts.push_back(c);
        }
    }
    else
    {
        gradients = {0};
        intercepts = {0};
    }

    return make_tuple(gradients, intercepts);
}