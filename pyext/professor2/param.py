import re
from operator import indexOf
import numpy



class ParamBase(numpy.ndarray):
    """Base class for name-value kind of containers.

    Factory functions for easy instance creation are:
        * :meth:`mkFromDict`
        * :meth:`mkFromFile`

    The constructor takes two arguments:

        names : array_like
            The parameter names.
        values : array_like
            The parameter values.

    """
    # Note: This code is mostly copied from the subclassing.py.gz documentation.
    # Note 2: And again bluntly copied from Professor 1
    def __new__(cls, names, values):
        """Create a new instance from lists of names and associated values.

        Parameters
        ----------
        names : array_like
            The parameter names.
        values : array_like
            The parameter values.

        Raises
        ------
        ValueError
            If `names` and `values` have incompatible length.
        """
        names = numpy.asarray(names)
        values = numpy.asarray(values)
        if names.shape[0] != values.shape[0]:
            raise ValueError("Different number of parameter names and values!")

        # Sort `values` by the order of parameters in `names`.
        idx = numpy.argsort(names)
        names = names[idx]
        values = values[idx]
        values = numpy.asarray(values).view(type=cls)

        values._parameter_names = tuple(names)
        values._dim = len(names)
        return values


    def __array_finalize__(self, obj):
        self._parameter_names = getattr(obj, "_parameter_names", None)
        self._dim = getattr(obj, "_dim", None)

    def checkCondition(self, clist):
        """
        Binary conditions, clist must contain lists with 3 items each:
            Key1, interpretable symbol, Key2

        e.g. ["PAR1", "<", "PAR2"]
        """
        dec = True
        for c in clist:
            if not eval('self["%s"] %s self["%s"]'%(c[0], c[1], c[2])):
                dec=False
                break
        return dec



    @classmethod
    def mkFromFile(cls, path):
        """Load parameters from a file.

        Parameters
        ----------
        path : str
            The path of the file.
        """
        with open(path) as f:
            names, values = cls._parselines(f)
        return cls(names, values)


    @classmethod
    def _parselines(cls, lines):
        """Parse a sequence/iterator of lines.

        Parameters
        ----------
        lines : iterator of strings, e.g. a ``file`` object.
            The strings to parse.
        Returns
        -------
        rnames : list of strings
            The parameter names
        rvals : list of list of floats, list of floats
            (Nested) list of parameter values.
        """
        rnames = []
        rvals = []
        scanparam = None
        numpar = 0
        # Keep the origline for useful error messages.
        for origline in lines:
            l = re.sub(r'(^.*?)#.*', r'\1', origline) # strip comments
            l = l.replace("=", " ")
            l = l.strip()
            if len(l) == 0:
                continue
            tokens = l.split()
            name = tokens[0]
            vals = map(float, tokens[1:])
            if numpar == 0:
                numpar = len(vals)
            if numpar != len(vals):
                raise ValueError("Number of parameter values differ from"
                                 " previous lines at line"
                                 " '%s'" % (origline[:-1]))

            ## Convert single values to floats
            if len(vals) == 1:
                vals = vals[0]

            rnames.append(name)
            rvals.append(vals)

        return rnames, rvals


    @classmethod
    def mkFromDict(cls, d):
        """Create an instance from a dictionary.

        Parameters
        ----------
        d : dict
            For 1D parameter data, e.g. parameter points
            `d` must have the form::

                d = {"PAR1" : val1, "PAR2" : val2 }

            for other data types, e.g. parameter errors and ranges::

                d = {"PAR1" : [val1, val2],
                     "PAR2" : [val3, val4] }

            In principle nesting is allowed, resulting in 3+ dimensional
            arrays.
        """
        names, values = [], []
        for n, v in d.items():
            names.append(n)
            values.append(v)
        return cls(names, values)


    def format(self, sep="  "):
        """Format parameter names and values

        The format is::
            Par1  val1
            Par2  val2
            ...

        Numbers are formatted using 4-digit precision `%g`.

        Parameters
        ----------
        key : function, optional
            Comparison function used to sort parameter names with the
            builtin `sorted` function. By default
            :func:`professor.tools.sorting.cmpByInt` is used to sort
            parameter names by a contained number.
        sep : str, optional
            String used to separate parameter names and values.
            [default: two spaces]
        """
        d = self.asDict()
        w = max(len(k) for k in d.keys())
        s = ""
        for k in sorted(d.keys()):
            s += "%s%s%8.4g\n" % (k.ljust(w), sep, d[k])
        return s[:-1]


    def __str__(self):
        return self.format()


    def writeParamFile(self, path, key=None):
        """Write the return value of :meth:`format` to a file."""
        if "/" in path.rstrip("/"):
            dname = path.rsplit("/", 1)[0] # the base path
            import os
            if not os.path.exists(dname):
                os.makedirs(dname)

        with open(path, "w") as f:
            f.write(self.format() + "\n")


    @property
    def names(self):
        """The parameter names.

        Warnings
        --------
        The names are stored in an `numpy.ndarray` as instances of `numpy.string_`.
        """
        return self._parameter_names


    def keys(self):
        """Get the parameter names.

        Implemented for dict-like behaviour::

            >>> d = dict()
            >>> params = ParameterBase(["a", "z"], [1.3, 9e4])
            >>> d.update(params)
            >>> d
            {'a' : 1.3, 'z' : 90000.0}
        """
        return self.names


    @property
    def dim(self):
        return self._dim


    def getIndex(self, name):
        """Return the index of parameter `name`."""
        return indexOf(self.names, name)


    # Note: We have to overwrite both __getitem__ and __getslice__, although
    # __getslice__ is marked as deprecated in the Python documentation!
    def __getitem__(self, idx):
        """x.__getitem__(idx) <==> x[idx]

        Using parameter names as single index is supported. Slicing with
        parameter names is not supported.

        Returns
        -------
        ret
            If `idx` is a slice object the returned object has the same type
            as this. In all other cases, a simple `np.ndarray` or item type
            is returned.
        """
        if isinstance(idx, str):
            idx = self.getIndex(idx)
        elif type(idx) == slice:
            names = self.names[idx]
            values = numpy.asarray(self)[idx]
            return type(self)(names, values)

        return numpy.asarray(self)[idx]


    def __getslice__(self, a, b):
        return self.__getitem__(slice(a,b))


    def __setitem__(self, idx, val):
        """x.__setitem__(idx, val) <==> x[idx] = val

        Using parameter names as single index is supported. Slicing with
        parameter names is not supported.
        """
        if isinstance(idx, str):
            idx = self.getIndex(idx)
        numpy.ndarray.__setitem__(self, idx, val)


    def asDict(self):
        """Convert the data to dict.

        The keys are the parameter names the values the stored numerical
        data.

        The resulting dictionary is suitable to create a new instance with
        :meth:`~ParameterBase.mkFromDict`.
        """
        return dict(zip(self.names, self))


    # Implement the pickle protocol
    # see: http://mail.scipy.org/pipermail/numpy-discussion/attachments/20070415/ebf0a380/attachment.py
    def __reduce__(self):
        obj_state = list(numpy.ndarray.__reduce__(self))
        sub_state = (self._parameter_names, self._dim, self.scanparam)
        obj_state[2] = (obj_state[2], sub_state)
        return tuple(obj_state)


    def __setstate__(self, state):
        nd_state, own_state = state
        numpy.ndarray.__setstate__(self, nd_state)
        names, dim, scanparam = own_state

        self._parameter_names = names
        self._dim = dim
        self.scanparam = scanparam

    def corners(self):
        """ Return a list containing the corner points of the hypercube """
        import itertools
        return list(itertools.product(*zip(self[:,0], self[:,1])))

