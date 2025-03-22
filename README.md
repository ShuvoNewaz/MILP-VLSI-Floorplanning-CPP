# Optimization of VLSI chip area using Mixed Integer Linear Programming

NOTE: This repository uses the MOSEK library in C++ to solve the problem. Click [here](https://github.com/ShuvoNewaz/MILP-VLSI-Floorplanning-Python/) to view the repository in Python. Aside from the major programming language used, the 2 repositories are identical for all intents and purposes.

This repository is for the Linux operating system and is based on C++11. The visualization uses the [matplotlib](https://matplotlib.org/) library with Python. To use this repository, please follow these steps:

- Clone this repository or download as a zip.
- This project makes use of an LP-solver named MOSEK. The tool can be downloaded and the license can be obtained from [MOSEK's website](https://www.mosek.com/resources/getting-started/). Once registered, follow the instructions regarding the directory setup for MOSEK in the email. In particular, pay attention to the directory where the license file is kept.
- The setup for the MOSEK libraries and header files are dependent on the operating system. For instance, g++ may not be used with Windows to run MOSEK. Please check their website to confirm compatibility. The [run_template.sh](run_template.sh) outlines the template paths for the required header files and libraries.
- The input arguments such as the number of blocks of the system, whether or not successive augmentation is applied, etc. are very similar to the Python version. Edit the [run_template.sh](run_template.sh) file to run as required.
- After setting up the arguments as needed, run `bash run_template.sh` in your terminal.

## Assumptions

This model assumes that the optimized floorplan has a square shape. Because of this, the effective utilization is lower in certain cases. Try running the [10-block system](spec_files/10_block.ilp) with a sub-block size of 6 with successive augmentation to illustrate this effect in the final floorplan. The constraints would have to be modified to mitigate this, which is beyond the scope of this work.

## Overview

### Objective

The objective is to optimize the area of a chip, given,

- The dimension of every module.

- The state of module.

         Hard - width and height are fixed.

         Soft - width and height are flexible, given some aspect ratio.

The hard modules can be rotated for area optimization. There can be no over lap between modules.

### Input

The input is a `*.ilp` file that contains

- The state of the module.

- The dimensions of the module.

- The allowed aspect ratio of the soft modules.

### Task

The task is to parse the `*.ilp` file and read the module parameters. The parameters are then used to formulate and solve an MILP problem. Assuming the final floorplan is square-shaped, the MILP problem solves to minimize the final width, $W$, of the chip.

An auxiliary task is to export to a `*.lp` file that containing all the contraints of the problem. The `*.lp` file can be parsed and solved by an off-the-shelf tool such as [LPSolve](https://sourceforge.net/projects/lpsolve/).

## Problem Formulation

**Acknowledgement**: The models are created on the basis of the work by [Sutanthavibul et al](https://dl.acm.org/doi/abs/10.1145/123186.123255). For a detailed walkthrough of how the prolem is formulated, please refer to their original paper.

### Setup

Let $x$ be the $x$-coordinate, $y$ be the $y$-coordinate, $w$ be the width, $h$ be the height of a module, $m$ be the gradient of a soft module and $c$ be the intercept of a soft module (computed from the aspect ratio). Additionally, let $W$ be the width of the final chip, $H$ be the height of the final chip, and $z$ be a binary variable that determines if a module has been rotated. Let $M = \max(W,H)$ The constraint equations are as follows.

### Hard-hard Non-overlap Constraints

$$x_i + z_i h_i + (1 - z_i) w_i \le x_j + M (x_{ij} + y_{ij})$$
$$x_i - z_i h_j - (1 - z_j) w_j \ge x_j - M (1 - x_{ij} + y_{ij})$$
$$y_i + z_i w_i + (1 - z_i) h_i \le y_j + M (1 + x_{ij} - y_{ij})$$
$$y_i - z_i w_j - (1 - z_j) h_j \ge y_j - M (2 - x_{ij} - y_{ij})$$

### Hard-soft Non-overlap Constraints

These constraints keep the hard module $i$ from overlapping with soft module $j$.

$$x_i + z_i h_i + (1 - z_i) w_i \le x_j + M (x_{ij} + y_{ij})$$
$$x_i - w_j \ge x_j - M (1 - x_{ij} + y_{ij})$$
$$y_i + z_i w_i + (1 - z_i) h_i \le y_j + M(1 + x_{ij} + y_{ij})$$
$$y_i - m_j w_j + c_j \ge y_j - M (2 - x_{ij} - y_{ij})$$

### Soft-soft Non-overlap Constraints

These constraints keep the soft modules from overlapping with each other.

$$x_i + w_i \le x_j + M (x_{ij} + y_{ij})$$
$$x_i - w_j \ge x_j - M (1 - x_{ij} + y_{ij})$$
$$y_i + m_i w_i + c_i \le y_j + M (1 - x_{ij} - y_{ij})$$
$$y_i - m_j w_j + c_j \ge y_j - M (2 - x_{ij} - y_{ij})$$

### Integer Constraints

$z$ determines hard module rotation and $x_{ij}, y_{ij}$ determines if modules $i$ and $j$ are physically connected.

$$z_i=[0,1]$$
$$x_{ij}=[0,1]$$
$$y_{ij}=[0,1]$$

### Aspect Ratio Constraints

$$w_{\mathrm{min}} \le w_i$$
$$w_{\mathrm{max}} \ge w_i$$

### Boundary Constraints

$$x_i, y_i \ge 0$$
$$x_i, y_i \le W$$

#### Hard Modules

$$x_i + z_i h_i + (1 - z_i) w_i \le M$$
$$y_i + z_i w_i + (1 - z_i) h_i \le M$$

#### Soft Modules

$$x_i + w_i \le M$$
$$y_i + m_i w_i + c_i \le M$$

## Results

The optimized floorplan of a 5-module system is shown below. This particular solution is optimal, however, most larger problems will have a sub-optimal solution because of early termination. Green: hard module, red: rotated hard module, yellow: soft module.

<p align="center">
  <img src="results/5_block_floorplan.png"/>
</p>