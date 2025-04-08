#include<iostream>
#include <fstream>
#include <vector>
#include <math.h>
// #include "generate/total_modules.h"
// #include "generate/hard_module_dimension.h"
// #include "generate/soft_module_properties.h"

using namespace std;

int minimum(int a, int b)
{
    return a * (a < b) + b * (b <= a);
}

class Augment
{
    public:        
        unsigned short int num_hard_modules, num_soft_modules, num_total_modules;
        vector<float> hard_module_width, hard_module_height, area, min_aspect, max_aspect;
        string file, spec_file;
        string spec_files_dir, sa_files_dir, sa_file_prefix;
        int num_blocks;
        vector<string> lines;

        Augment(string);

        void break_problem(int sub_block_size)
        {
            if(num_total_modules > sub_block_size)
            {
                unsigned short int soft_count {0}, i, j, k, num_subblocks, modules_in_subblock, soft_left;
                short int hard_left;
                num_subblocks = int(ceil(float(num_blocks) / float(sub_block_size)));
                soft_count = 0;
                vector<float> area_clipped, min_aspect_clipped, max_aspect_clipped;
                ofstream g;

                for(i=0; i<num_subblocks; i++)
                {
                    modules_in_subblock = minimum(sub_block_size, num_total_modules - i * sub_block_size);
                    hard_left = num_hard_modules - sub_block_size*i;
                    if(hard_left >= modules_in_subblock)
                    {
                        g.open(sa_file_prefix + "/" + to_string(num_blocks) + "_" + to_string(i+1) + ".ilp", ios::out);
                        g << "hard - " + to_string(modules_in_subblock) + "\n";
                        g.close();
                        g.open(sa_file_prefix + "/" + to_string(num_blocks) + "_" + to_string(i+1) + ".ilp", ios::app);
                        
                        for(j=0; j<modules_in_subblock; j++)
                        {
                            g << to_string(hard_module_width[i*modules_in_subblock+j]) + "," + to_string(hard_module_height[i*modules_in_subblock+j]) + "\n";
                        }
                        g.close();
                    }
                    else if(modules_in_subblock > hard_left && hard_left > 0)
                    {
                        g.open(sa_file_prefix + "/" + to_string(num_blocks) + "_" + to_string(i+1) + ".ilp", ios::out);
                        g << "hard - " + to_string(hard_left) + "\n";
                        g.close();
                        g.open(sa_file_prefix + "/" + to_string(num_blocks) + "_" + to_string(i+1) + ".ilp", ios::app);
                        for(j=0; j<hard_left; j++)
                        {
                            g << to_string(hard_module_width[i*modules_in_subblock+j]) + "," + to_string(hard_module_height[i*modules_in_subblock+j]) + "\n";
                        }
                        if(num_soft_modules > 0)
                        {
                            g << "\nsoft - " + to_string(modules_in_subblock - hard_left) + "\n";
                            for(j=0; j<modules_in_subblock - hard_left; j++)
                            {
                                g << to_string(area[j]) + "," + to_string(min_aspect[j]) + ","  + to_string(max_aspect[j])  + "\n";
                            }
                            g.close();
                            for(k=j; k<num_soft_modules; k++)
                            {
                                area_clipped.push_back(area[k]);
                                min_aspect_clipped.push_back(min_aspect[k]);
                                max_aspect_clipped.push_back(max_aspect[k]);
                            }
                            area = area_clipped;
                            min_aspect = min_aspect_clipped;
                            max_aspect = max_aspect_clipped;
                        }
                        soft_left = num_soft_modules - (modules_in_subblock - hard_left);
                    }
                    else if(hard_left <= 0)
                    {
                        soft_left = num_soft_modules - modules_in_subblock * soft_count;
                        g.open(sa_file_prefix + "/" + to_string(num_blocks) + "_" + to_string(i+1) + ".ilp", ios::out);
                        g << "soft - " + to_string(modules_in_subblock) + "\n";
                        g.close();
                        g.open(sa_file_prefix + "/" + to_string(num_blocks) + "_" + to_string(i+1) + ".ilp", ios::app);
                        if(soft_left > 0)
                        {
                            for(j=0; j<modules_in_subblock; j++)
                            {
                                g << to_string(area[soft_count*modules_in_subblock+j]) + "," + to_string(min_aspect[soft_count*modules_in_subblock+j]) + "," + to_string(max_aspect[soft_count*modules_in_subblock+j]) + "\n";
                            }
                            g.close();
                            soft_count ++;
                        }
                    }
                }
            }

        }
};

Augment::Augment(string fname)
{
    file = fname;
    num_blocks = stoi(file.substr(0, file.find("_")));
    spec_files_dir = "spec_files";
    sa_files_dir = spec_files_dir + "/successive_augmentation";
    sa_file_prefix = sa_files_dir + "/" + to_string(num_blocks);
    spec_file = spec_files_dir + "/" + file;

    ifstream f; // Read file contents into f
    f.open(spec_file);
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
    tie(hard_module_width, hard_module_height) = hard_module_dimension(lines, num_hard_modules);
    tie(area, min_aspect, max_aspect) = soft_module_properties(lines, num_soft_modules);
}