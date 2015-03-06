#include "ProfDriver.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdlib.h>
#include <boost/foreach.hpp>
#include<boost/algorithm/string/split.hpp>                                      
#include<boost/algorithm/string.hpp>
#include <iostream>
#include <eigen3/Eigen/SVD>
#include <algorithm> 

using namespace Eigen;
using namespace std;
using boost::algorithm::split;
using boost::is_any_of;                                                     

ProfDriver::ProfDriver():
  m_dim(0),
  m_order(-1),
  m_center(0),
  m_params(0),
  m_anchors(0)
{
  m_version="0.1.1";
  m_message="Professor CPP-driver version " + m_version + " \nPlease cite arXiv:0907.2973 [hep-ph]\n2015 Andy Buckley, Holger Schulz, Simone Amoroso\n";
  //cout << m_message <<endl;
}
ProfDriver::ProfDriver(int order):
  m_dim(0),
  m_order(order),
  m_center(0),
  m_params(0),
  m_anchors(0)
{
  m_version="0.1.2";
  m_message="Professor CPP-driver version " + m_version + " \nPlease cite arXiv:0907.2973 [hep-ph]\n2015 Andy Buckley, Holger Schulz, Simone Amoroso\n";
  //cout << m_message <<endl;
}

ProfDriver::ProfDriver(string const& filename):
  m_dim(0),
  m_order(-1),
  m_center(0),
  m_params(0),
  m_anchors(0)
{
  m_version="0.1.2";
  m_message="Professor CPP-driver version " + m_version + " \nPlease cite arXiv:0907.2973 [hep-ph]\n2015 Andy Buckley, Holger Schulz, Simone Amoroso\n";
  //cout << m_message <<endl;
    readProfDF(filename.c_str());
}

ProfDriver::~ProfDriver() {
}


void ProfDriver::calc(string binid, int order) {
  assert(m_anchors.size() == m_values[binid].size());
  if (m_center.size() <1) {
    this->setCenter(this->calcCenter()); //TODO add debug message?
  }
  if (m_min.size() <1) {
    this->m_min = this->calcMin();
  }
  if (m_max.size() <1) {
    this->m_max = this->calcMax();
  }
  int ncoeff = numOfCoefficients(m_center.size(), order);
  if (ncoeff > m_anchors.size()) {
    cout << "Error: not enough ("<< ncoeff <<" vs. " <<m_anchors.size()<< ") anchor points, aborting" <<endl;
    abort();
  }

  MatrixXd DP = MatrixXd(m_anchors.size(), ncoeff);
  VectorXd MC = VectorXd(m_anchors.size());

  vector<double> tempLV;
  vector<double> tempDP;
  // Populate the to be inversed matrix
  for (int a=0;a<m_anchors.size();a++) {
    tempLV = getLongVector(getDP(m_anchors[a], m_center), order);
    for (int i=0;i<tempLV.size();i++) {
      DP(a, i) = tempLV[i];
    }
    // The vector of values (corresponding to anchors)
    MC[a] = m_values[binid][a];
  }
  VectorXd co = DP.jacobiSvd(ComputeThinU|ComputeThinV).solve(MC);
  vector<double> temp;
  for (int i=0;i<ncoeff;i++) {
    temp.push_back(co[i]);
  }
  tuple<int, vector<double> > pb(order, temp);
  m_coeffs[binid] = pb;
}

void ProfDriver::calc(string binid) {
  if (m_order>0) {
    calc(binid, m_order);
  }
}

void ProfDriver::addAnchor(vector<double> anchor) {
  m_anchors.push_back(anchor);
  if (m_dim ==0) m_dim=anchor.size(); // only set dimension (of param space) once
}

vector<double> ProfDriver::calcCenter() {
  vector<double> temp_max, temp_min;
  for (unsigned int i=0;i<m_anchors[0].size();i++) { // iteration over coordinates
    vector<double> temp;
      for (unsigned int j=0;j<m_anchors.size();j++) { // iteration over anchors
        temp.push_back(m_anchors[j][i]);
      }
    temp_max.push_back(*max_element(temp.begin(), temp.end()));
    temp_min.push_back(*min_element(temp.begin(), temp.end()));
  }
  vector<double> center;
  for (unsigned int i=0;i<m_anchors[0].size();i++) { // iteration over coordinates
    center.push_back(temp_min[i] + 0.5* (temp_max[i] - temp_min[i]));
  }

  return center;
}

vector<double> ProfDriver::calcMin() {
  vector<double> temp_min;
  for (unsigned int i=0;i<m_anchors[0].size();i++) { // iteration over coordinates
    vector<double> temp;
      for (unsigned int j=0;j<m_anchors.size();j++) { // iteration over anchors
        temp.push_back(m_anchors[j][i]);
      }
    temp_min.push_back(*min_element(temp.begin(), temp.end()));
  }

  return temp_min;
}

vector<double> ProfDriver::calcMax() {
  vector<double> temp_max;
  for (unsigned int i=0;i<m_anchors[0].size();i++) { // iteration over coordinates
    vector<double> temp;
      for (unsigned int j=0;j<m_anchors.size();j++) { // iteration over anchors
        temp.push_back(m_anchors[j][i]);
      }
    temp_max.push_back(*max_element(temp.begin(), temp.end()));
  }

  return temp_max;
}

void ProfDriver::addValue(string binid, double val) {
  m_values[binid].push_back(val);
}

double ProfDriver::getValue(string key, vector<double> P) {
  vector<double> LV = getLongVector(getDP(P, m_center), boost::get<1>(m_coeffs[key]), boost::get<0>(m_coeffs[key]));
  double v = 0.0;
  for (int i=0; i< LV.size();i++) {
    v += LV[i]*boost::get<1>(m_coeffs[key])[i];
  }
  return v;
}


void ProfDriver::printMeta() {
  cout << "Parametrisation info: " << m_info << endl;
  //cout << "Polynomial degree: " << m_order << endl;
  cout << "Number of parameters: " << m_dim << " " <<  endl;
  cout << "Parameters:";
  for (int i=0;i<m_params.size();i++) {
    cout << " " << m_params[i];
  }
  cout << endl;
  cout << "Interpolation center:";
  for (int i=0;i<m_center.size();i++) {
    cout << " " << m_center[i];
  }
  cout << endl;
}


void ProfDriver::printCoeffs() {
  BOOST_FOREACH(const ProfDF::value_type& c, m_coeffs) {
    cout << c.first << " --- ";
    cout << "Order: " << boost::get<0>(c.second);
    for (int i=0; i<boost::get<1>(c.second).size();i++) {
      cout << " " << boost::get<1>(c.second)[i];
    }
    cout << endl;
  }
}

void ProfDriver::printBinIds() {
  BOOST_FOREACH(const ProfDF::value_type& c, m_coeffs) {
    cout << c.first << endl;
  }
}

void ProfDriver::printNbins() {
    cout << m_coeffs.size() << " bins available" << endl;
}

void ProfDriver::readProfDF(const char* fname) {
  string line;
  ifstream myfile (fname);
  if (myfile.is_open())
  {
    while ( getline (myfile,line) )
    {
      vector<string> tokens;
      boost::algorithm::split(tokens, line, boost::is_any_of("\t "), boost::token_compress_on);
      if (tokens[0] != "#") {
        int thisorder=atoi(tokens[1].c_str());
        vector<double> c;
        for (int i=2; i<tokens.size();i++) c.push_back(atof(tokens[i].c_str()));

        tuple<int, vector<double> > pb(thisorder, c);
        m_coeffs[tokens[0]] = pb;
      }
      else {
        if (tokens[1] == "DIM") {
          m_dim = atoi(tokens[2].c_str());
        }
        //else if (tokens[1] == "ORDER") {
          //m_order = atoi(tokens[2].c_str());
        //}
        else if (tokens[1] == "INFO") {
          for (int i=2;i<tokens.size();i++) {
            m_info+=" ";
            m_info+=tokens[i];
          }
        }
        else if (tokens[1] == "PARAMS") {
          for (int i=2;i<tokens.size();i++) {
            m_params.push_back(tokens[i]);
          }
        }
        else if (tokens[1] == "CENTER") {
          for (int i=2;i<tokens.size();i++) {
            m_center.push_back(atof(tokens[i].c_str()));
          }
        }
        else if (tokens[1] == "MINV") {
          for (int i=2;i<tokens.size();i++) {
            m_min.push_back(atof(tokens[i].c_str()));
          }
        }
        else if (tokens[1] == "MAXV") {
          for (int i=2;i<tokens.size();i++) {
            m_max.push_back(atof(tokens[i].c_str()));
          }
        }
      }
    }
    myfile.close();
    cout << "Successfully read coefficients from " << fname << endl;
  }
  else cout << "Unable to open file " << fname; 
}

void ProfDriver::writeProfDF(const char* fname) {
  ofstream myfile (fname);
  
  if (myfile.is_open())
  {
    myfile << "# INFO Professor ProfDF generated with profdriver version " << this->m_version << "\n";
    myfile << "# DIM " << this->dim() <<"\n"; 
    //myfile << "# ORDER " << this->order() <<"\n";
    myfile << "# PARAMS";
    for (unsigned int i=0;i<m_params.size();i++) {
      myfile << " " << m_params[i];
    }
    myfile << "\n";
    myfile << "# CENTER";
    for (unsigned int i=0;i<m_center.size();i++) {
      myfile << " " << std::setprecision(16) << m_center[i];
    }
    myfile << "\n";
    myfile << "# MINV";
    for (unsigned int i=0;i<m_min.size();i++) {
      myfile << " " << std::setprecision(16) << m_min[i];
    }
    myfile << "\n";
    myfile << "# MAXV";
    for (unsigned int i=0;i<m_max.size();i++) {
      myfile << " " << std::setprecision(16) << m_max[i];
    }
    myfile << "\n";
    BOOST_FOREACH(const ProfDF::value_type& c, m_coeffs) {
      myfile << c.first;
      myfile << " " << boost::get<0>(c.second);
      for (int i=0; i<boost::get<1>(c.second).size();i++) {
        myfile << " " << std::setprecision(16) << boost::get<1>(c.second)[i];
      }
      myfile << "\n";
    }
    myfile.close();
    cout << "Successfully wrote coefficients to " << fname << endl;
  }
  else cout << "Unable to open file for writing"; 
}

int ProfDriver::binomial(int n, int k) {
    int ntok = 1;
    int r = min(k, n-k);
    for(int i=0; i<r;++i) {
      ntok=ntok*(n-i)/(i+1);
    }
    return ntok;
}

// Tested and working
int ProfDriver::numOfCoefficients(int dim, int order) {
  return binomial(dim+order, order);
}

vector<double> ProfDriver::getLongVector1D(vector<double> p) {
  int nop = p.size();
  vector<double> retvec;
  retvec.push_back(1.0);    // This is the offset, for alpha
  for (int i=0;i<nop;i++) { // Linear coefficients, for beta
    retvec.push_back(p[i]);
  }

  assert(retvec.size() == numOfCoefficients(nop,1));
  return retvec;
}

vector<double> ProfDriver::getLongVector2D(vector<double> p) {
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

  assert(retvec.size() == numOfCoefficients(nop,2));
  return retvec;
}

vector<double> ProfDriver::getLongVector3D(vector<double> p) {
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

  assert(retvec.size() == numOfCoefficients(nop,3));
  return retvec;
}

vector<double> ProfDriver::getLongVector4D(vector<double> p) {
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

  assert(retvec.size() == numOfCoefficients(nop,4));
  return retvec;
}
vector<double> ProfDriver::getLongVector5D(vector<double> p) {
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

  assert(retvec.size() == numOfCoefficients(nop,5));
  return retvec;
}
vector<double> ProfDriver::getLongVector6D(vector<double> p) {
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

  assert(retvec.size() == numOfCoefficients(nop,6));
  return retvec;
}

vector<double> ProfDriver::getLongVector(vector<double> p, vector<double> coeffs, int order) {
  if (order < 1 || order > 6) {
    cout << "ERROR degree " << order << " not implemented, exiting" << endl;
    exit(1);
  }
  if (coeffs.size() != numOfCoefficients(p.size(), order)) {
    cout << "ERROR invalid number of coefficients: " << coeffs.size() << " supplied, " << numOfCoefficients(p.size(), order) << " required, exiting" << endl;
  }
  if (order == 1) return getLongVector1D(p);
  if (order == 2) return getLongVector2D(p);
  if (order == 3) return getLongVector3D(p);
  if (order == 4) return getLongVector4D(p);
  if (order == 5) return getLongVector5D(p);
  if (order == 6) return getLongVector6D(p);

}

vector<double> ProfDriver::getLongVector(vector<double> p, int order) {
  if (order < 1 || order > 6) {
    cout << "ERROR degree " << order << " not implemented, exiting" << endl;
    exit(1);
  }
  if (order == 1) return getLongVector1D(p);
  if (order == 2) return getLongVector2D(p);
  if (order == 3) return getLongVector3D(p);
  if (order == 4) return getLongVector4D(p);
  if (order == 5) return getLongVector5D(p);
  if (order == 6) return getLongVector6D(p);
}

vector<double> ProfDriver::getDP(vector<double> P, vector<double> C) {
  vector<double> dp;
  for (int i=0; i<P.size();i++) {
    dp.push_back(P[i] - C[i]);
  }
  return dp;
}
