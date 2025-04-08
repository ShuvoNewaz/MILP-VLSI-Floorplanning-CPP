#include <fstream>
#include <string>
#include <vector>

using namespace std;

void binary_constraints(ofstream& constraint_file,
                        string output,
                        unsigned short int num_hard_modules,
                        unsigned short int num_total_modules)
{
    constraint_file.open(output, ios::app);
    constraint_file << "/* variable type constraints */\n";
    constraint_file << "bin ";
    for(short int i = 1; i <= num_total_modules; i++)
    {
        for(short int j = 1; j <= num_total_modules; j++)
        {
        if(j > i)
        {
            if(i == num_total_modules-1 && j == num_total_modules)
            {
                constraint_file << "x" << to_string(i) << to_string(j) << ";\n";
            }
            else
            {
                constraint_file << "x" << to_string(i) << to_string(j) << ", ";
            }
        }
        }
    }

    constraint_file << "bin ";
    for(short int i = 1; i <= num_total_modules; i++)
    {
        for(short int j = 1; j <= num_total_modules; j++)
        {
            if(j > i)
            {
                if(i == num_total_modules-1 && j == num_total_modules)
                {
                    constraint_file << "y" << to_string(i) << to_string(j) << ";\n";
                }
                else
                {
                    constraint_file << "y" << to_string(i) << to_string(j) << ", ";
                }
            }
        }
    }

    constraint_file << "bin ";
    for(short int i = 1; i <= num_hard_modules; i++)
    {
        if(i == num_hard_modules)
        {
            constraint_file << "z" << to_string(i) << ";\n";
        }
        else
        {
            constraint_file << "z" << to_string(i) << ", ";
        }
    }
    constraint_file.close();
}