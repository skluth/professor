#include "Professor/ProfMaster.h"

using namespace std;


void ProfMaster::addIpol(const string& name, const vector<double>& v, int order) {
  _pdf[name] = unique_ptr<Ipol>(new Ipol(_anchors, v, order, name)); ///< @todo Should use make_unique, but that requires C++14
}

void ProfMaster::addIpol(const string& ipolstr) {
  Ipol* ip = new Ipol(ipolstr);
  _pdf[ip->name()] = unique_ptr<Ipol>(ip); ///< @todo Should use make_unique, but that requires C++14
}

double ProfMaster::value(const string& key, const vector<double>& params) const {
  return _pdf.at(key)->value(params); //< map::at added in C++11
}
