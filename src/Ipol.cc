#include "Professor/Ipol.h"
#include "Professor/Counter.h"
#include "Eigen/SVD"
#include <sstream>
#include <cassert>
#include <cmath>


namespace Professor {

  using namespace std;
  using namespace Eigen;


  // NB. Not a member function
  int numCoeffs(int dim, int order) {
    int ntok = 1;
    int r = min(order, dim);
    for (int i = 0; i < r; ++i) {
      ntok = ntok*(dim+order-i)/(i+1);
    }
    return ntok;
  }


  // NB. Not a member function
  std::vector<double> calcCoeffs(const ParamPoints& pts, const vector<double>& vals, int order, double threshold) {

    // Early exit if this is a trivial 0th order polynomial
    vector<double> rtn;
    if (order == 0) {
      rtn.push_back(vals[0]);
      return rtn;
    }

    // Check the inputs
    if (pts.numPoints() != vals.size())
      throw IpolError("pts.numPoints() != vals.size() in calcCoeffs");
    const int ncoeff = numCoeffs(pts.dim(), order);
    if (ncoeff > pts.numPoints()) {
      stringstream ss;
      ss << "Ipol: not enough (" << ncoeff << " vs. " << pts.numPoints() << ") anchor points "
         << "for interpolating with " << pts.dim() << " params at order " << order;
      throw IpolError(ss.str());
    }

    // Create Eigen objects for the SVD solving
    MatrixXd DP = MatrixXd(pts.numPoints(), ncoeff);
    VectorXd MC = VectorXd(pts.numPoints());

    // Populate the matrix to be inverted
    vector<double> tempLV;
    for (int a = 0; a < pts.numPoints(); ++a) {
      tempLV = mkLongVector(pts.point(a), order);
      for (size_t i = 0; i < tempLV.size(); ++i) {
        DP(a, i) = tempLV[i];
      }
      // The vector of values (corresponding to anchors)
      MC[a] = vals[a];
    }
    JacobiSVD<MatrixXd> svd = DP.jacobiSvd(ComputeThinU|ComputeThinV);
    #if EIGEN_WORLD_VERSION >= 3 && EIGEN_MAJOR_VERSION >= 2 && EIGEN_MINOR_VERSION >= 1
    svd.setThreshold(threshold); // Needed TODO find transform for dependence on stuff
    #endif

    // Check for singular values, i.e. fully correlated parameters
    /// @todo Maybe figure out how to use Eigen's setThreshold better?
    VectorXd svals = svd.singularValues();
    for (unsigned int i = 0; i < svd.nonzeroSingularValues();++i) {
      if (fabs(svals[i]) < threshold) {
        std::cout << "Singular value encountered, aborting" << std::endl;
        abort();
      }
    }

    // Solve for coefficients
    VectorXd co = svd.solve(MC);

    // Populate the coefficient std::vector and return
    for (size_t i = 0; i < ncoeff; ++i) rtn.push_back(co[i]);
    return rtn;
  }


  // NB. Not a member function
  vector<double> mkLongVector(const vector<double>& p, int order) {
    if (order < 0) {
      std::cout << "ERROR degree " << order << " not implemented, exiting" << std::endl;
      /// @todo Never call exit() from a library function. Throw an IpolError instead
      // exit(1);
    }

  
    int N=p.size(); 
    vector<int> zero; 
    for (unsigned int i;i<N;++i) zero.push_back(0);
    vector<vector<int> > temp;
    temp.push_back(zero);
    
    for (unsigned int i=0;i<=order;++i) {
      Professor::Counter c(N,i);
      while (c.next(N-1)) {
        if (c.sum() == i) {
          temp.push_back(c.data());
        }
      }
    }

    vector<double> retvec;
    double prod(1.0);

    for (vector<int> v : temp) {
      prod=1.0;
      for (unsigned int i=0;i<v.size();++i) {
        prod*=std::pow(p[i],v[i]);
      }
      retvec.push_back(prod);
    }
    return retvec;
  }





  ///////////////////////////////////////////////////////



  string Ipol::toString(const string& name) const {
    stringstream ss;
    if (!name.empty()) ss << name << ": ";
    else if (!_name.empty()) ss << _name << ": ";
    ss << this->dim() << " ";
    ss << this->order() << " ";
    for (const double& a : coeffs())
      ss << a << " ";
    return ss.str();
  }


  void Ipol::fromString(const string& s) {
    // Extract a name if given at the start of the string
    _name = (s.find(":") != std::string::npos) ? s.substr(0, s.find(":")) : "";
    // Load the rest of the string into a stringstream and load into numerical variables
    istringstream numss( (s.find(":") != std::string::npos) ? s.substr(s.find(":")+1) : s );
    numss >> _dim;
    numss >> _order;
    double tmp; int ncoeffs = 0;
    while (numss >> tmp) {
      _coeffs.push_back(tmp);
      ncoeffs += 1;
    }
    assert(ncoeffs == numCoeffs(dim(),order()));
  }


  double Ipol::value(const vector<double>& params) const {
    if (params.size() != dim()) {
      stringstream ss;
      ss << "Incorrect number of parameters passed to Ipol::value ("
         << dim() << " params required, " << params.size() << " supplied)";
      throw IpolError(ss.str());
    }
    const vector<double> lv = mkLongVector(params, order());
    assert(lv.size() == coeffs().size());
    double v = 0.0;
    for (size_t i = 0; i < lv.size(); ++i) {
      v += lv[i] * coeff(i);
    }
    return v;
  }


}
