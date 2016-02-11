# -*- python -*-

from professor2.core import *
from professor2.histos import *


def mk_ipolinputs(params):
    """
    Make sorted run and parameter lists suitable for passing to prof.Ipol

    params is a dict (actually, prefer OrderedDict) of run_names -> param_vals,
    as returned from read_rundata
    """
    runs = sorted(params.keys())
    if not runs:
        return runs, [], [[]]
    paramnames = params[runs[0]].keys()
    paramslist = [[params[run][pn] for pn in paramnames] for run in runs]
    return runs, paramnames, paramslist


def mk_ipolhisto(histos, runs, paramslist, order, errmode=None, errorder=None):
    """\
    Make a prof.IpolHisto from a dict of prof.DataHistos and the corresponding
    runs and params lists, at the given polynomial order.

    If errs is non-null, the data histo errors will also be interpolated.

    If errmode is None or 'none', uncertainties will not be parameterised and
    will return 0 if queried; 'mean' and 'median' will use fixed values derived
    from the anchor points; 'symm' will parameterise the average of the + and -
    errors of each bin at the polynomial order given by errorder. If errorder is
    None, the same order as for the value parameterisation will be used.

    Parameter range scaling will be applied, so a DoParamScaling=true flag will
    need to be written to the metadata when persisting the resulting IpolHisto.

    """
    if errmode is None:
        errmode = "none"
    if errorder is None:
        errorder = order
    #
    nbins = len(histos.itervalues().next().bins)
    ibins = []
    for n in xrange(nbins):
        ## Check that the bin edges are consistent and extract their values
        # TODO: move bin edge consistency checking into the Histo base class
        xmins = set([histos[run].bins[n].xmin for run in runs])
        xmaxs = set([histos[run].bins[n].xmax for run in runs])
        assert len(xmins) == len(xmaxs) == 1
        xmin, xmax = xmins.pop(), xmaxs.pop()
        ## Build the value interpolation
        vals = [histos[run].bins[n].val for run in runs]
        valipol = Ipol(paramslist, vals, order)

        # nan check in coeffs
        import math
        if any([math.isnan(x) for x in valipol.coeffs]):
            print "Error: nan coefficient encountered in %s"%histos.values()[0].path
            print "Check inputs or remove from weights"
            import sys
            sys.exit(1)

        ## Build the error interpolation(s)
        if not errmode or errmode == "none":
            erripols = None
        ## Build the error interpolation(s)
        elif errmode == "mean":
            meanerr = sum(histos[run].bins[n].err for run in runs) / float(len(runs))
            erripols = Ipol(paramslist, [meanerr], 0) #< const 0th order interpolation
        elif errmode == "median":
            errs = [histos[run].bins[n].err for run in runs]
            medianerr = errs[len(runs)//2]
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
