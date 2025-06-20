#include "../generate/generate.h"
#include "fusion.h"
#include "hard_hard_nonoverlap.h"
#include "hard_soft_nonoverlap.h"
#include "soft_soft_nonoverlap.h"
#include "other_constraints.h"
#include <algorithm>

using namespace mosek::fusion;
using namespace monty;


class SolveILP
{
    public:
        GenerateProblem problem;
        short unsigned int num_hard_modules, num_soft_modules, num_total_modules;
        bool hard_exists, soft_exists;

        // Hard module properties
        
        vector<float> hard_module_width, hard_module_height;

        // Soft module properties

        vector<float> soft_area, min_aspect, max_aspect;
        vector<vector<float>> soft_module_width_range, soft_module_height_range;
        vector<float> gradient, intercept;

        float bound;
        
        SolveILP(string, bool, bool); // Class constructor is defined outside of class

        // Set up the MOSEK environment and variables

        Model::t M;
        vector<float> h;
        Variable::t X; // Contains N x's, N y's, N_hard z's, N_soft w's, Nc2 x_ij, Nc2 y_ij, 1 Y

        void create_constraints()
        {

            // Offset indices

            unsigned short int x_offset = num_total_modules;
            unsigned short int y_offset = x_offset + num_total_modules;
            unsigned short int z_offset = y_offset + num_hard_modules;
            unsigned short int w_offset = z_offset + num_soft_modules;
            unsigned short int x_ij_offset = w_offset + NcR(num_total_modules, 2);
            unsigned short int total_variables = 3 * num_total_modules + 2 * NcR(num_total_modules, 2) + 1;
            unsigned short int tri_flat;
            unsigned short int i, j;
                        
            hard_hard_nonoverlap(hard_exists,
                                num_hard_modules,
                                num_total_modules,
                                hard_module_width,
                                hard_module_height,
                                bound,
                                x_offset, y_offset,
                                w_offset, x_ij_offset,
                                total_variables,
                                M, X);

            hard_soft_nonoverlap(hard_exists,
                                soft_exists,
                                num_hard_modules,
                                num_total_modules,
                                hard_module_width,
                                hard_module_height,
                                gradient, intercept,
                                bound,
                                x_offset,y_offset,
                                w_offset, x_ij_offset,
                                total_variables,
                                M, X);

            soft_soft_nonoverlap(soft_exists,
                                num_hard_modules,
                                num_total_modules,
                                gradient, intercept,
                                bound,
                                x_offset,y_offset,
                                w_offset, x_ij_offset,
                                total_variables,
                                M, X);

            other_constraints(hard_exists,
                            soft_exists,
                            num_hard_modules,
                            num_soft_modules,
                            num_total_modules,
                            hard_module_width,
                            hard_module_height,
                            gradient, intercept,
                            soft_module_width_range,
                            x_offset,y_offset, z_offset,
                            w_offset, x_ij_offset,
                            total_variables,
                            M, X);
        }

        tuple<float, vector<float>, vector<float>, vector<float>, vector<float>, vector<float>> solve(float run_time)
        {
            vector<float> x_i, y_i, w_i, h_i, z_i;
            float Y;           
            unsigned short int total_variables = 3 * num_total_modules + 2 * NcR(num_total_modules, 2) + 1;

            // Populate the optimization model and variables

            M = new Model("Floorplan_Optimization");

            auto _M = finally([&]() { M->dispose(); });

            X = M->variable(total_variables, Domain::inRange(0, bound));
            X->slice(2 * num_total_modules, 2 * num_total_modules + num_hard_modules)->makeInteger(); // z_i are binary
            X->slice(3 * num_total_modules, total_variables - 1)->makeInteger(); // x_ij and y_ij are binary
            
            create_constraints();

            // Set max solution time

            // M->setSolverParam("optimizerMaxTime", run_time);
            M->setSolverParam("optimizerMaxTime", run_time);

            // Set max relative gap (to its default value)

            M->setSolverParam("mioTolRelGap", 1e-4);

            // Set max absolute gap (to its default value)

            M->setSolverParam("mioTolAbsGap", 0.1);

            vector<double> A(total_variables);
            A[A.size() - 1] = 1; // Set up the objective
            auto Coefficients = new_array_ptr<double>(A);
            M->objective("Objective", ObjectiveSense::Minimize, Expr::dot(Coefficients, X));
            M->writeTask("VLSI.ptf");            
            M->solve();

            auto solStatus = M->getPrimalSolutionStatus();
            if (solStatus == SolutionStatus::Optimal)
            {
                std::cout << "Optimal solution found.\n";
                M->acceptedSolutionStatus(AccSolutionStatus::Optimal);
            }
            else if (solStatus == SolutionStatus::Feasible)
            {
                std::cout << "Feasible solution found within the time limit.\n";
                M->acceptedSolutionStatus(AccSolutionStatus::Feasible);
            }
            else
            {
                std::cerr << "No feasible solution found within the time limit.\n";
            }

            // Get all the parameter values
            
            auto sol = X->level();
            for(int i=0; i < num_total_modules; i++)
            {
                x_i.push_back((*sol)[i]);
                y_i.push_back((*sol)[i + num_total_modules]);

                if(i < num_hard_modules)
                {
                    z_i.push_back((*sol)[i + 2*num_total_modules]);
                }
                
                if(i < num_soft_modules)
                {
                    w_i.push_back((*sol)[i + 2*num_total_modules + num_hard_modules]);
                    h_i.push_back(gradient[i] * w_i[i] + intercept[i]);
                }
            }
            
            Y = (*sol)[total_variables - 1];

            // Shift modules if doesn't start from 0
            float min_x{Y}, min_y{Y};
            min_x = *std::min_element(x_i.begin(), x_i.end());
            min_y = *std::min_element(y_i.begin(), y_i.end());
            for(size_t i = 0; i < x_i.size(); i++)
            {
                x_i[i] = x_i[i] - min_x;
                y_i[i] = y_i[i] - min_y;
            }

            return make_tuple(Y, x_i, y_i, z_i, w_i, h_i);
        }

        float get_chip_dimension(vector<float> coordinate,
                                vector<float> hard_dim1,
                                vector<float> hard_dim2,
                                vector<float> soft_dim,
                                vector<float> z)
        /*
        hard_dim1 is hard_module_width and hard_dim2 is hard_module_height for
        width computation. Opposite for height computation.

        soft_dim is soft width for width computation, soft height for height computation.
        */
        {
            float chip_dimension{0.0}, module_dimension;
            for(size_t i = 0; i < num_total_modules; i++)
            {
                if(i < num_hard_modules)
                {
                    module_dimension = hard_dim1[i];
                    if(z[i] == 1) // Rotated module
                    {
                        module_dimension = hard_dim2[i];
                    }
                }
                else
                {
                    module_dimension = soft_dim[i - num_hard_modules];
                }
                chip_dimension = max(coordinate[i] + module_dimension, chip_dimension);
            }

            return chip_dimension;
        }

        float export_results(float chip_height,
                            float chip_width,
                            vector<float> x_i,
                            vector<float>y_i,
                            vector<float> z_i,
                            vector<float> w_i,
                            vector<float> h_i,
                            vector<float> utilizations,
                            string output_file_name)
        /*
            Exports the dimensions and coordinates of the optimized blocks to a text file. The results can
            later be read and plotted using Python.
        */
        {
            unsigned short int i;
            vector<float> W, H;
            float utilization, used_area{0};

            /* Final Utilization = 
                (sum_i^num_superblock (utilization * superblock area)) / final_chip_area
            */

            for(i = 0; i < num_hard_modules; i++)
            {
                W.push_back(hard_module_width[i]);
                H.push_back(hard_module_height[i]);
                used_area += utilizations[i] * W[i] * H[i];
            }
            for(i = 0; i < num_soft_modules; i++)
            {
                W.push_back(w_i[i]);
                H.push_back(h_i[i]);
                used_area += utilizations[i + num_hard_modules] *
                                W[i + num_hard_modules] *
                                H[i + num_hard_modules];
            }
            
            float chip_area = chip_height * chip_width;
            utilization = used_area / chip_area;

            // Write outputs to a file

            ofstream output_file;
            output_file.open(output_file_name, ios::out);
            if(output_file.is_open())
            {   
                output_file << to_string(num_hard_modules) + "\n";
                output_file << to_string(num_soft_modules) + "\n";
                writeFile(output_file, x_i, num_total_modules);
                writeFile(output_file, y_i, num_total_modules);
                writeFile(output_file, W, num_total_modules);
                writeFile(output_file, H, num_total_modules);
                if(hard_exists)
                {
                    writeFile(output_file, z_i, num_hard_modules);
                }
                else
                {
                    output_file << "0\n";
                }
                output_file << to_string(utilization) + "\n";
                output_file << to_string(chip_height) + "\n";
                output_file << to_string(chip_width) + "\n";
            }
            
            output_file.close();

            return utilization;
        }
};

// Create constructor for SolveILP

SolveILP::SolveILP(string file, bool underestimation, bool save_lp) : problem(file, underestimation, save_lp)
    {
        num_hard_modules = problem.num_hard_modules;
        num_soft_modules = problem.num_soft_modules;
        num_total_modules = problem.num_total_modules;
        tie(hard_exists, soft_exists) = make_tuple(problem.hard_exists, problem.soft_exists);
        tie(hard_module_width, hard_module_height) = make_tuple(problem.hard_module_width,
                                                                problem.hard_module_height);
        tie(soft_area, min_aspect, max_aspect) = make_tuple(problem.area,
                                                            problem.min_aspect,
                                                            problem.max_aspect);
        tie(soft_module_width_range, soft_module_height_range) = make_tuple(problem.soft_module_width_range,
                                                                            problem.soft_module_height_range);
        tie(gradient, intercept) = make_tuple(problem.gradient, problem.intercept);
        bound = problem.bound;

        for(unsigned short int i=0; i<max<unsigned short int>(1, num_soft_modules); i++)
        {
            h.push_back(0);
        }
    }