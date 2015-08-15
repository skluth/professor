from libcpp.map cimport map
from libcpp.pair cimport pair
from libcpp.vector cimport vector
from libcpp cimport bool
from libcpp.string cimport string


cdef extern from "Professor/ParamPoints.h" namespace "Professor":
    cdef cppclass ParamPoints:
        ParamPoints(const vector[ vector[double] ]&)
        # TODO: 'dress' this Python wrapper with more functionality than the basic C++ object

cdef extern from "Professor/Ipol.h" namespace "Professor":
    cdef cppclass Ipol:
        # Ipol(const vector[ vector[double] ] p, const vector[double]&, int, const string&)
        #Ipol(const ParamPoints& p, const vector[double]&, int, const string&)
        Ipol(const ParamPoints& p, const vector[double]&, int, const string&, const double&)
        Ipol(const string&)
        double value(const vector[double]&)
        int order()
        # TODO: add dim() and numCoeffs()
        double coeff(size_t)
        const vector[double]& coeffs()
        const ParamPoints& params()
        string toString()
        string toString(const string&)
        string name()

cdef extern from "Professor/ProfMaster.h" namespace "Professor":
    cdef cppclass ProfMaster:
        ProfMaster(vector[vector[double]])
        void addIpol(string, vector[double], int)
        double value(string, vector[double])
