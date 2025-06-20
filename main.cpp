#include "src/solve/build_pipeline.h"
#include "src/multi_threading.h"


int main(int argc, char *argv[])
{
    int num_blocks{stoi(argv[1])}, sub_block_size{stoi(argv[6])}, num_augmentations, i;
    bool underestimation{parseBool(argv[2])}, successive_augmentation{parseBool(argv[3])},
    visualize_superblock{parseBool(argv[5])}, save_lp{parseBool(argv[7])};
    float runtime{stof(argv[4])};
    string src_file_path;
    string spec_files_dir = "spec_files/";
    string result_dir = "results/" + to_string(num_blocks) + "/";
    string sa_files_dir = spec_files_dir + "successive_augmentation/" + to_string(num_blocks) + "/";
    string file = to_string(num_blocks) + "_block.ilp";
    system(("rm -Rf " + result_dir).c_str());
    system(("mkdir -p " + result_dir).c_str());

    vector<float> utilizations;
    float utilization, Y;
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
        
        // Begin multi-threading
        ThreadPool pool(std::thread::hardware_concurrency());  // Use all cores

        std::vector<std::future<tuple<float, float>>> futures;
        
        int remaining_blocks = num_blocks;
        for (int i = 1; i <= num_augmentations; i++) {
            sub_block_size = min(sub_block_size, remaining_blocks);
            remaining_blocks -= sub_block_size;
            utilizations = {};
            for(int j = 0; j < sub_block_size; j++)
            {
                utilizations.push_back(1.0f); // Primary blocks are fully used
            }
            futures.emplace_back(
                pool.submit(mainProcess,
                            std::ref(num_blocks),
                            std::ref(underestimation),
                            false,
                            std::ref(successive_augmentation),
                            std::ref(runtime),
                            std::ref(result_dir),
                            utilizations, false, i)
            );
        }
        
        utilizations = {}; // Reset for final layout
        for (auto& fut : futures)
        {
            std::tie(utilization, Y) = fut.get(); // blocks if not ready
            utilizations.push_back(utilization); // Superblocks are under-utilized
            final_dimensions.push_back(Y);
        }
        // End multi-threading

        src_file_path = sa_files_dir + to_string(num_blocks) + "_sa.ilp";
        writeHard(src_file_path, num_augmentations);
        save_augmented_dimensions(src_file_path, final_dimensions);
    }
    else
    {
        for(int j = 0; j < num_blocks; j++)
        {
            utilizations.push_back(1.0f); // Primary blocks are fully used
        }
    }

    // Optimize and plot final floorplan

    tie(utilization, Y) = mainProcess(num_blocks,
                                    underestimation, save_lp,
                                    successive_augmentation,
                                    runtime, result_dir,
                                    utilizations, true);
    cout << "Final utilization: " << 100 * utilization << '%' << endl;
        
    return 0;
}