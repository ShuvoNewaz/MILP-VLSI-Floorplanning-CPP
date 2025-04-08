#include <fstream>
#include <string>
#include <vector>

using namespace std;

void soft_soft_nonoverlap(bool soft_exists,
                        ofstream& constraint_file,
                        string output,
                        unsigned short int num_hard_modules,
                        unsigned short int num_total_modules,
                        vector<float> gradient,
                        vector<float> intercept,
                        float bound)
{
    if(soft_exists)
    {
        float g_i, c_i, g_j, c_j;

        constraint_file.open(output, ios::app);
        constraint_file << "/* Non-overlap constraints soft-soft */\n";

        for(short unsigned int i = num_hard_modules+1; i <= num_total_modules; i++)
        {
            for(short unsigned int j = num_hard_modules+1; j <= num_total_modules; j++)
            {
                if(j > i)
                {
                    g_i = gradient[i-num_hard_modules-1];
                    c_i = intercept[i-num_hard_modules-1];
                    g_j = gradient[j-num_hard_modules-1];
                    c_i = intercept[j-num_hard_modules-1];

                    constraint_file << "x" << to_string(i) << " + w" << to_string(i) << " <= x" << to_string(j) << " + " << to_string(roundf(bound)*1) << " x" << to_string(i) << to_string(j) << " + " << to_string(roundf(bound)) << " y" << to_string(i) << to_string(j) << ";\n";
                    constraint_file << "x" << to_string(i) << " - w" << to_string(j) << " >= x" << to_string(j) << " - " << to_string(roundf(bound)*1) << " + " << to_string(roundf(bound)) << " x" << to_string(i) << to_string(j) << " - " << to_string(roundf(bound)) << " y" << to_string(i) << to_string(j) << ";\n";
                    constraint_file << "y" << to_string(i) << " - " << to_string(-1*g_i) << " w" << to_string(i) << " + " << to_string(c_i) << " <= y" << to_string(j) << " + " << to_string(roundf(bound)*1) << " + " << to_string(roundf(bound)) << " x" << to_string(i) << to_string(j) << " - " << to_string(roundf(bound)) << " y" << to_string(i) << to_string(j) << ";\n";
                    constraint_file << "y" << to_string(i) << " + " << to_string(-1*g_j) << " w" << to_string(j) << " - " << to_string(c_j) << " >= y" << to_string(j) << " - " << to_string(roundf(bound)*2) << " + " << to_string(roundf(bound)) << " x" << to_string(i) << to_string(j) << " + " << to_string(roundf(bound)) << " y" << to_string(i) << to_string(j) << ";\n\n\n";
                }
            }
        }
        constraint_file.close();
    }
}