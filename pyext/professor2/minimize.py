# -*- python -*-



def mk_fitfunc(fname, pnames):
    """
    Dynamically make a fit function for the given param names, to be passed to Minuit.

    Return a string definition of the function, to be exec'd, and the list of
    generated internal arg names corresponding to pnames.
    """
    fargs = ["A%03i" % i for i in xrange(len(pnames))]
    funcdef = "def profGoF({fargs}): return {fname}([{fargs}])".format(fargs=", ".join(fargs), fname=fname)
    return funcdef
