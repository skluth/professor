#cython: embedsignature=True

cimport professor as c
cimport cython.operator.dereference as deref


def version(astuple=False):
    "Professor version code, as a string by default or a tuple on request"
    v = c.version()
    return v.split(".") if astuple else v


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
        # NOTE: we shouldn't invent Python-only constructors for a one-off purpose that can be done adequately/better with an explicit call...
        # if len(args) == 3 and type(args[0]) is str and type(args[1]) is str and type(args[2]) is str:
        #     self._ptr = new c.Ipol(args[0])
        #     self._ptr.setMinParamVals([float(x) for x in args[1].split()])
        #     self._ptr.setMaxParamVals([float(x) for x in args[2].split()])
        if len(args) == 1 and type(args[0]) is str: # Backward compatibility --- no scaling
            self._ptr = new c.Ipol(args[0])
            # self._ptr.setMinParamVals([0 for x in xrange(self._ptr.dim())])
            # self._ptr.setMaxParamVals([1 for x in xrange(self._ptr.dim())])
        else:
            pp = ParamPoints(args[0])
            vals = list(args[1])
            order = int(args[2])
            name = ""
            threshold = 1e-15 #< ???
            if len(args) == 4:
                try:
                    threshold = float(args[3])
                except:
                    name = str(args[3])
            if len(args) == 5:
                    name = str(args[3])
                    threshold = float(args[4])
            self._ptr = new c.Ipol(deref(pp._ptr), vals, order, name, threshold, True)

    def __del__(self):
        del self._ptr

    @property
    def coeffs(self):
        return self._ptr.coeffs()

    @property
    def dim(self):
        return self._ptr.dim()

    @property
    def order(self):
        return self._ptr.order()

    @property
    def name(self):
        return self._ptr.name()

    def value(self, *params, vmin=None, vmax=None):
        """Calculate the value of this interpolation at the given params point,
        forcing return within the range vmin..vmax.

        params can be an expanded tuple of floats, an unexpanded iterable of
        floats, or an ordered dict of paramname -> value.
        """

        import collections

        ## Detect if the params have been passed as a single iterable and convert
        if len(params) == 1 and isinstance(params[0], collections.Iterable):
            params = params[0]
            ## Further, detect if the params have been passed as a (ordered!) dict-like and extract the (ordered) values
            if isinstance(params, collections.Mapping):
                params = params.values()

        ## Ensure that the param values are floats
        params = [float(p) for p in params]

        ## Compute the interpolated value at 'params' and impose optional range limits
        v = self._ptr.value(params)
        if vmin is not None and v < vmin:
            return vmin
        if vmax is not None and v > vmax:
            return vmax

        return v

    ## Alias
    val = value


    def setParamLimits(self, pmins, pmaxs):
        "Set the minimum and maximum param values via 2 lists ordered cf. the param names. Used in SVD internal scaling."
        self._ptr.setParamLimits(pmins, pmaxs)

    def setMinParamVals(self, pmins):
        "Set the minimum param values via a list of values ordered cf. the param names. Used in SVD internal scaling."
        self._ptr.setMinParamVals(pmins)

    def setMaxParamVals(self, pmaxs):
        "Set the maximum param values via a list of values ordered cf. the param names. Used in SVD internal scaling."
        self._ptr.setMaxParamVals(pmaxs)


    def toString(self, name=""):
        "Produce a persistent string representing this Ipol object"
        return self._ptr.toString(name)

    def __repr__(self):
        return self.toString(self.name)
