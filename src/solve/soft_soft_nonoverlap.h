// #include "utils.h"
#include "fusion.h"
#include <vector>

using namespace std;
using namespace mosek::fusion;
using namespace monty;

void soft_soft_nonoverlap(bool soft_exists,
                        unsigned short int num_hard_modules,
                        unsigned short int num_total_modules,
                        vector<float> gradient,
                        vector<float> intercept,
                        float bound,
                        unsigned short int x_offset,
                        unsigned short int y_offset,
                        unsigned short int w_offset,
                        unsigned short int x_ij_offset,
                        unsigned short int total_variables,
                        Model::t M, Variable::t X)
{
    unsigned short int i, j, tri_flat;
    if(soft_exists)
            {
                for(i=num_hard_modules; i<num_total_modules; i++)
                {
                    for(j=num_hard_modules; j<num_total_modules; j++)
                    {
                        if(j > i)
                        {
                            tri_flat = position(num_total_modules, i, j);

                            {
                                auto Coefficients = nonOverlapConstraint(total_variables,
                                                                    i, j, y_offset + i, w_offset + tri_flat, x_ij_offset + tri_flat,
                                                                    1, -bound, -bound);
                                M->constraint(Expr::dot(Coefficients, X), Domain::lessThan(0));
                            }

                            {
                                auto Coefficients = nonOverlapConstraint(total_variables,
                                                                    i, j, y_offset + j, w_offset + tri_flat, x_ij_offset + tri_flat,
                                                                    -1, -bound, bound);
                                M->constraint(Expr::dot(Coefficients, X), Domain::greaterThan(-bound));
                            }

                            {
                                auto Coefficients = nonOverlapConstraint(total_variables,
                                                                    x_offset + i, x_offset + j, y_offset + i, w_offset + tri_flat, x_ij_offset + tri_flat,
                                                                    gradient[i-num_hard_modules], -bound, bound);
                                M->constraint(Expr::dot(Coefficients, X), Domain::lessThan(bound - intercept[i-num_hard_modules]));
                            }

                            {
                                auto Coefficients = nonOverlapConstraint(total_variables,
                                                                    x_offset + i, x_offset + j, y_offset + j, w_offset + tri_flat, x_ij_offset + tri_flat,
                                                                    -gradient[j-num_hard_modules], -bound, -bound);
                                M->constraint(Expr::dot(Coefficients, X), Domain::greaterThan(intercept[j-num_hard_modules] - 2 * bound));
                            }
                        }
                    }
                }
            }
}