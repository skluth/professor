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
