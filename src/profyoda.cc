// Professor technology demonstrator --- read yoda files
#include "ProfDriver.h"
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <map>
#include "YODA/Histo1D.h"
#include "AnyOption.h"
#include "YODA/Histo1D.h"
#include "YODA/WriterYODA.h"
#include "YODA/ReaderYODA.h"
#include "YODA/Reader.h"
#include "YODA/Writer.h"
#include <cmath>
#include <vector>
#include <fstream>

using std::map;
using namespace std;
using namespace YODA;


//http://stackoverflow.com/questions/3619872/append-integer-to-end-of-const-char-c
std::string mkFname(std::string const& x, unsigned int num) {
    std::stringstream s;
    s << x << num << ".prof";
    return s.str();
}

std::string mkBinId(std::string const& x, unsigned int num) {
    std::stringstream s;
    s << x << ":" << num;
    return s.str();
}

std::string appendInt(std::string const& x, unsigned int num) {
    std::stringstream s;
    s << x << "_" << num << ".yoda";
    return s.str();
}

// The object names by type
vector<string> getObjectNames(string fname, string type="Histo1D") {
  vector<AnalysisObject*> temp = ReaderYODA::create().read(fname);
  // Empty return vector
  vector<string> names;
  // Prepare vector of all binids --- for Histo1D things only
  for (unsigned int i=0; i<temp.size();i++) {
    if (temp[i]->type() == type) { // Check for type
      names.push_back(temp[i]->path());
    }
  }
  return names;
}

// The identifiers for the parametrisation map
vector<string> getBinIDs(string fname, string type="Histo1D") {
  // Open file
  vector<AnalysisObject*> temp = ReaderYODA::create().read(fname);
  // Empty return vector
  vector<string> binids;
  // Prepare vector of all binids --- for Histo1D things only
  for (unsigned int i=0; i<temp.size();i++) {
    if (temp[i]->type() == type) { // Check for type
      Histo1D* temphisto = (Histo1D*)temp[i]; // Get the current histo
      for (unsigned int b=0; b<temphisto->bins().size();b++) {
        binids.push_back(mkBinId(temphisto->path(), b));
      }
    }
  }
  return binids;
}

vector<HistoBin1D> getBinsFrom(string fname, string objname) {
  vector<AnalysisObject*> temp = ReaderYODA::create().read(fname);
  // Iterate over objects, find objname
  for (unsigned int i=0; i<temp.size();i++) {
    if (temp[i]->path() == objname) { // Check for type
      Histo1D* temphisto = (Histo1D*)temp[i]; // Get the current histo
      return temphisto->bins();
    }
  }
}

Histo1D* getHistoFrom(string fname, string objname, bool empty=true) {
  vector<AnalysisObject*> temp = ReaderYODA::create().read(fname);
  // Iterate over objects, find objname
  for (unsigned int i=0; i<temp.size();i++) {
    if (temp[i]->path() == objname) { // Check for type
      Histo1D* temphisto = ((Histo1D*)temp[i]); // Get the current histo
      if (empty) { 
        temphisto->reset();
      }
      return temphisto;
    }
  }
}

void updateBins(ProfDriver* prof, Histo1D* h, vector<double> p) {
  h->reset();
  for (unsigned int i=0;i<h->bins().size();i++) {
    string currbin = mkBinId(h->path(), i);
    h->bin(i).fillBin(prof->getValue(currbin, p));
  }
}

vector< vector<double> > readParamsAndyP(string fname, int dim) {
  vector< vector<double> > params;
  string line;
  ifstream myfile (fname.c_str());
  if (myfile.is_open()) {
    while ( getline (myfile,line) ) {
      vector<string> tokens;
      boost::algorithm::split(tokens, line, boost::is_any_of("\t "), boost::token_compress_on);
      if (tokens[0] != "#") {
        vector<double> c;
        for (int i=1; i<dim+1;i++) c.push_back(atof(tokens[i].c_str()));
        params.push_back(c);
      }
    }
    myfile.close();
    cout << "Successfully read parameters from " << fname << endl;
  }
  else cout << "Unable to open file " << fname << endl;

  return params;
}

int main(int argc, char* argv[]) {

  AnyOption *opt = new AnyOption();

  opt->addUsage( "\
  Professor parametrisation builder from yoda files \n\n\
  Usage: prof-yoda <options> yodafiles[...]\n\
\n\
   -h  --help         Print this help\n\
   -d  --debug        Print debug output\n\
   -q  --quiet        Limit output messages\n\
    " );

  opt->setFlag( "help", 'h' );
  opt->setFlag( "debug", 'd' );
  opt->setFlag( "quiet", 'q' );
  opt->setOption( "params", 'p' );
  opt->setOption( "degree", 'g' );
  opt->setOption( "output", 'o' );

  opt->processCommandArgs( argc, argv );

  int ipolorder=1;
  string paramfile = "";
  string outputfname = "default.yoda";

  if( ! opt->hasOptions()) {
    cout << endl << "Error, no options specified. Usage:" << endl;
    opt->printUsage();
    exit(1);
  }
  
  if( opt->getValue('p')!=0 || opt->getValue("params")!=0) {
    cout << endl << "Reading parameters from " << opt->getValue('p') << endl;
    paramfile = opt->getValue('p');
  }
  if( opt->getValue('g')!=0 || opt->getValue("degree")!=0) {
    cout << endl << "Set parametrisation otder to " << opt->getValue('g') << endl;
    ipolorder = atoi(string(opt->getValue('g')).c_str());
  }
  if( opt->getValue('o')!=0 || opt->getValue("output")!=0) {
    cout << endl << "Set output file name to " << opt->getValue('o') << endl;
    outputfname = opt->getValue('o');
  }
  cout << "Done." <<endl;

  // A vector of all the file-names
  cout << "Preparing parametrisation" << endl;
  vector<string> inputfilenames;
  for ( int i = 0 ; i < opt->getArgc() ; i++ ) {
    inputfilenames.push_back(opt->getArgv( i ));
  }
 
  // The object names --- type Histo1D
  vector<string> names_h1d=getObjectNames(inputfilenames[0], "Histo1D");
  // The binids --- type Histo1D
  vector<string> binids = getBinIDs(inputfilenames[0], "Histo1D");
  cout << "Found " << names_h1d.size() << " histograms in " << inputfilenames.size() << " files each" << endl;

  // Stuff
  map< string, vector<double> > data_h1d;
  map< string, vector<double> > data_h1d_errup;
  map< string, vector<double> > data_h1d_errdn;


  vector<HistoBin1D> temp_bins;
  cout << "Reading YODA histos" << endl;
  for (unsigned int i=0; i<names_h1d.size();i++) {
    //cout << "Processing " << i << " " << names_h1d[i] << endl;
    for (unsigned int j=0; j<inputfilenames.size();j++) {
      temp_bins = getBinsFrom(inputfilenames[j], names_h1d[i]);
      for (unsigned int k=0; k<temp_bins.size();k++) {
        data_h1d[mkBinId(names_h1d[i], k)].push_back(temp_bins[k].area());
        data_h1d_errup[mkBinId(names_h1d[i], k)].push_back(temp_bins[k].area() + temp_bins[k].areaErr());
        data_h1d_errdn[mkBinId(names_h1d[i], k)].push_back(temp_bins[k].area() - temp_bins[k].areaErr());
      }
    }
  }
  cout << "Done." << endl;


  // The driver freund
  ProfDriver *pd = new ProfDriver(ipolorder);
  ProfDriver *pd_errup = new ProfDriver(ipolorder);
  ProfDriver *pd_errdn = new ProfDriver(ipolorder);

  // Populate values
  typedef std::map<string, vector<double> > map_t;
  BOOST_FOREACH(map_t::value_type &c, data_h1d) {
    for (int i=0; i<c.second.size();i++) {
      pd->addValue(c.first, c.second[i]);
    }
  }
  BOOST_FOREACH(map_t::value_type &c, data_h1d_errup) {
    for (int i=0; i<c.second.size();i++) {
      pd_errup->addValue(c.first, c.second[i]);
    }
  }
  BOOST_FOREACH(map_t::value_type &c, data_h1d_errdn) {
    for (int i=0; i<c.second.size();i++) {
      pd_errdn->addValue(c.first, c.second[i]);
    }
  }
  
  vector< vector<double> > params = readParamsAndyP(paramfile, 4);
  for (unsigned int i=0;i<params.size();i++) {
    pd->addAnchor(params[i]);
    pd_errup->addAnchor(params[i]);
    pd_errdn->addAnchor(params[i]);
    for (unsigned int j=0;j<params[i].size();j++) {
      cout << params[i][j] << " ";
    }
    cout << endl;
  }

  vector<string> paramnames;
  paramnames.push_back("PAR1");
  paramnames.push_back("PAR2");
  paramnames.push_back("PAR3");
  paramnames.push_back("PAR4");
  pd->setParamNames(paramnames);
  pd_errup->setParamNames(paramnames);
  pd_errdn->setParamNames(paramnames);

  assert(pd->anchors().size() == inputfilenames.size());
  // Running the parametrisation
  cout << "Parametrising " << binids.size() << " objects" << endl;
  for (unsigned int i=0;i<binids.size();i++) {
    //cout << "Parametrising " << binids[i] << endl;
    pd->calc(binids[i]);
    pd_errup->calc(binids[i]);
    pd_errdn->calc(binids[i]);
    //cout << "Value at center: " << pd->getValue(binids[i], pd->getCenter()) << endl;
  }
  cout << "Done." << endl;
  int ncoeff = pd->numOfCoefficients(pd->getCenter().size(), ipolorder);
  cout << "center size: " << pd->getCenter().size() << " order " << ipolorder << endl;
  cout << "Number of coefficients: " << ncoeff << endl;

  string outname = mkFname("output", ipolorder);
  pd->writeProfDF(outname.c_str());

  // Dummy histos
  cout << "Generating ipolhistos" << endl <<endl;
  vector<Histo1D*> dummyH;
  for (unsigned int i=0;i<names_h1d.size();i++) {
    dummyH.push_back(getHistoFrom(inputfilenames[0], names_h1d[i], true));
  }

  //for (unsigned int i=0;i<pd->anchors().size();i++) {
    //typedef vector<Histo1D* > vec_h;
    //BOOST_FOREACH(vec_h::value_type &c, dummyH) {
      //updateBins(pd, c, pd->anchors()[i]);
    //}
    //WriterYODA::write(appendInt(outputfname, i), dummyH.begin(), dummyH.end());
  //}

  //for (unsigned int i=0;i<pd_errup->anchors().size();i++) {
    //typedef vector<Histo1D* > vec_h;
    //BOOST_FOREACH(vec_h::value_type &c, dummyH) {
      //updateBins(pd_errup, c, pd_errup->anchors()[i]);
    //}
    //WriterYODA::write(appendInt(outputfname+string("_errup"), i), dummyH.begin(), dummyH.end());
  //}
  //for (unsigned int i=0;i<pd_errdn->anchors().size();i++) {
    //typedef vector<Histo1D* > vec_h;
    //BOOST_FOREACH(vec_h::value_type &c, dummyH) {
      //updateBins(pd_errdn, c, pd_errdn->anchors()[i]);
    //}
    //WriterYODA::write(appendInt(outputfname+string("_errdn"), i), dummyH.begin(), dummyH.end());
  //}

  vector<double> a1;
  a1.push_back(-0.00290300);
  a1.push_back(0.00357811);
  a1.push_back(-0.00065765);
  a1.push_back(0.00044269);
  typedef vector<Histo1D* > vec_h;
  BOOST_FOREACH(vec_h::value_type &c, dummyH) {
    updateBins(pd, c, a1);
  }
  WriterYODA::write("Atanchor1.yoda", dummyH.begin(), dummyH.end());
  BOOST_FOREACH(vec_h::value_type &c, dummyH) {
    updateBins(pd_errup, c, a1);
  }
  WriterYODA::write("Atanchor1_errup.yoda", dummyH.begin(), dummyH.end());
  BOOST_FOREACH(vec_h::value_type &c, dummyH) {
    updateBins(pd_errdn, c, a1);
  }
  WriterYODA::write("Atanchor1_errdn.yoda", dummyH.begin(), dummyH.end());


  cout << endl << "Done" << endl;
  return 0;
}
