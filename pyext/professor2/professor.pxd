from libcpp.map cimport map
from libcpp.pair cimport pair
from libcpp.vector cimport vector
from libcpp cimport bool
from libcpp.string cimport string



cdef extern from "Professor/ParamPoints.h":
    cdef cppclass ParamPoints:
        ParamPoints(vector[ vector[double] ])

cdef extern from "Professor/Ipol.h":
    cdef cppclass Ipol:
        Ipol(ParamPoints p, const vector[double]&, int, const string&)
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

cdef extern from "Professor/ProfMaster.h":
    cdef cppclass ProfMaster:
        ProfMaster(vector[ vector[double] ])
        void addIpol(string, vector[double], int)
        double getValue(string, vector[double])
