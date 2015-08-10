#include "ProfDriver/ProfDriver.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;
using namespace Professor;

int main(int argc, char** argv)
{

  ProfDriver* p = new ProfDriver(string(argv[1]));
  //ProfDriver* p;
  //p = new ProfDriver(argv[1]);


  //readMeta(argv[1]);
  //readIpol(argv[1]);

  //vector<Ipol> IV = readIpol(argv[1])["/CMS_2013_I1218372/d03-x01-y01"];

  //TH1F* h_test = new TH1F("test", "test", 7, 0, 1);

  //for (unsigned int n=0;n<10;n++) {
      //vector<double> P{0.0015*n+0.5, 2, 1, 2, 0.5, 0.5, 2.5, 0.5, 1.0};
      //cout << p->value("/CMS_2013_I1218372/d03-x01-y01", 1 ,P) << endl;
  //}

}
