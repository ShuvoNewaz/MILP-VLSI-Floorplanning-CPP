#include "objective.h"
#include "hard_hard_nonoverlap.h"
#include "hard_soft_nonoverlap.h"
#include "soft_soft_nonoverlap.h"
#include "variable_type_constraints.h"
#include "chip_width_constraints.h"
#include "chip_height_constraints.h"
#include "binary_constraints.h"

using namespace std;

/*
    This file puts together the *.lp file containing the constraints for
    the problem. Allows solving using an LP solver.
*/        

void create_lp_file(string lp_solve_files_dir,
                    ofstream& constraint_file,
                    string output,
                    bool hard_exists,
                    bool soft_exists,
                    unsigned short int num_hard_modules,
                    unsigned short int num_soft_modules,
                    unsigned short int num_total_modules,
                    vector<float> hard_module_width,
                    vector<float> hard_module_height,
                    float bound,
                    vector<float> gradient,
                    vector<float> intercept,
                    vector<vector<float>> soft_module_width_range,
                    vector<vector<float>> soft_module_height_range)
{
    system(("mkdir -p " + lp_solve_files_dir).c_str());
    objective(constraint_file,
                output);
    hard_hard_nonoverlap(hard_exists,
                        constraint_file,
                        output,
                        num_hard_modules,
                        hard_module_width,
                        hard_module_height,
                        bound);
    hard_soft_nonoverlap(hard_exists, soft_exists,
                            constraint_file,
                            output,
                            num_hard_modules,
                            num_total_modules,
                            hard_module_width,
                            hard_module_height,
                            gradient,
                            intercept,
                            bound);
    soft_soft_nonoverlap(soft_exists,
                        constraint_file,
                        output,
                        num_hard_modules,
                        num_total_modules,
                        gradient,
                        intercept,
                        bound);
    variable_type_constraints(constraint_file,
                            output,
                            soft_module_width_range,
                            soft_module_height_range,
                            num_hard_modules,
                            num_total_modules);
    chip_width_constraints(constraint_file,
                            output,
                            hard_module_width,
                            hard_module_height,
                            num_hard_modules,
                            num_total_modules);
    chip_height_constraints(constraint_file,
                            output,
                            hard_module_width,
                            hard_module_height,
                            gradient,
                            intercept,
                            num_hard_modules,
                            num_total_modules);
    binary_constraints(constraint_file,
                        output,
                        num_hard_modules,
                        num_total_modules);
}