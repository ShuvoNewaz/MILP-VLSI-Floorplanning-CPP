#!bin/sh
clear
# Change the following as needed
USER=shuvo
MOSEK_HOME="home/${USER}/mosek"
MOSEK_VERSION="10.2"
num_blocks=100
underestimation=true
successive_augmentation=true
runtime=5
visualize=true
sub_block_size=10
save_lp=true
g++ -std=c++20 main.cpp -o main.out "-I/${MOSEK_HOME}/${MOSEK_VERSION}/tools/platform/linux64x86/h/" "-L/${MOSEK_HOME}/${MOSEK_VERSION}/tools/platform/linux64x86/bin/" -Wl,-rpath-link,"/${MOSEK_HOME}/${MOSEK_VERSION}/tools/platform/linux64x86/bin/" -Wl,"-rpath=/${MOSEK_HOME}/${MOSEK_VERSION}/tools/platform/linux64x86/bin/"   -lmosek64 -lfusion64
./main.out ${num_blocks} ${underestimation} ${successive_augmentation} ${runtime} ${visualize} ${sub_block_size} ${save_lp}