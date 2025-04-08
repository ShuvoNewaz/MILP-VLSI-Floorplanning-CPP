#include <tuple>
#include <vector>

using namespace std;

float upper_bound(unsigned short int num_hard_modules,
                    unsigned short int num_soft_modules,
                    vector<float> hard_module_width,
                    vector<float> hard_module_height,
                    vector<vector<float>> soft_module_width_range,
                    vector<vector<float>> soft_module_height_range)
/*
        Assuming a square chip block, this functions returns the maximum required
        length (and width) of a block that can be formed using the given modules.
*/
{
    float W_hard{0}, H_hard{0}, W_soft{0}, H_soft{0};
    float W_block, H_block;

    for(short unsigned int i = 0; i < num_hard_modules; i++)
    {
        W_hard += max(hard_module_width[i], hard_module_height[i]);
    }
    H_hard = W_hard;

    for(short unsigned int i = 0; i < num_soft_modules; i++)
    {
        W_soft += soft_module_width_range[i][1];
        H_soft += soft_module_height_range[i][1];
    }

    W_block = W_hard + W_soft;
    H_block = H_hard + H_soft;

    return max(W_block, H_block);
}