# gicp-python

The implement is totally from https://github.com/avsegal/gicp. 

I warp c++ code to a python. Suppose I am in a conda environment, env in Makefile.

## Installation

1. install pybind11, gsl, pkg-config, boost
    ```
    conda install pybind11
    conda install pkg-config
    conda install libboost
    conda install gsl
    ```

2. compile ANN

    Modify the Makefile, change  `CONDA_PREFIX`  in line 1 to your conda prefix path.

    ```bash
    CONDA_PREFIX = /GPFS/rhome/yifanlu/miniconda3/envs/disco # change to your envs
    INC = $(shell ${CONDA_PREFIX}/bin/python3 -m pybind11 --includes)
    ...
    ```

    Then run the command

    ```
    cd ann_1.1.2
    make linux-g++
    ```

3. compile the project

    after you specify the conda environment in Makefile, go to the root dir and call make command.
    ```
    cd ..
    make
    ```
    then it should generate warp_gicp.so


## Usage

```
import warp_gicp
T = warp_gicp.gicp(source, target, max_distance, gicp_epsilon)
'''
Args:
    source: np.ndarray (N1,3)
        source point cloud

    target: np.ndarray (N2,4)
        target point cloud

    max_distance: float, default = 1.0
        filter corresponding pairs over max_distance
    
    gicp_epsilon: float, default = 1e-3
         epsilon constant for gicp paper; this is NOT the convergence tolerence

Returns:
    T: np.ndarray (4, 4)
        T @ source_homo = target_homo, *_homo is homogeneous coordinates with shape (4, N)

```

There are many optional parameters in gicp.cpp, like max_iteration, epsilon , but I just use it's default setting.

You can run `python test_with_python.py` to check if it's nicely compiled.