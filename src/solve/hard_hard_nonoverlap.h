#include "utils.h"
#include "fusion.h"

using namespace std;
using namespace mosek::fusion;
using namespace monty;

void hard_hard_nonoverlap(bool hard_exists,
                        unsigned short int num_hard_modules,
                        unsigned short int num_total_modules,
                        vector<float> hard_module_width,
                        vector<float> hard_module_height,
                        float bound,
                        unsigned short int x_offset,
                        unsigned short int y_offset,
                        unsigned short int w_offset,
                        unsigned short int x_ij_offset,
                        unsigned short int total_variables,
                        Model::t M, Variable::t X)
{
    unsigned short int i, j, tri_flat;
    if(hard_exists)
        {
            for(i=0; i<num_hard_modules; i++)
            {
                for(j=0; j<num_hard_modules; j++)
                {
                    if(j > i)
                    {
                        tri_flat = position(num_total_modules, i, j);
                        {
                            auto Coefficients = nonOverlapConstraint(total_variables,
                                                                i, j, y_offset + i, w_offset + tri_flat, x_ij_offset + tri_flat,
                                                                hard_module_height[i] - hard_module_width[i], -bound, -bound);
                            M->constraint(Expr::dot(Coefficients, X), Domain::lessThan(-hard_module_width[i]));
                        }
                        
                        {
                            auto Coefficients = nonOverlapConstraint(total_variables,
                                                                i, j, y_offset + j, w_offset + tri_flat, x_ij_offset + tri_flat,
                                                                hard_module_width[j] - hard_module_height[j], -bound, bound);
                            M->constraint(Expr::dot(Coefficients, X), Domain::greaterThan(hard_module_width[j] - bound));
                        }

                        {
                            auto Coefficients = nonOverlapConstraint(total_variables,
                                                                x_offset + i, x_offset + j, y_offset + i, w_offset + tri_flat, x_ij_offset + tri_flat,
                                                                hard_module_width[i] - hard_module_height[i], -bound, bound);
                            M->constraint(Expr::dot(Coefficients, X), Domain::lessThan(bound - hard_module_height[i]));
                        }

                        {
                            auto Coefficients = nonOverlapConstraint(total_variables,
                                                                x_offset + i, x_offset + j, y_offset + j, w_offset + tri_flat, x_ij_offset + tri_flat,
                                                                hard_module_height[j] - hard_module_width[j], -bound, -bound);
                            M->constraint(Expr::dot(Coefficients, X), Domain::greaterThan(hard_module_height[j] - 2 * bound));
                        }                           
                    }
                }
            }
        }
}