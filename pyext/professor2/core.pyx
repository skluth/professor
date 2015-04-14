cimport professor as c
cimport cython.operator.dereference as deref


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


cdef class ParamPoints:
    cdef c.ParamPoints* _ptr

    def __cinit__(self, V):
        self._ptr = new c.ParamPoints(V)


cdef class Ipol:
    cdef c.Ipol *_ptr

    # Overloading workaround
    def __cinit__(self, *args):
        if len(args) == 1 and type(args[0]) is str:
            self._ptr = new c.Ipol(args[0])
        else:
            pp = ParamPoints(args[0])
            vals = list(args[1])
            order = int(args[2])
            name = str(args[3]) if len(args) == 4 else ""
            self._ptr = new c.Ipol(deref(pp._ptr), vals, order, name)

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
