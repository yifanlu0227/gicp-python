CONDA_PREFIX = /GPFS/rhome/yifanlu/miniconda3/envs/disco
INC = $(shell ${CONDA_PREFIX}/bin/python3 -m pybind11 --includes)

CXX = g++
CXXFLAGS = -O3 -Wall -std=c++11 -fPIC ${INC} 
# CXXFLAGS += `pkg-config --cflags gsl` -I./ann_1.1.2/include/ANN 
CXXFLAGS += -I$CONDA_PREFIX/include -I./ann_1.1.2/include/ANN -I$CONDA_PREFIX/include/eigen3

# LDFLAGS = `pkg-config --libs gsl`
LDFLAGS = -L$CONDA_PREFIX/lib -lgsl -lgslcblas -lm 
LDFLAGS += -L. -lgicp -Lann_1.1.2/lib -lANN -lboost_program_options -lboost_system -lstdc++
LDFLAGS += -Wl,-rpath="$CONDA_PREFIX/lib"

objects = warp_gicp.o gicp.o optimize.o bfgs_funcs.o transform.o scan.o

all:  libgicp.a warp_gicp.so test_gicp

libgicp.a: gicp.o optimize.o bfgs_funcs.o transform.o scan.o
	ar rvs $@ $^

warp_gicp.so: $(objects)
	${CXX} ${CXXFLAGS} -shared -o $@${SUF} $^ ${LDFLAGS}

test_gicp: test_gicp.o gicp.o optimize.o bfgs_funcs.o transform.o
	$(CXX) -o $@ $^ $(LDFLAGS)

.cpp.o: 
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o *.so *.a *~ t_*.tfm *.dat mahalanobis.txt correspondence.txt iterations.txt test_gicp