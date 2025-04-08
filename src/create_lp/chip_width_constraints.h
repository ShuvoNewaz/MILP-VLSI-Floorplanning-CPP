#include <fstream>
#include <string>
#include <vector>

using namespace std;

void chip_width_constraints(ofstream& constraint_file,
                        string output,
                        vector<float> hard_module_width,
                        vector<float> hard_module_height,
                        unsigned short int num_hard_modules,
                        unsigned short int num_total_modules)
{
    float width_h, height_h;
    constraint_file.open(output, ios::app);
    constraint_file << "/* chip width constraints */\n";

    for(short unsigned int i = 1; i <= num_hard_modules; i++)
    {
        width_h = hard_module_width[i-1];
        height_h = hard_module_height[i-1];

        constraint_file << "x" << to_string(i) << " + " << to_string(width_h) << " - " << to_string(width_h) << " z" << to_string(i) << " + " << to_string(height_h) << " z" << to_string(i) << " <= Y;\n";
    }
    for(short unsigned int i = num_hard_modules+1; i <= num_total_modules; i++)
    {
        constraint_file << "x" << to_string(i) << " + w" << to_string(i) << " <= Y;\n"; 
    }
    constraint_file << "\n\n";
    constraint_file.close();
}