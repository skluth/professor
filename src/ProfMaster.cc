#include "Professor/ProfMaster.h"
#include "Professor/ParamPoints.h"
#include <iostream>

using namespace std;


ProfMaster::~ProfMaster() {
}


void ProfMaster::addIpol(string name, vector<double> v, int order) {
  _pdf[name] = new Ipol(*_anchors, v, order, name);

}

void ProfMaster::addIpol(string ipolstring) {
  Ipol* ip = new Ipol(ipolstring);
  _pdf[ip->name()] = ip;
}

double ProfMaster::getValue(string key, vector<double> P) {
  return _pdf[key]->value(P);
}

