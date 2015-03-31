#include "Professor/ProfMaster.h"
#include "Professor/ParamPoints.h"
#include <iostream>

using namespace std;

//ProfMaster::ProfMaster(){
  //m_version="1.0";
  //m_message="Professor CPP-driver version " + m_version + " \nPlease cite arXiv:0907.2973 [hep-ph]\n2015 Andy Buckley, Holger Schulz, Simone Amoroso\n";
  //cout << m_message <<endl;
//}

//ProfMaster::ProfMaster(const ParamPoints& p){
  //_anchors = p;
//}

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



//void ProfMaster::readProfDF(const char* fname) {
  //string line;
  //ifstream myfile (fname);
  //if (myfile.is_open())
  //{
    //while ( getline (myfile,line) )
    //{
      //vector<string> tokens;
      //boost::algorithm::split(tokens, line, boost::is_any_of("\t "), boost::token_compress_on);
      //if (tokens[0] != "#") {
        //int thisorder=atoi(tokens[1].c_str());
        //vector<double> c;
        //for (int i=2; i<tokens.size();i++) c.push_back(atof(tokens[i].c_str()));

        //tuple<int, vector<double> > pb(thisorder, c);
        //m_coeffs[tokens[0]] = pb;
      //}
      //else {
        //if (tokens[1] == "DIM") {
          //m_dim = atoi(tokens[2].c_str());
        //}
        ////else if (tokens[1] == "ORDER") {
          ////m_order = atoi(tokens[2].c_str());
        ////}
        //else if (tokens[1] == "INFO") {
          //for (int i=2;i<tokens.size();i++) {
            //m_info+=" ";
            //m_info+=tokens[i];
          //}
        //}
        //else if (tokens[1] == "PARAMS") {
          //for (int i=2;i<tokens.size();i++) {
            //m_params.push_back(tokens[i]);
          //}
        //}
        //else if (tokens[1] == "CENTER") {
          //for (int i=2;i<tokens.size();i++) {
            //m_center.push_back(atof(tokens[i].c_str()));
          //}
        //}
        //else if (tokens[1] == "MINV") {
          //for (int i=2;i<tokens.size();i++) {
            //m_min.push_back(atof(tokens[i].c_str()));
          //}
        //}
        //else if (tokens[1] == "MAXV") {
          //for (int i=2;i<tokens.size();i++) {
            //m_max.push_back(atof(tokens[i].c_str()));
          //}
        //}
      //}
    //}
    //myfile.close();
    //cout << "Successfully read coefficients from " << fname << endl;
  //}
  //else cout << "Unable to open file " << fname; 
//}

//void ProfMaster::writeProfDF(const char* fname) {
  //ofstream myfile (fname);
  
  //if (myfile.is_open())
  //{
    //myfile << "# INFO Professor ProfDF generated with profdriver version " << this->m_version << "\n";
    //myfile << "# DIM " << this->dim() <<"\n"; 
    ////myfile << "# ORDER " << this->order() <<"\n";
    //myfile << "# PARAMS";
    //for (unsigned int i=0;i<m_params.size();i++) {
      //myfile << " " << m_params[i];
    //}
    //myfile << "\n";
    //myfile << "# CENTER";
    //for (unsigned int i=0;i<m_center.size();i++) {
      //myfile << " " << std::setprecision(16) << m_center[i];
    //}
    //myfile << "\n";
    //myfile << "# MINV";
    //for (unsigned int i=0;i<m_min.size();i++) {
      //myfile << " " << std::setprecision(16) << m_min[i];
    //}
    //myfile << "\n";
    //myfile << "# MAXV";
    //for (unsigned int i=0;i<m_max.size();i++) {
      //myfile << " " << std::setprecision(16) << m_max[i];
    //}
    //myfile << "\n";
    //BOOST_FOREACH(const ProfDF::value_type& c, m_coeffs) {
      //myfile << c.first;
      //myfile << " " << boost::get<0>(c.second);
      //for (int i=0; i<boost::get<1>(c.second).size();i++) {
        //myfile << " " << std::setprecision(16) << boost::get<1>(c.second)[i];
      //}
      //myfile << "\n";
    //}
    //myfile.close();
    //cout << "Successfully wrote coefficients to " << fname << endl;
  //}
  //else cout << "Unable to open file for writing"; 
//}

//}
