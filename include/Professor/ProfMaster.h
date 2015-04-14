#ifndef PROFMASTER_H
#define PROFMASTER_H

#include "Professor/Ipol.h"
#include "Professor/ParamPoints.h"
#include <unordered_map>
#include <memory>

namespace Professor {


  /// Class that knows about anchors AND ipols
  /// @todo Is this necessary / helpful? Rename, at least...
  class ProfMaster {
  public:

    /// Constructor with parameter point anchors (as ParamPoints or vector<vector<double>>)
    ProfMaster(const ParamPoints& p)
      : _anchors(p) { }

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

    /// Get the Ipol named 'key'
    const Ipol& ipol(const std::string& key) const { return *_pdf.at(key); }

    /// Get the value from the Ipol named 'key'
    double value(const std::string& key, const std::vector<double>& params) const;


  private:

    std::unordered_map<std::string, std::unique_ptr<Ipol> > _pdf; //< *Unordered* map for speed (?)
    const ParamPoints _anchors;
    // std::vector<std::string> _paramnames;
    // std::string _version;
    // std::string _message;
    // std::string _info;

  };


}

#endif
