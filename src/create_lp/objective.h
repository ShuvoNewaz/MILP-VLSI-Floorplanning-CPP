#include <fstream>
#include <string>

using namespace std;

/*
    This file puts together the *.lp file containing the constraints for
    the problem. Allows solving using an LP solver.
*/        

void objective(ofstream& constraint_file, string output)
{
    constraint_file.open(output, ios::out);
    if(constraint_file.is_open())
    {
        constraint_file << "/* Objective Function */\nmin: Y;\n\n\n";
    }
    constraint_file.close();
}