#ifndef IPOL_H
#define IPOL_H
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <map>
#include "boost/tuple/tuple.hpp"
#include "Professor/ParamPoints.h"

using std::string;
using std::min;
using std::cout;
using std::endl;
using std::vector;
using std::stringstream;

class Ipol {
public:

  // ctor for (lazy) calculation of coeffs
  // TODO understand the const stuff and how to fix it: error: passing ‘const ParamPoints’ as ‘this’ argument of ‘void ParamPoints::printMeta()’ discards qualifiers [-fpermissive]
  Ipol(ParamPoints& pts, vector<double> values, int order) {
    _values=values;
    _pts=&pts;
    _order=order;
  };
  //Ipol(const ParamPoints& pts, vector<double> values, int order) {pts.printMeta();};

  // ctor reading existing coeffs string
  //Ipol(const string& str, vector<double> center) { fromString(str, center); }

  int numOfCoefficients(int, int);
  void fromString(const string& s);

  //string toString(const string& name="") {
    //stringstream ss;
    //if (!name.empty()) ss << name << ": ";
    //ss << this->dim() << " ";
    //ss << this->order() << " ";
    //for (const double& a : coeffs())
      //ss << coeff << " ";
    //return ss.str();
  //}
  double value(vector <double> p);

  double value(const ParamPoints& pt) const {
    return 1.0;
  }

  //vector<double> coeffs() const {
    //if (_coeffs.empty()) {
      //if (_pts == NULL) throw IpolError("No ParameterPoints available when calculating ipol coeffs");
      //_calcCoeffs();
      //_pts = NULL; //< Not necessary, but ensures consistency
    //}
    //return _coeffs;
  //}

  int dim() const { return _pts->dim(); }

  int order() const { return _order; }


private:

  int _order;

  vector<double> _values;

  mutable vector<double> _coeffs;

  mutable ParamPoints* _pts; //= 0; TODO: warning: non-static data member initializers only available with -std=c++11
  
  void _calcCoeffs(); // Most generic way of parametrisation
  //void calc(string binid, int order); // Most generic way of parametrisation
  //void calc(string binid); // The version where the order is globally set in the constructor
  
  vector<double> getLongVector1D(vector<double> p);
  vector<double> getLongVector2D(vector<double> p);
  vector<double> getLongVector3D(vector<double> p);
  vector<double> getLongVector4D(vector<double> p);
  vector<double> getLongVector5D(vector<double> p);
  vector<double> getLongVector6D(vector<double> p);
  vector<double> getLongVector(vector<double> p, vector<double> coeffs, int order);
  vector<double> getLongVector(vector<double> p, int order);

};

#endif
