#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

// program options
#include <boost/program_options.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/tokenizer.hpp> 
#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <Eigen/Dense>

#include "gicp.h"

using namespace std;
using namespace dgc::gicp;
using namespace Eigen;
namespace po = boost::program_options;
namespace py = pybind11;


static string filename1;
static string filename2;
static string filename_t_base;
static bool debug = false;
static double gicp_epsilon = 1e-3;
static double max_distance = 1.;

bool load_points_from_mat(GICPPointSet *set, const MatrixXd &points){
  // suppose points is [N, 3]
  bool error = false;
  
  string line;
  GICPPoint pt;
  pt.range = -1;
  for(int k = 0; k < 3; k++) {
    for(int l = 0; l < 3; l++) {
      pt.C[k][l] = (k == l)?1:0;
    }
  } 

  for(int i=0; i<points.rows(); i++){
    pt.x = points(i, 0);
    pt.y = points(i, 1);
    pt.z = points(i, 2);
    set->AppendPoint(pt);
  }


  return false;  
}

MatrixXd gicp_mat(const MatrixXd &src, const MatrixXd &dst, double max_distance, double gicp_epsilon){
  bool error;
  GICPPointSet p1, p2;
  dgc_transform_t t_base, t0, t1;

  // set up the transformations
  dgc_transform_identity(t_base);
  dgc_transform_identity(t0);
  dgc_transform_identity(t1);

  error = load_points_from_mat(&p1, src);
  error = load_points_from_mat(&p2, dst);

  if(debug) {
    // save data for debug/visualizations
    cout << "Loaded " << p1.Size() << " points into GICPPointSet 1." << endl;
    cout << "Loaded " << p2.Size() << " points into GICPPointSet 2." << endl;
    p1.SavePoints("pts1.dat");
    p1.SaveMatrices("mats1.dat");
    p2.SavePoints("pts2.dat");
    p2.SaveMatrices("mats2.dat");
  }

  p1.SetGICPEpsilon(gicp_epsilon);
  p2.SetGICPEpsilon(gicp_epsilon);  

  p1.BuildKDTree();
  p1.ComputeMatrices();

  p2.BuildKDTree();
  p2.ComputeMatrices();

  // align the point clouds
  dgc_transform_copy(t1, t0);
  p2.SetDebug(debug);
  p2.SetMaxIterationInner(8);
  p2.SetMaxIteration(100);
  int iterations = p2.AlignScan(&p1, t_base, t1, max_distance);
  
  MatrixXd tfm(4,4);
  for(int i=0; i<4; i++){
    for(int j=0; j<4; j++){
      tfm(i,j) = t1[i][j];
    }
  }

  if(debug) {
    dgc_transform_print(t_base, "t_base");
    dgc_transform_print(t0, "t0");  
    dgc_transform_print(t1, "t1");  
    ofstream fout("iterations.txt");
    fout << "Converged in " << iterations << " iterations." << endl;
    fout.close();
  }

  return tfm;
}



PYBIND11_MODULE(warp_gicp, m) {
    m.doc() = "output(4,4) * source(4, N) = target(4, N)"; // optional module docstring

    m.def("gicp", &gicp_mat, pybind11::arg("source"), pybind11::arg("target"), pybind11::arg("max_distance") = 1., pybind11::arg("gicp_epsilon") = 1e-3, "output * source = target");
}