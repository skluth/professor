// Professor technology demonstrator
#include "ProfDriver.h"
#include <iostream>
#include <cstdlib>
#include <sstream>

using namespace std;

//http://stackoverflow.com/questions/3619872/append-integer-to-end-of-const-char-c
std::string mkFname(std::string const& x, unsigned int num) {
    std::stringstream s;
    s << x << num << ".prof";
    return s.str();
}

int main(int argc, char** argv)
{
  string profdf = "";
  string binid = "";
  if (argc > 2)
    {
      profdf = argv[1];
      binid =  argv[2];
      // fast option load single grid file
      ProfDriver *pd = new ProfDriver(profdf);

      pd->printMeta();
      pd->printMeta();
      pd->printNbins();

      delete pd;
     
    }
  else
    {
      vector<string> params;
      params.push_back("PAR1");
      params.push_back("PAR2");
      vector<double> anchor1, anchor2, anchor3;
      anchor1.push_back(0.5449);
      anchor1.push_back(0.6028);
      anchor2.push_back(0.3834);
      anchor2.push_back(0.9637);
      anchor3.push_back(0.5289);
      anchor3.push_back(0.7917);
      for (int i=1;i<2;i++) {
        ProfDriver *pd = new ProfDriver(i);
        pd->setParamNames(params);
        pd->addAnchor(anchor1);
        pd->addAnchor(anchor2);
        pd->addAnchor(anchor3);
        pd->addValue("test", 0.682881040904);
        pd->addValue("test", 1.06349167191 );
        pd->addValue("test", 0.886184394455);
        pd->addValue("tesr", 0.682881040904);
        pd->addValue("tesr", 1.06349167191 );
        pd->addValue("tesr", 0.886184394455);
        pd->calc("test");
        pd->calc("tesr");
        string outname = mkFname("output", i);
        pd->writeProfDF(outname.c_str());
        cout << "Test at center order " << i << ": " << pd->getValue("test", pd->getCenter()) << endl;
        delete pd;
      }
      ProfDriver *pd = new ProfDriver();
      pd->setParamNames(params);
      pd->addAnchor(anchor1);
      pd->addAnchor(anchor2);
      pd->addAnchor(anchor3);
      pd->addValue("test", 0.682881040904);
      pd->addValue("test", 1.06349167191 );
      pd->addValue("test", 0.886184394455);
      pd->addValue("tesr", 0.682881040904);
      pd->addValue("tesr", 1.06349167191 );
      pd->addValue("tesr", 0.886184394455);
      pd->calc("test",1);
      pd->calc("tesr",1);
      cout << "Test at center before write out: " << pd->getValue("test", pd->getCenter()) << endl;
      string outname = mkFname("output", 23);
      pd->writeProfDF(outname.c_str());
      delete pd;
      
      ProfDriver *pdread = new ProfDriver(outname);
      cout << "Test at center after read in: " << pdread->getValue("test", pdread->getCenter()) << endl;
      delete pdread;
    }

  



  return 0;
}
