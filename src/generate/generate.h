#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <math.h>

#include "total_modules.h"
#include "hard_module_dimension.h"
#include "soft_module_properties.h"
#include "soft_module_dimension_range.h"
#include "linear_approximation.h"
#include "upper_bound.h"
#include "../create_lp/create_lp_file.h"

using namespace std;

class GenerateProblem
{
    public:
        bool hard_exists{false};
        bool soft_exists{false};
        unsigned short int num_hard_modules, num_soft_modules, num_total_modules;
        vector<float> hard_module_width, hard_module_height;
        vector<float> area, min_aspect, max_aspect;
        vector<vector<float>> soft_module_width_range, soft_module_height_range;
        vector<float> gradient, intercept;
        float bound;

        string lp_solve_files_dir;
        bool underestimation;
        vector<string> lines;
        string output;
        ofstream constraint_file;

        GenerateProblem(string, bool, bool); // Constructor is defined outside the class
};

// Create constructor for GenerateProblem

GenerateProblem::GenerateProblem(string file, bool u, bool save_lp)
    {
        underestimation = u;
        ifstream f; // Read file contents into f
        f.open(file);
        string line;
        if(f.is_open())
        {
            while(getline(f, line)) // store contents of f in line
            {
                lines.push_back(line);
            }
            f.close();
        }

        tie(num_hard_modules, num_soft_modules) = total_modules(lines);
        num_total_modules = num_hard_modules + num_soft_modules;
        hard_exists = num_hard_modules > 0;
        soft_exists = num_soft_modules > 0;
        tie(hard_module_width, hard_module_height) = hard_module_dimension(lines, num_hard_modules);
        tie(area, min_aspect, max_aspect) = soft_module_properties(lines, num_soft_modules);
        tie(soft_module_width_range, soft_module_height_range) = soft_module_dimension_range(soft_exists,
                                                                                        underestimation,
                                                                                        num_soft_modules,
                                                                                        area,
                                                                                        min_aspect,
                                                                                        max_aspect);
        tie(gradient, intercept) = linear_approximation(soft_exists,
                                                    num_soft_modules,
                                                    underestimation,
                                                    area,
                                                    soft_module_width_range,
                                                    soft_module_height_range);
        bound = upper_bound(num_hard_modules,
                            num_soft_modules,
                            hard_module_width,
                            hard_module_height,
                            soft_module_width_range,
                            soft_module_height_range);
        lp_solve_files_dir = "lp_solve_files/" + to_string(num_total_modules) + "/";
        output = lp_solve_files_dir + to_string(num_total_modules) + "_blocks_constraints.lp";
        if(save_lp)
        {
            cout << "Saving to " + output << endl;
            create_lp_file(lp_solve_files_dir, constraint_file,
                            output, hard_exists, soft_exists,
                            num_hard_modules, num_soft_modules,
                            num_total_modules, hard_module_width,
                            hard_module_height, bound, gradient, intercept,
                            soft_module_width_range, soft_module_height_range);
        }
    }