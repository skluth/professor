from libcpp.map cimport map
from libcpp.pair cimport pair
from libcpp.vector cimport vector
from libcpp cimport bool
from libcpp.string cimport string



#cdef extern from "Professor/ParamPoints.h":
    #cdef cppclass ParamPoints:
        #ParamPoints(vector[ vector[double] ])

cdef extern from "Professor/ProfMaster.h":
    cdef cppclass ProfMaster:
        ProfMaster(vector[ vector[double] ])
        void addIpol(string, vector[double], int)
        double getValue(string, vector[double])
