#include <fstream>
#include <string>
#include <vector>

using namespace std;

void hard_soft_nonoverlap(bool hard_exists, bool soft_exists,
                        ofstream& constraint_file,
                        string output,
                        unsigned short int num_hard_modules,
                        unsigned short int num_total_modules,
                        vector<float> hard_module_width,
                        vector<float> hard_module_height,
                        vector<float> gradient,
                        vector<float> intercept,
                        float bound)
{
    if(hard_exists && soft_exists)
    {
        float width_h, height_h, g, c;
        constraint_file.open(output, ios::app);
        constraint_file << "/* Non-overlap constraints hard-soft */\n";

        for(short unsigned int i = 1; i <= num_hard_modules; i++)
        {
            for(short unsigned int j = num_hard_modules+1; j <= num_total_modules; j++)
            {
                width_h = hard_module_width[i-1];
                height_h = hard_module_height[i-1];

                g = gradient[j-num_hard_modules-1];
                c = intercept[j-num_hard_modules-1];

                constraint_file << "x" << to_string(i) << " + " << to_string(height_h) << " z" << to_string(i) << " + " << to_string(width_h) << " - " << to_string(width_h) << " z" << to_string(i) << " <= x" << to_string(j) << " + " << to_string(roundf(bound)) << " x" << to_string(i) << to_string(j) << " + " << to_string(roundf(bound)) << " y" << to_string(i) << to_string(j) << ";\n";
                constraint_file << "x" << to_string(i) << " - w" << to_string(j) << " >= x" << to_string(j) << " - " << to_string(roundf(bound)*1) << " + " << to_string(roundf(bound)) << " x" << to_string(i) << to_string(j) << " - " << to_string(roundf(bound)) << " y" << to_string(i) << to_string(j) << ";\n";
                constraint_file << "y" << to_string(i) << " + " << to_string(width_h) << " z" << to_string(i) << " + " << to_string(height_h) << " - " << to_string(height_h) << " z" << to_string(i) << " <= y" << to_string(j) << " + " << to_string(roundf(bound)*1) << " + " << to_string(roundf(bound)) << " x" << to_string(i) << to_string(j) << " - " << to_string(roundf(bound)) << " y" << to_string(i) << to_string(j) << ";\n";
                constraint_file << "y" << to_string(i) << " + " << to_string(-1*g) << " w" << to_string(j) << " - " << to_string(c) << " >= y" << to_string(j) << " - " << to_string(roundf(bound)*2) << " + " << to_string(roundf(bound)) << " x" << to_string(i) << to_string(j) << " + " << to_string(roundf(bound)) << " y" << to_string(i) << to_string(j) << ";\n\n\n";
            }
        }
        constraint_file.close();
    }
}