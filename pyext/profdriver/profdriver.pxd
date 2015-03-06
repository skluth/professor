from libcpp.map cimport map
from libcpp.pair cimport pair
from libcpp.vector cimport vector
from libcpp cimport bool
from libcpp.string cimport string


ctypedef map[string, pair[int, vector[double] ] ] ProfDF

cdef extern from "ProfDriver.h":
    cdef cppclass ProfDriver:
        void writeProfDF(const char*)
        int numOfCoefficients(int, int)
        vector[ vector[double] ] anchors()
        void calc(string, int)
        void printMeta()
        void printNbins()
        void printBinIds()
        void printCoeffs()
        int dim()
        int order()
        void addAnchor(vector[double])
        void addValue(string, double)
        vector[double] getCenter()
        void setCenter(vector[double])
        vector[double] calcCenter()
        vector[double] calcMin()
        vector[double] calcMax()
        void setParamNames(vector[string])
        double getValue(string, vector[double])
