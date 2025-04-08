// #include "utils.h"
#include "fusion.h"
#include <vector>

using namespace std;
using namespace mosek::fusion;
using namespace monty;

void other_constraints(bool hard_exists,
                    bool soft_exists,
                    unsigned short int num_hard_modules,
                    unsigned short int num_soft_modules,
                    unsigned short int num_total_modules,
                    vector<float> hard_module_width,
                    vector<float> hard_module_height,
                    vector<float> gradient,
                    vector<float> intercept,
                    vector<vector<float>> soft_module_width_range,
                    unsigned short int x_offset,
                    unsigned short int y_offset,
                    unsigned short int z_offset,
                    unsigned short int w_offset,
                    unsigned short int x_ij_offset,
                    unsigned short int total_variables,
                    Model::t M, Variable::t X)
{
    unsigned short int i, j, tri_flat;
    for(i=0; i<num_total_modules; i++)
    {
        greaterThanZeroConstraint(total_variables, i, M, X);
        greaterThanZeroConstraint(total_variables, x_offset + i, M, X);
    }
    
    for(i=0; i<num_soft_modules; i++)
    {
        double w_min = soft_module_width_range[i][0];
        double w_max = soft_module_width_range[i][1];
        softWidthConstraint(total_variables, z_offset+i, w_min, w_max, M, X);
    }

    for(i=0; i<NcR(num_total_modules, 2); i++)
    {
        binaryConstraint(total_variables, w_offset+i, M, X);
        binaryConstraint(total_variables, x_ij_offset+i, M, X);
    }

    if(hard_exists)
    {
        for(i=0; i < num_hard_modules; i++)
        {
            binaryConstraint(total_variables, y_offset+i, M, X);
        }

        for(i=0; i<num_hard_modules; i++)
        {
            chipDimensionConstraint(total_variables, i, y_offset+i, hard_module_height[i], hard_module_width[i], M, X); // Chip width constraint
            chipDimensionConstraint(total_variables, x_offset+i, y_offset+i, hard_module_width[i], hard_module_height[i], M, X); // Chip height constraint
        }
    }
    if(soft_exists)
    {
        for(i=num_hard_modules; i<num_total_modules; i++)
        {
            {
                vector<double> A(total_variables);
                A[i] = 1; // x_i = 1
                A[y_offset + i] = 1; // w_i = 1
                A[A.size() - 1] = -1; // Y = -1
                auto Coefficients = new_array_ptr<double>(A);
                M->constraint(Expr::dot(Coefficients, X), Domain::lessThan(0));
            }

            {
                vector<double> A(total_variables);
                A[x_offset + i] = 1; // y_i = 1
                A[y_offset + i] = gradient[i-num_hard_modules]; // w_i = m_i
                A[A.size() - 1] = -1; // Y = -1
                auto Coefficients = new_array_ptr<double>(A);
                M->constraint(Expr::dot(Coefficients, X), Domain::lessThan(-intercept[i-num_hard_modules]));
            }
        }
    }
}