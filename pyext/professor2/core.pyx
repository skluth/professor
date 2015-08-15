cimport professor as c
cimport cython.operator.dereference as deref


cdef class ParamPoints:
    cdef c.ParamPoints* _ptr

    def __cinit__(self, pvec):
        self._ptr = new c.ParamPoints(pvec)

    def __del__(self):
        del self._ptr


cdef class Ipol:
    """An interpolation of a scalar function built from a list of values across
    a set of parameter point anchors.

    The main workhorse object in Professor. The interpolation coefficients are
    calculated lazily, i.e. when first used.
    """
    cdef c.Ipol* _ptr

    def __cinit__(self, *args):
        if len(args) == 1 and type(args[0]) is str:
            self._ptr = new c.Ipol(args[0])
        else:
            pp = ParamPoints(args[0])
            vals = list(args[1])
            order = int(args[2])
            name = ""
            threshold = 1.e-10
            if len(args) == 4:
                try:
                    threshold = float(args[3])
                except:
                    name = str(args[3])
            if len(args) == 5:
                    name = str(args[3])
                    threshold = float(args[4])

            self._ptr = new c.Ipol(deref(pp._ptr), vals, order, name, threshold)

    def __del__(self):
        del self._ptr

    def coeffs(self):
        return self._ptr.coeffs()

    def value(self, *params):
        if len(params) == 1 and hasattr(params[0], "__iter__"):
            params = params[0]
        return self._ptr.value(params)

    def order(self):
        return self._ptr.order()

    def toString(self, name=""):
        return self._ptr.toString(name)

    def name(self):
        return self._ptr.name()



cdef class ProfMaster:
    """Helper object for managing a collection of Ipols.

    Construct from a list of
    parameter point anchors, then make named interpolations (e.g. one per bin) by
    adding lists of values corresponding to those points.
    """
    cdef c.ProfMaster* _ptr

    # TODO: overloading
    def __cinit__(self, parampts):
        self._ptr = new c.ProfMaster(parampts)

    def __del__(self):
        del self._ptr

    # TODO: overloading
    def addIpol(self, name, vals, order):
        self._ptr.addIpol(name, vals, order)

    def value(self, name, params):
        return self._ptr.value(name, params)
