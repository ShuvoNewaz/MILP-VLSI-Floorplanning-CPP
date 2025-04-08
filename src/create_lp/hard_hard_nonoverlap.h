#include <fstream>
#include <string>
#include <vector>

using namespace std;

void hard_hard_nonoverlap(bool hard_exists,
                        ofstream& constraint_file,
                        string output,
                        unsigned short int num_hard_modules,
                        vector<float> hard_module_width,
                        vector<float> hard_module_height,
                        float bound)
{
    if(hard_exists)
    {
        float width_i, height_i, width_j, height_j;
        constraint_file.open(output, ios::app);
        constraint_file << "/* Non-overlap constraints hard-hard */\n";

        for(short unsigned int i = 1; i <= num_hard_modules; i++)
        {
            for(short unsigned int j = 1; j <= num_hard_modules; j++)
            {
                if(j > i)
                {
                    width_i = hard_module_width[i-1];
                    height_i = hard_module_height[i-1];
                    width_j = hard_module_width[j-1];
                    height_j = hard_module_height[j-1];

                    constraint_file << "x" << to_string(i) << " + " << to_string(height_i) << " z" << to_string(i) << " + " << to_string(width_i) << " - " << to_string(width_i) << " z" << to_string(i) << " <= x" << to_string(j) << " + " << to_string(roundf(bound)) + " x" << to_string(i) << to_string(j) << " + " << to_string(roundf(bound)) << " y" << to_string(i) << to_string(j) << ";\n";
                    constraint_file << "x" << to_string(i) << " - " << to_string(height_j) << " z" << to_string(j) << " - " << to_string(width_j) << " + " << to_string(width_j) << " z" << to_string(j) << " >= x" << to_string(j) << " - " << to_string(roundf(bound)*1) << " + " << to_string(roundf(bound)) << " x" << to_string(i) << to_string(j) << " - " << to_string(roundf(bound)) << " y" << to_string(i) << to_string(j) << ";\n";
                    constraint_file << "y" << to_string(i) << " + " << to_string(width_i) << " z" << to_string(i) << " + " << to_string(height_i) << " - " << to_string(height_i) << " z" << to_string(i) << " <= y" << to_string(j) << " + " << to_string(roundf(bound)*1) << " + " << to_string(roundf(bound)) << " x" << to_string(i) << to_string(j) << " - " << to_string(roundf(bound)) << " y" << to_string(i) << to_string(j) << ";\n";
                    constraint_file << "y" << to_string(i) << " - " << to_string(width_j) << " z" << to_string(j) << " - " << to_string(height_j) << " + " << to_string(height_j) << " z" << to_string(j) << " >= y" << to_string(j) << " - " << to_string(roundf(bound)*2) << " + " << to_string(roundf(bound)) << " x" << to_string(i) << to_string(j) << " + " << to_string(roundf(bound)) << " y" << to_string(i) << to_string(j) << ";\n\n\n";
                }
            }
        }
        constraint_file.close();
    }
}