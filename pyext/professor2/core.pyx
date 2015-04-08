cimport professor as c

cdef class ProfMaster:
    cdef c.ProfMaster *_ptr

    def __cinit__(self, P):
        self._ptr = new c.ProfMaster(P)

    def __del__(self):
        del self._ptr

    def addIpol(self, name, V, order):
        self._ptr.addIpol(name, V, order)

    def getValue(self, name, P):
        return self._ptr.getValue(name, P)

# cdef class ParamPoints:
#     cdef c.ParamPoints *_ptr

#     def __cinit__(self, V):
#         self._ptr=new c.ParamPoints(V)

cdef class Ipol:
    cdef c.Ipol *_ptr

    # Overloading workaround
    def __cinit__(self, V1, V2=None, V3=None, V4=None):
        if V3 is None:
            self._ptr = new c.Ipol(V1)
        else:
            self._ptr = new c.Ipol(V1, list(V2), V3, V4)

    def coeffs(self):
        return self._ptr.coeffs()

    def value(self, P):
        return self._ptr.value(P)

    def order(self):
        return self._ptr.order()

    def toString(self, name=""):
        return self._ptr.toString(name)

    def name(self):
        return self._ptr.name()
