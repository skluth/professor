#ifndef PROFDRIVER_H
#define PROFDRIVER_H
#include "Professor/Ipol.h"
#include <unordered_map>

using namespace std;
using namespace Professor;

class ProfDriver {
 
    public:
         /// The constructor
         ProfDriver(string const& filename); // Read coeffs from file
         ProfDriver(const char* filename);
         double value(string, int, vector<double>);
         vector<double> values(string, vector<double>);

    private:
         /// The map of Ipols
         unordered_map<string, vector< Professor::Ipol> > _ipols;
         unordered_map<string, string > _meta;
         void readIpol(const char* fname);
         void readMeta(const char* fname);
         string _version;
         std::vector<std::string> tokenize(const std::string& input, const char* separator);
         bool has_key(unordered_map< string, vector< Ipol > > mymap, string key);


};
#endif
