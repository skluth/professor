#include <iostream>
#include <fstream>
#include <string>
#include <boost/tokenizer.hpp>
#include <unordered_map>
#include "Professor/Ipol.h"
#include "ProfDriver/ProfDriver.h"

using namespace boost;

ProfDriver::ProfDriver(string const& filename) {
  _version="0.2.0";
  readIpol(filename.c_str());
}


void ProfDriver::readIpol(const char* fname) {

    std::cout << "ProfDriver (" << _version << ") Please cite arXiv:0907.2973 [hep-ph]" << std::endl;
    std::cout << "Reading Professor parametrisations from file " << fname << std:: endl;
    std::ifstream infile(fname);
    std::string line;
    bool header_complete = false;
    vector<string> temp;
    string hname("");
    vector<std::string> hnames;
    while (std::getline(infile, line)) {
        std::string first = line.substr(0,1);
        if (first=="#") continue;
        if (!header_complete) {
            temp = tokenize(line, ":");
            if (temp.size()==1 && temp[0]=="---") {
                header_complete = true;
            }
        }
        else {
            if (first=="/") {
                hname = tokenize(line, "#")[0];
                vector<Ipol> test;
                if (!has_key(_ipols,hname)) {
                  hnames.push_back(hname);
                    _ipols[hname] = test;
                }
            }
            else {
                string istring = tokenize(line, ":")[1];
                _ipols[hname].push_back(Ipol(istring));
            }
        }
    } // End while loop

    int nbins=0;
    for (unsigned int i=0; i< hnames.size();++i) {
      nbins += _ipols[hnames[i]].size();
    }
    
    std::cout << "Successfully read " << nbins << " objects." << std::endl;
    //for (unsigned int i=0; i< hnames.size();++i) {
      //std::cout << hnames[i] << ": " << _ipols[hnames[i]].size() << " bins" << std::endl;
    //}
}

void ProfDriver::readMeta(const char* fname) {
    std::ifstream infile(fname);
    std::string line;
    bool header_complete = false;
    vector<string> temp;
    while (std::getline(infile, line)) {
        std::string first = line.substr(0,1);
        if (first=="#") continue;
        if (!header_complete) {
            temp = tokenize(line, ":");
            if (temp.size()==2) {
                _meta[temp[0]] = temp[1];
            }
            if (temp.size()==1 && temp[0]=="---") {
                header_complete = true;
            }
        }
        else break;
    } // End while loop
}

std::vector<std::string> ProfDriver::tokenize(const std::string& input, const char* separator) {
    char_separator<char> sep(separator);
    tokenizer<char_separator<char>> tokens(input, sep);
    vector<std::string> ret;
    for (const auto& t : tokens) {
        ret.push_back(t);
    }
    return ret;
}

bool ProfDriver::has_key(unordered_map< string, vector< Ipol > > mymap, string key) {
    std::unordered_map<std::string, vector< Ipol > >::const_iterator got = mymap.find (key);
    if (got==mymap.end()) return false;
    else return true;
}

double ProfDriver::value(string hname, int binnum, vector<double> P) {
  return _ipols[hname][binnum].value(P);
}

vector<double> ProfDriver::values(string hname, vector<double> P) {
  vector<double> temp;
  for (unsigned int i=0; i<_ipols[hname].size();++i) {
    temp.push_back(value(hname, i, P));
  }
  return temp;
}
