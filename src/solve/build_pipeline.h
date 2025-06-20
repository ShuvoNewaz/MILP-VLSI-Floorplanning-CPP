#include "solve.h"
#include "../augment.h"
#include "../save_dimensions.h"


string printBool(bool Bool)
{
    string out;
    if (Bool) {out = "True";}
    else {out = "False";}
    return out;
}

bool parseBool(string Bool)
{
    return (Bool == "true" || Bool == "True");
}

inline tuple<float, float> mainProcess(int num_blocks,
                                        bool underestimation,
                                        bool save_lp,
                                        bool successive_augmentation,
                                        float runtime,
                                        string result_dir,
                                        vector<float> utilizations,
                                        bool final_layout, int i=0)
// Builds the main solver pipeline
{
    float utilization;
    string output_file_name, plot_command, src_file_path, sa_files_dir;
    sa_files_dir = "spec_files/successive_augmentation/" + to_string(num_blocks) + "/";
    if(final_layout)
    {
        cout << "\nFinal Optimization\n";
        src_file_path = successive_augmentation ? 
                        sa_files_dir + to_string(num_blocks) + "_sa.ilp" :
                        "spec_files/" + to_string(num_blocks) + "_block.ilp";
        output_file_name = result_dir + to_string(num_blocks) + "_sa_" + printBool(successive_augmentation) + ".txt";
        plot_command = "python src/visualize.py -f " + output_file_name + " --glob True --sa " + printBool(successive_augmentation) + " -show True";
    }
    else
    {
        cout << "\nOptimizing sub block " << to_string(i) << endl;
        src_file_path = sa_files_dir + to_string(num_blocks) + "_" + to_string(i) + ".ilp";
        output_file_name = result_dir + to_string(num_blocks) + "_" + to_string(i) + ".txt";
        plot_command = "python src/visualize.py -f " + output_file_name + " --glob False --sa " + printBool(successive_augmentation) + " -idx " + to_string(i) + " -show True";
    }
    SolveILP problem = SolveILP(src_file_path, underestimation, save_lp);
    vector<float>x_i, y_i, z_i, w_i, h_i;
    float Y, chip_width, chip_height;
    tie(Y, x_i, y_i, z_i, w_i, h_i) = problem.solve(runtime);
    chip_width = problem.get_chip_dimension(x_i, problem.hard_module_width,
                    problem.hard_module_height,
                    w_i, z_i);
    chip_height = problem.get_chip_dimension(y_i, problem.hard_module_height,
                    problem.hard_module_width,
                    h_i, z_i);

    utilization = problem.export_results(chip_height, chip_width,
                                        x_i, y_i, z_i, w_i, h_i,
                                        utilizations, output_file_name);
    system(plot_command.c_str());

    return make_tuple(utilization, Y);
}