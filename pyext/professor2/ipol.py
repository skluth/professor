# -*- python -*-

from professor2.core import *
from professor2.histos import *


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


def mk_ipolhisto(histos, runs, paramslist, order, errmode="none"):
    """\
    Make a prof.Histo filled with prof.IpolBins, from a dict of prof.DataBin
    histos and the corresponding runs and params lists, at the given polynomial order.

    If errs is non-null, the data histo errors will also be interpolated.

    TODO: add relative versions of the error interpolation
    """
    nbins = len(histos.itervalues().next().bins)
    ibins = []
    for n in xrange(nbins):
        ## Check that the bin edges are consistent and extract their values
        xmins = set([histos[run].bins[n].xmin for run in runs])
        xmaxs = set([histos[run].bins[n].xmax for run in runs])
        assert len(xmins) == len(xmaxs) == 1
        xmin, xmax = xmins.pop(), xmaxs.pop()
        ## Build the value interpolation
        vals = [histos[run].bins[n].val for run in runs]
        valipol = Ipol(paramslist, vals, order)
        ## Build the error interpolation(s)
        if not errmode or errmode == "none":
            erripols = None
        ## Build the error interpolation(s)
        elif errmode == "mean":
            meanerr = sum(histos[run].bins[n].err for run in runs) / float(nbins)
            erripols = Ipol(paramslist, [meanerr], 0) #< const 0th order interpolation
        elif errmode == "median":
            medianerr = [histos[run].bins[n].err for run in runs][nbins//2]
            erripols = Ipol(paramslist, [medianerr], 0) #< const 0th order interpolation
        elif errmode == "symm":
            errs = [histos[run].bins[n].err for run in runs]
            erripols = Ipol(paramslist, errs, order)
        elif errmode == "asymm":
            raise Exception("Error interpolation mode 'asymm' not yet supported")
            # errs0 = [histos[run].bins[n].errs[0] for run in runs]
            # erripol0 = Ipol(paramslist, errs0, order)
            # errs1 = [histos[run].bins[n].errs[1] for run in runs]
            # erripol1 = Ipol(paramslist, errs1, order)
            # erripols = [erripol0, erripol1]
        else:
            raise Exception("Unknown error interpolation mode '%s'" % errmode)
        ibins.append(IpolBin(xmin, xmax, valipol, erripols))
    return Histo(ibins, histos.values()[0].path)
