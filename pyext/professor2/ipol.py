# -*- python -*-

from __future__ import division
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

def mk_ipolbin(P, V, E, xmin, xmax, order, errmode, errorder):
    valipol = Ipol(P, V, order)
    # nan check in coeffs
    import math
    if any([math.isnan(x) for x in valipol.coeffs]):
        print "Warning: nan coefficient encountered in value ipol for %s"%histos.values()[0].path
        return None

    ## Build the error interpolation(s)
    if not errmode or errmode == "none":
        erripols = None
    ## Build the error interpolation(s)
    elif errmode == "mean":
        meanerr = sum(E) / float(len(E)) #histos[run].bins[binnr].err for run in runs) / float(len(runs))
        erripols = Ipol(P, [meanerr], 0) #< const 0th order interpolation
    elif errmode == "median":
        medianerr = E[len(E)//2]
        erripols = Ipol(P, [medianerr], 0) #< const 0th order interpolation
    elif errmode == "symm":
        erripols = Ipol(P, E, order)
    elif errmode == "asymm":
        raise Exception("Error interpolation mode 'asymm' not yet supported")
        # errs0 = [histos[run].bins[n].errs[0] for run in runs]
        # erripol0 = Ipol(paramslist, errs0, order)
        # errs1 = [histos[run].bins[n].errs[1] for run in runs]
        # erripol1 = Ipol(paramslist, errs1, order)
        # erripols = [erripol0, erripol1]
    else:
        raise Exception("Unknown error interpolation mode '%s'" % errmode)
    if erripols is not None:
        if any([math.isnan(x) for x in erripols.coeffs]):
            print "Warning: nan coefficient encountered in error ipol for %s"%histos.values()[0].path
            return None
    return IpolBin(xmin, xmax, valipol, erripols)

# Keep this for backward compatibility 
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
        xmax = histos.values()[0].bins[n].xmax
        xmin = histos.values()[0].bins[n].xmin
        vals = [histos[run].bins[n].val for run in runs]
        errs = [histos[run].bins[n].err for run in runs]
        ibins.append(mk_ipolbin(paramslist, vals, errs, xmin, xmax, order, errmode, errorder))
    return Histo(ibins, histos.values()[0].path)



def mkStandardIpols(HISTOS, HNAMES, RUNS, PARAMSLIST, CFG):

    BNAMES = []
    for hn in HNAMES:
        histos = HISTOS[hn]
        nbins = histos.values()[0].nbins
        for n in xrange(nbins):
            BNAMES.append([hn, n])

    NBINS=len(BNAMES)

    MSGEVERY = int(NBINS/100.);

    import sys
    import professor2 as prof
    def worker(q, rdict, counter):
        "Function to make bin ipols and store ipol persistency strings for each histo"
        while True:
            if q.empty():
                break
            temp = q.get()
            hn=temp[0]
            histos = HISTOS[hn]
            n = temp[1]
            xmax = histos.values()[0].bins[n].xmax
            xmin = histos.values()[0].bins[n].xmin
            vals = [histos[run].bins[n].val for run in RUNS]
            errs = [histos[run].bins[n].err for run in RUNS]
            ib = prof.mk_ipolbin(PARAMSLIST, vals, errs, xmin, xmax, CFG["ORDER"], CFG["IERR"], CFG["ERR_ORDER"])
            if ib is None:
                print "Ignoring", hn, "Bin number", n
            else:
                s = ""
                s += "%s#%d %.5e %.5e\n" % (hn, n, ib.xmin, ib.xmax)
                s += "  " + ib.ival.toString("val") + "\n"
                if ib.ierrs:
                    s += "  " + ib.ierrs.toString("err") + "\n"
                rdict[(hn,n)] = s
                del s
            del ib #< pro-actively clear up memory
            counter.value+=1
            if counter.value==MSGEVERY:
                counter.value=0
                sys.stderr.write('\rProgress: {0:.1%}'.format(len(rdict.keys())/NBINS))



    print "\n\nParametrising %i objects...\n"%len(BNAMES)
    import time, multiprocessing
    # time1 = time.time()

    ## A shared memory object is required for coefficient retrieval
    from multiprocessing import Manager, Value
    manager = Manager()
    tempDict = manager.dict()

    # This for the status --- modululs is too expensive
    ndone=Value('i', 0)

    ## The job queue
    q = multiprocessing.Queue()

    map(lambda x:q.put(x), BNAMES)



    ## Fire away
    workers = [multiprocessing.Process(target=worker, args=(q, tempDict, ndone)) for i in xrange(CFG["MULTI"])]
    map(lambda x:x.start(), workers)
    map(lambda x:x.join(),  workers)

    # ## Timing
    # time2 = time.time()
    # sys.stderr.write('\rParametrisation took %0.2fs.\nWriting output...' % ((time2-time1)))

    return tempDict
