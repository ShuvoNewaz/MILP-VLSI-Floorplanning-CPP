#include <fstream>
#include <string>
#include <vector>

using namespace std;

void variable_type_constraints(ofstream& constraint_file,
                            string output,
                            vector<vector<float>> soft_module_width_range,
                            vector<vector<float>> soft_module_height_range,
                            unsigned short int num_hard_modules,
                            unsigned short int num_total_modules)
{
    float soft_width_min, soft_width_max;
    constraint_file.open(output, ios::app);
    constraint_file << "/* variable type constraints */\n";

    auto soft_range = make_tuple(soft_module_width_range, soft_module_height_range);

    for(short unsigned int i = 1; i <= num_total_modules; i++)
    {
        constraint_file << "x" << to_string(i) << " >= 0;\n";
        constraint_file << "y" << to_string(i) << " >= 0;\n";
    }
    for(short unsigned int i = num_hard_modules+1; i <= num_total_modules; i++)
    {
        soft_width_min = soft_module_width_range[i-num_hard_modules-1][0];
        soft_width_max = soft_module_width_range[i-num_hard_modules-1][1];
        
        constraint_file << "w" << to_string(i) << " >= " << to_string(soft_width_min) << ";\n";
        constraint_file << "w" << to_string(i) << " <= " << to_string(soft_width_max) << ";\n";
    }
    constraint_file << "\n\n";
    constraint_file.close();
}