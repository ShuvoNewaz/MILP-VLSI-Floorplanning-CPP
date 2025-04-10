#include "src/solve/solve.h"
#include "src/augment.h"
#include"src/save_dimensions.h"

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

int main(int argc, char *argv[])
{
    int num_blocks{stoi(argv[1])}, sub_block_size{stoi(argv[6])}, num_augmentations, i;
    bool underestimation{parseBool(argv[2])}, successive_augmentation{parseBool(argv[3])},
    visualize_superblock{parseBool(argv[5])}, save_lp{parseBool(argv[7])};
    float runtime{2};
    string src_file_path;
    string spec_files_dir = "spec_files/";
    string result_dir = "results/" + to_string(num_blocks) + "/";
    string sa_files_dir = spec_files_dir + "successive_augmentation/" + to_string(num_blocks) + "/";
    string file = to_string(num_blocks) + "_block.ilp";
    system(("rm -Rf " + result_dir).c_str());
    system(("mkdir -p " + result_dir).c_str());

    vector<float> utilizations;
    float utilization;
    if(successive_augmentation)
    {   
        if(sub_block_size > num_blocks)
        {
            cerr << "Error: Sub-block size exceeds the total number of blocks.\n";
            return -1;
        }

        num_augmentations = int(ceil(float(num_blocks) / float(sub_block_size)));
        cout << "Number of augmentations: " << num_augmentations << endl;
        vector<float> final_dimensions;
        
        system(("rm -Rf " + sa_files_dir).c_str());
        system(("mkdir -p " + sa_files_dir).c_str());
        Augment aug = Augment(file);
        aug.break_problem(sub_block_size);
        
        for(i=1; i<num_augmentations+1; i++)
        {
            cout << "\nOptimizing sub block " << to_string(i) << endl;
            src_file_path = sa_files_dir + to_string(num_blocks) + "_" + to_string(i) + ".ilp";
            SolveILP problem = SolveILP(src_file_path, underestimation, false);
            vector<float>x_i, y_i, z_i, w_i, h_i;
            float Y, chip_width, chip_height;
            tie(Y, x_i, y_i, z_i, w_i, h_i) = problem.solve(runtime, true);
            final_dimensions.push_back(Y);
            chip_width = problem.get_chip_dimension(x_i, problem.hard_module_width,
                                                    problem.hard_module_height,
                                                    w_i, z_i);
            chip_height = problem.get_chip_dimension(y_i, problem.hard_module_height,
                                                    problem.hard_module_width,
                                                    h_i, z_i);
            // cout << Y << ' ' << chip_width << endl;
            string output_file_name = result_dir + to_string(num_blocks) + "_" + to_string(i) + ".txt";
            utilization = problem.export_results(chip_height, chip_width, x_i, y_i, z_i, w_i, h_i,
                                                {1}, output_file_name);
            utilizations.push_back(utilization);

            system(("python src/visualize.py -f " + output_file_name + " --glob False --sa " + printBool(successive_augmentation) + " -idx " + to_string(i) + " -show True").c_str()); // Call visualize.py
        }
        src_file_path = sa_files_dir + to_string(num_blocks) + "_sa.ilp";
        writeHard(src_file_path, num_augmentations);
        save_augmented_dimensions(src_file_path, final_dimensions);
    }
    else
    {
        utilizations = {1};
        src_file_path = spec_files_dir + to_string(num_blocks) + "_block.ilp";
    }

    // Optimize and plot final floorplan

    cout << "\nFinal Optimization\n";

    SolveILP problem = SolveILP(src_file_path, underestimation, save_lp);
    vector<float>x_i, y_i, z_i, w_i, h_i;
    float Y, chip_width, chip_height;
    tie(Y, x_i, y_i, z_i, w_i, h_i) = problem.solve(runtime, false);
    chip_width = problem.get_chip_dimension(x_i, problem.hard_module_width,
                                            problem.hard_module_height,
                                            w_i, z_i);
    chip_height = problem.get_chip_dimension(y_i, problem.hard_module_height,
                                            problem.hard_module_width,
                                            h_i, z_i);
    string output_file_name = result_dir + to_string(num_blocks) + "_sa_" + printBool(successive_augmentation) + ".txt";
    utilization = problem.export_results(chip_height, chip_width, x_i, y_i, z_i, w_i, h_i,
                                        utilizations, output_file_name);
    cout << "Utilization: " << 100 * utilization << '%' << endl;
    system(("python src/visualize.py -f " + output_file_name + " --glob True --sa " + printBool(successive_augmentation) + " -show True").c_str()); // Call visualize.py
        
    return 0;
}