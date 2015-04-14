#ifndef PROFMASTER_H
#define PROFMASTER_H

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

#include "Professor/Ipol.h"
#include "Professor/ParamPoints.h"
#include <unordered_map>
#include <memory>

namespace Professor {


  /// Class that knows about anchors AND ipols
  /// @todo Is this necessary / helpful? Rename, at least...
  class ProfMaster {
  public:

    /// Constructor from anchors given as vector< vector<double> >
    /// @todo Add a constructor from a ParamPoints object... the 'more official' way
    ProfMaster(const std::vector< std::vector<double> >& p)
      : _anchors(p)
    { }

    // /// Needs implementation
    // void writeProfDF(const char* fname);

    // /// Needs implementation
    // void readProfDF(const char* fname);

    /// Build from values
    /// @todo Rename / add this to the constructor
    void addIpol(const std::string& name, const std::vector<double>& values, int order);

    /// Build from string
    /// @todo Rename / add this to the constructor
    void addIpol(const std::string& ipolstr);

    /// @todo Provide an addIpol which actually takes an Ipol object argument

    // /// @deprecated Not necessary/implemented
    // void setParamNames(const std::vector<std::string>& pnames) { m_params = pnames; }

    /// Get the value from the Ipol named 'key'
    double value(const std::string& key, const std::vector<double>& params) const;


  private:

    std::unordered_map<std::string, std::unique_ptr<Ipol> > _pdf; //< *Unordered* map for speed (?)
    const std::vector< std::vector<double> >& _anchors; ///< @todo Should be a ParamPoints
    // std::vector<std::string> _paramnames;
    // std::string _version;
    // std::string _message;
    // std::string _info;

  };


}

#endif
