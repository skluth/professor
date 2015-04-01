/***
 *
 * Professor C++ Master
 *
 * Holger Schulz for the Professor Collaboration
 * email: holger.schulz@durham.ac.uk
 *
 * Requirements: boost, eigen3
 *
 *
 */
#ifndef PROFMASTER_H
#define PROFMASTER_H
#include "Professor/Ipol.h"
#include "Professor/ParamPoints.h"
#include <unordered_map>
#include "boost/tuple/tuple.hpp"

using std::string;
using std::vector;
using boost::tuple;

/// Coeffiecient map
typedef std::unordered_map<string, Ipol*> ProfDF;

/// Class that knows about anchors AND ipols
class ProfMaster {
  
    private:
        ProfDF _pdf;
        mutable ParamPoints* _anchors;
        string m_version;
        string m_message;
        string m_info;
        vector<string> m_params;


    public:
        /// ctor from anchors given as vector< vector<double> >
        ProfMaster(vector< vector<double> > p) {_anchors=new ParamPoints(p);};

        /// ctor from anchros given as ParamPoints
        ProfMaster(ParamPoints& p) {_anchors=&p;}; // Basic ctor
        
        /// The destructor
         ~ProfMaster();
        
        /// Needs implementation
        void writeProfDF(const char* fname);
        
        /// Needs implementation
        void readProfDF(const char* fname);

        /// Build from values ---- operator?
        void addIpol(string name, std::vector<double> values, int order);
        
        /// Build from string ---- operator?
        void addIpol(string ipolstring);

        /// Not necessary/implementad
        void setParamNames(vector<string> pnames) {m_params=pnames;};
        
        /// call value on Ipol with name key
        double getValue(string key, vector<double> P);
      
};

#endif
