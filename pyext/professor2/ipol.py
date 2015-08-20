# -*- python -*-


# TODO: don't we already have this in the C++ library? We should move it there & map it if not
def min_runs(order, dim):
    """
    Calculate min number of runs (number of coefficients)
    for polynomial of order in dim dimensions.
    """
    # TODO: do we *need* numpy?
    import numpy
    Nc = 1
    for i in xrange(1, order+1):
        t = 1./numpy.math.factorial(i)
        for j in xrange(i):
            t *= (dim+j)
        Nc += t
    return int(Nc)

def mk_ipolinputs(params):
    """
    Make sorted run and parameter lists suitable for passing to prof.Ipol

    params is a dict of run_names -> param_vals, as returned from load_rundata
    """
    runs = sorted(params.keys())
    if not runs:
        return runs, [], [[]]
    paramnames = sorted(params[runs[0]].keys())
    paramslist = [[params[pn] for pn in paramnames] for (run, params) in sorted(params.iteritems())]
    return runs, paramnames, paramslist
