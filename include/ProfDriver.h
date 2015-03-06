/***
 *
 * Professor C++ Driver
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
 *  ProfDriver *pdf = new ProfDriver("coeffs.ProfDF");
 *
 *  pdf->getValue(binid, P); // -> returns double
 *
 *
 */
#ifndef PROFDRIVER_H
#define PROFDRIVER_H
#include <string>
#include <vector>
#include <map>
//#include <tuple>
#include "boost/tuple/tuple.hpp"

using std::string;
using std::vector;
//using std::tuple;
using boost::tuple;
// Coeffiecient map
typedef std::map<string, tuple<int, vector<double> > > ProfDF;

class ProfDriver {
  
    private:
        ProfDF m_coeffs;
        int m_dim;
        int m_order;
        //static string m_version = "0.1.1";
        //static string m_message = "Professor CPP-driver version " + m_version + " \nPlease cite arXiv:0907.2973 [hep-ph]\n2015 Andy Buckley, Holger Schulz, Simone Amoroso\n";
        string m_version;
        string m_message;
        vector<double> m_center;
        vector<double> m_min;
        vector<double> m_max;
        vector<string> m_params;

        vector< vector<double> > m_anchors;
        std::map< string, vector<double> > m_values; // Later replace by (unordered) map to accommodate for than one "bin"

        string m_info;

        
        int binomial(int n, int k);
        
        vector<double> getLongVector1D(vector<double> p);
        vector<double> getLongVector2D(vector<double> p);
        vector<double> getLongVector3D(vector<double> p);
        vector<double> getLongVector4D(vector<double> p);
        vector<double> getLongVector5D(vector<double> p);
        vector<double> getLongVector6D(vector<double> p);
        vector<double> getLongVector(vector<double> p, vector<double> coeffs, int order);
        vector<double> getLongVector(vector<double> p, int order);
        vector<double> getDP(vector<double> P, vector<double> C);
      
        
        void readProfDF(const char* fname);

    public:
        void writeProfDF(const char* fname);
        int numOfCoefficients(int dim, int order);
         /// The constructor
         ProfDriver(string const& filename); // To read from file
         ProfDriver(int order); // Globally set order of polynominal
         ProfDriver(); // Basic ctor
         /// The destructor
         ~ProfDriver();

         vector< vector<double> > anchors() {return m_anchors;};
         void calc(string binid, int order); // Most generic way of parametrisation
         void calc(string binid); // The version where the order is globally set in the constructor
         void printMeta();
         void printNbins();
         void printBinIds();
         void printCoeffs();
         int dim() {return m_dim;};
         int order() {return m_order;};

         //void addAV(vector<double> anchor, double val);
         void addAnchor(vector<double> anchor);
         void addValue(string binid, double val);

         vector<double> getCenter() {return m_center;};
         void setCenter(vector<double> ctr) {m_center=ctr;};
         vector<double> calcCenter();
         vector<double> calcMin();
         vector<double> calcMax();
         void setParamNames(vector<string> pnames) {m_params=pnames;};
        
         double getValue(string key, vector<double> P);
      
};

#endif
