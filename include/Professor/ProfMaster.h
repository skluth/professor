/***
 *
 * Professor C++ Master
 *
 * Holger Schulz for the Professor Collaboration
 * email: holger.schulz@durham.ac.uk
 *
 * Requirements: boost, eigen3
 *
 * January 2015
 *
 * Usage:
 *
 *  ProfMaster *pdf = new ProfMaster("coeffs.ProfDF");
 *
 *  pdf->getValue(binid, P); // -> returns double
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
//using std::tuple;
using boost::tuple;
// Coeffiecient map
typedef std::unordered_map<string, Ipol*> ProfDF;

class ProfMaster {
  
    private:
        ProfDF _pdf;
        mutable ParamPoints* _anchors;
        string m_version;
        string m_message;
        string m_info;
        vector<string> m_params;


        void readProfDF(const char* fname);

    public:
         ProfMaster(vector< vector<double> > p) {_anchors=new ParamPoints(p);}; // Basic ctor
         ProfMaster(ParamPoints& p) {_anchors=&p;}; // Basic ctor
         //ProfMaster(); // Basic ctor
         /// The destructor
        void writeProfDF(const char* fname);
         ~ProfMaster();

         // Build from values
         void addIpol(string name, std::vector<double> values, int order);
         // Build from string
         void addIpol(string ipolstring);

         void setParamNames(vector<string> pnames) {m_params=pnames;};
        
         double getValue(string key, vector<double> P);
      
};

#endif
