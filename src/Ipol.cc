#include "Professor/Ipol.h"
#include <eigen3/Eigen/SVD>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>

using namespace std;
using namespace Eigen;


// NB. Not a member function
int numCoeffs(int dim, int order) {
    int ntok = 1;
    int r = min(order, dim);
    for (int i = 0; i < r; ++i) {
      ntok=ntok*(dim+order-i)/(i+1);
    }
  return ntok;
}


void Ipol::fromString(const string& s) {
  vector<string> tokens;
  // Check if a name is given in the string
  if (s.find(':') != std::string::npos) {
    vector<string> temp;
    boost::algorithm::split(temp, s, boost::is_any_of(":"), boost::token_compress_on);
    _name = temp[0];

    //Remove trailing and leading whitespaces
    boost::algorithm::trim(temp[1]);

    // Split string at whitespaces
    boost::algorithm::split(tokens, temp[1], boost::is_any_of("\t "), boost::token_compress_on);
  } else {
    boost::algorithm::split(tokens, s, boost::is_any_of("\t "), boost::token_compress_on);
    _name = "";
    // TODO also remove whitespace from string here?
  }
  _order = atoi(tokens[0].c_str());

  for (size_t i = 1; i < tokens.size(); ++i)
    _coeffs.push_back(atof(tokens[i].c_str()));
}


double Ipol::value(const vector<double>& params) const {
  const vector<double> lv = _getLongVector(params, order());
  assert(lv.size() == coeffs().size());
  double v = 0.0;
  for (size_t i = 0; i < lv.size(); ++i) {
    v += lv[i] * coeff(i);
  }
  return v;
}


void Ipol::_calcCoeffs() const {
  assert(_pts != NULL);
  cerr << _pts << ": " << _pts->size() << ", " <<  _values.size() << endl;
  assert(_pts->size() == _values.size());
  int ncoeff = numCoeffs(dim(), order());
  if (ncoeff > _pts->size()) {
    cout << "Error: not enough ("<< ncoeff <<" vs. " <<_pts->size()<< ") anchor points, aborting" <<endl;
    abort();
  }
  MatrixXd DP = MatrixXd(_pts->size(), ncoeff);
  VectorXd MC = VectorXd(_pts->size());

  vector<double> tempLV;
  vector<double> tempDP;
  // Populate the to be inversed matrix
  for (int a = 0; a < _pts->size(); ++a) {
    tempLV = _getLongVector(_pts->at(a), order());
    for (int i = 0; i < tempLV.size(); ++i) {
      DP(a, i) = tempLV[i];
    }
    // The vector of values (corresponding to anchors)
    MC[a] = _values[a];
  }
  VectorXd co = DP.jacobiSvd(ComputeThinU|ComputeThinV).solve(MC);
  vector<double> temp;
  for (int i = 0; i < ncoeff; ++i) {
    temp.push_back(co[i]);
  }
  // tuple<int, vector<double> > pb(order, temp); // TODO: do we want coeffs more multiple orders
  _coeffs = temp;
}


const vector<double>& Ipol::coeffs() const {
  if (_coeffs.empty()) {
    if (_pts == NULL) throw IpolError("No parameter points available when calculating ipol coeffs");
    _calcCoeffs(); // TODO get this to work, something about constness being lost
    _pts = NULL; //< Not necessary, but ensures consistency
  }
  return _coeffs;
}



vector<double> Ipol::_getLongVector(const vector<double>& p, int order) const {
  if (order < 1 || order > 6) {
    std::cout << "ERROR degree " << order << " not implemented, exiting" << std::endl;
    /// @todo Never call exit() from a library function. Throw an IpolError instead
    // exit(1);
  }
  if (order == 1) return _getLongVector1D(p);
  if (order == 2) return _getLongVector2D(p);
  if (order == 3) return _getLongVector3D(p);
  if (order == 4) return _getLongVector4D(p);
  if (order == 5) return _getLongVector5D(p);
  if (order == 6) return _getLongVector6D(p);
}

vector<double> Ipol::_getLongVector1D(const vector<double>& p) const {
  int nop = p.size();
  vector<double> retvec;
  retvec.push_back(1.0);    // This is the offset, for alpha
  for (int i = 0; i < nop; ++i) { // Linear coefficients, for beta
    retvec.push_back(p[i]);
  }

  assert(retvec.size() == numCoeffs(nop,1));
  return retvec;
}

vector<double> Ipol::_getLongVector2D(const vector<double>& p) const {
  int nop = p.size();
  vector<double> retvec;
  retvec.push_back(1.0);    // This is the offset, for alpha
  for (int i = 0; i < nop; i++) { // Linear coefficients, for beta
    retvec.push_back(p[i]);
  }
  for (int i = 0; i < nop; i++) {
    for (int j = 0; j < nop; j++) {
      if (i <= j) {
        retvec.push_back(p[i]*p[j]);
      }
    }
  }

  assert(retvec.size() == numCoeffs(nop,2));
  return retvec;
}

vector<double> Ipol::_getLongVector3D(const vector<double>& p) const {
  /// @todo Build the first terms in _getLongVector2D

  int nop = p.size();
  vector<double> retvec;
  retvec.push_back(1.0);    // This is the offset, for alpha
  for (int i = 0; i < nop; i++) { // Linear coefficients, for beta
    retvec.push_back(p[i]);
  }
  for (int i = 0; i < nop; i++) {
    for (int j = 0; j < nop; j++) {
      if (i <= j) {
        retvec.push_back(p[i]*p[j]);
      }
    }
  }
  for (int i=0;i<nop;i++) {
    for (int j=0;j<nop;j++) {
      for (int k=0;k<nop;k++) {
        if (i<=j && i<=k && j<=k) {
          retvec.push_back(p[i]*p[j]*p[k]);
        }
      }
    }
  }

  assert(retvec.size() == numCoeffs(nop,3));
  return retvec;
}


vector<double> Ipol::_getLongVector4D(const vector<double>& p) const {
  /// @todo Build the first terms in _getLongVector3D

  int nop = p.size();
  vector<double> retvec;
  retvec.push_back(1.0);    // This is the offset, for alpha
  for (int i=0;i<nop;i++) { // Linear coefficients, for beta
    retvec.push_back(p[i]);
  }
  for (int i=0;i<nop;i++) {
    for (int j=0;j<nop;j++) {
      if (i<=j) {
        retvec.push_back(p[i]*p[j]);
      }
    }
  }
  for (int i=0;i<nop;i++) {
    for (int j=0;j<nop;j++) {
      for (int k=0;k<nop;k++) {
        if (i<=j && i<=k && j<=k) {
          retvec.push_back(p[i]*p[j]*p[k]);
        }
      }
    }
  }
  for (int i=0;i<nop;i++) {
    for (int j=0;j<nop;j++) {
      for (int k=0;k<nop;k++) {
        for (int l=0;l<nop;l++) {
          if (i<=j && i<=k && i<=l &&
                      j<=k && j<=l &&
                              k<=l) {
            retvec.push_back(p[i]*p[j]*p[k]*p[l]);
          }
        }
      }
    }
  }

  assert(retvec.size() == numCoeffs(nop,4));
  return retvec;
}


vector<double> Ipol::_getLongVector5D(const vector<double>& p) const {
  /// @todo Build the first terms in _getLongVector4D

  int nop = p.size();
  vector<double> retvec;
  retvec.push_back(1.0);    // This is the offset, for alpha
  for (int i=0;i<nop;i++) { // Linear coefficients, for beta
    retvec.push_back(p[i]);
  }
  for (int i=0;i<nop;i++) {
    for (int j=0;j<nop;j++) {
      if (i<=j) {
        retvec.push_back(p[i]*p[j]);
      }
    }
  }
  for (int i=0;i<nop;i++) {
    for (int j=0;j<nop;j++) {
      for (int k=0;k<nop;k++) {
        if (i<=j && i<=k && j<=k) {
          retvec.push_back(p[i]*p[j]*p[k]);
        }
      }
    }
  }
  for (int i=0;i<nop;i++) {
    for (int j=0;j<nop;j++) {
      for (int k=0;k<nop;k++) {
        for (int l=0;l<nop;l++) {
          if (i<=j && i<=k && i<=l && j<=k && j<=l && k<=l) {
            retvec.push_back(p[i]*p[j]*p[k]*p[l]);
          }
        }
      }
    }
  }
  for (int i=0;i<nop;i++) {
    for (int j=0;j<nop;j++) {
      for (int k=0;k<nop;k++) {
        for (int l=0;l<nop;l++) {
          for (int m=0;m<nop;m++) {
            if (
                i<=j && i<=k && i<=l && i<=m &&
                        j<=k && j<=l && j<=m &&
                                k<=l && k<=m &&
                                        l<=m
               ) {
              retvec.push_back(p[i]*p[j]*p[k]*p[l]*p[m]);
            }
          }
        }
      }
    }
  }

  assert(retvec.size() == numCoeffs(nop,5));
  return retvec;
}


vector<double> Ipol::_getLongVector6D(const vector<double>& p) const {
  /// @todo Build the first terms in _getLongVector5D

  int nop = p.size();
  vector<double> retvec;
  retvec.push_back(1.0);    // This is the offset, for alpha
  for (int i=0;i<nop;i++) { // Linear coefficients, for beta
    retvec.push_back(p[i]);
  }
  for (int i=0;i<nop;i++) {
    for (int j=0;j<nop;j++) {
      if (i<=j) {
        retvec.push_back(p[i]*p[j]);
      }
    }
  }
  for (int i=0;i<nop;i++) {
    for (int j=0;j<nop;j++) {
      for (int k=0;k<nop;k++) {
        if (i<=j && i<=k && j<=k) {
          retvec.push_back(p[i]*p[j]*p[k]);
        }
      }
    }
  }
  for (int i=0;i<nop;i++) {
    for (int j=0;j<nop;j++) {
      for (int k=0;k<nop;k++) {
        for (int l=0;l<nop;l++) {
          if (i<=j && i<=k && i<=l && j<=k && j<=l && k<=l) {
            retvec.push_back(p[i]*p[j]*p[k]*p[l]);
          }
        }
      }
    }
  }
  for (int i=0;i<nop;i++) {
    for (int j=0;j<nop;j++) {
      for (int k=0;k<nop;k++) {
        for (int l=0;l<nop;l++) {
          for (int m=0;m<nop;m++) {
            if (
                i<=j && i<=k && i<=l && i<=m &&
                        j<=k && j<=l && j<=m &&
                                k<=l && k<=m &&
                                        l<=m
               ) {
              retvec.push_back(p[i]*p[j]*p[k]*p[l]*p[m]);
            }
          }
        }
      }
    }
  }
  for (int i=0;i<nop;i++) {
    for (int j=0;j<nop;j++) {
      for (int k=0;k<nop;k++) {
        for (int l=0;l<nop;l++) {
          for (int m=0;m<nop;m++) {
            for (int n=0;n<nop;n++) {
              if (
                  i<=j && i<=k && i<=l && i<=m && i<=n &&
                          j<=k && j<=l && j<=m && j<=n &&
                                  k<=l && k<=m && k<=n &&
                                          l<=m && l<=n &&
                                                  m<=n
                 ) {
                retvec.push_back(p[i]*p[j]*p[k]*p[l]*p[m]*p[n]);
              }
            }
          }
        }
      }
    }
  }

  assert(retvec.size() == numCoeffs(nop,6));
  return retvec;
}
