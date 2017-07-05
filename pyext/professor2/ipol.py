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

def mk_ipolbin(P, V, E, xmin, xmax, order, errmode, errorder):
    valipol = Ipol(P, V, order)
    # nan check in coeffs
    import math
    if any([math.isnan(x) for x in valipol.coeffs]):
        print "Warning: nan coefficient encountered in value ipol"
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
            print "Warning: nan coefficient encountered in error ipol"
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
        if ibins[-1] is None:
            print "in bin %i of %s"%(n, histos.values()[0].path)
    return Histo(ibins, histos.values()[0].path)

# https://stackoverflow.com/questions/2130016/splitting-a-list-of-into-n-parts-of-approximately-equal-length
def chunkIt(seq, num):
  avg = len(seq) / float(num)
  out = []
  last = 0.0

  while last < len(seq):
    out.append(seq[int(last):int(last + avg)])
    last += avg

  return out

def mkStandardIpols(HISTOS, HNAMES, RUNS, PARAMSLIST, CFG, nchunks=10):

    BNAMES = []
    for hn in HNAMES:
        histos = HISTOS[hn]
        nbins = histos.values()[0].nbins
        for n in xrange(nbins):
            BNAMES.append([hn, n])

    NBINS=len(BNAMES)

    MSGEVERY = int(NBINS/100.);

    import sys, zlib
    import professor2 as prof
    def worker(q, rdict, counter):
        "Function to make bin ipols and store ipol persistency strings for each histo"
        import sys
        while True:
            if q.empty():
                break
            try:
                temp = q.get(False)
            except:
                break

            hn=temp[0]
            histos = HISTOS[hn]
            n = temp[1]
            xmax = histos.values()[0].bins[n].xmax
            xmin = histos.values()[0].bins[n].xmin
            vals = [histos[run].bins[n].val for run in RUNS]
            errs = [histos[run].bins[n].err for run in RUNS]
            ib = prof.mk_ipolbin(PARAMSLIST, vals, errs, xmin, xmax, CFG["ORDER"], CFG["IERR"], CFG["ERR_ORDER"])
            if ib is not None:
                s = ""
                s += "%s#%d %.5e %.5e\n" % (hn, n, ib.xmin, ib.xmax)
                s += "  " + ib.ival.toString("val") + "\n"
                if ib.ierrs:
                    s += "  " + ib.ierrs.toString("err") + "\n"
                rdict.put( [hn,n, zlib.compress(s, 9)])
                del s
            else:
                print "in bin %i of %s"%(n, histos.values()[0].path)
            del ib #< pro-actively clear up memory
            del histos
            counter.value+=1
            if counter.value==MSGEVERY:
                counter.value=0
                sys.stderr.write('\rProgress: {current}/{total}'.format(current=rdict.qsize(), total=NBINS))
            q.task_done()
        return



    rDict={}
    print "\n\nParametrising %i objects...\n"%len(BNAMES)
    import time
    time1 = time.time()

    from multiprocessing import Manager, Process
    manager = Manager()

    # This for the status --- modulus is too expensive
    ndone=manager.Value('i', 0)
    ## A shared memory object is required for coefficient retrieval
    r = manager.Queue()
    for chunk in chunkIt(BNAMES, nchunks): # The chunking is necessary as the memory blows up otherwise


        ## The job queue
        q = manager.Queue()

        ## Fire away
        workers = [Process(target=worker, args=(q, r, ndone)) for i in xrange(CFG["MULTI"])]
        map(lambda x:q.put(x), chunk)
        map(lambda x:x.start(), workers)

        map(lambda x:x.join(),  workers)
        map(lambda x:x.terminate(),  workers)

        # ## Timing
    while not r.empty():
        a,b,c = r.get()
        rDict[(a,b)] =c

    time2 = time.time()
    sys.stderr.write('\rParametrisation took %0.2fs.\nWriting output...' % ((time2-time1)))

    return rDict

def writeIpol(fname, ipolDict, params, runs=[], summary="", runsdir=""):
    PARAMNAMES = params[0]
    PARAMSLIST = params[1]

    import os, tempfile, zlib
    if fname=="temp":
        f=tempfile.NamedTemporaryFile(delete=False)
    else:
        f=open(fname, "w")

    import professor2 as prof
    f.write("Summary: %s\n" % summary)
    f.write("DataDir: %s\n" % os.path.abspath(runsdir))
    f.write("ProfVersion: %s\n" % prof.version())
    f.write("Date: %s\n" % prof.mk_timestamp())
    f.write("DataFormat: binned 2\n") # This tells the reader how to treat the coefficients that follow
    # Format and write out parameter names
    pstring = "ParamNames:"
    for p in PARAMNAMES:
        pstring += " %s" % p
    f.write(pstring + "\n")
    # Dimension (consistency check)
    f.write("Dimension: %i\n" % len(PARAMNAMES))
    # Interpolation validity (hypercube edges)
    minstring = "MinParamVals:"
    for v in prof.mk_minvals(PARAMSLIST):
        minstring += " %f" % v
    f.write(minstring + "\n")
    maxstring = "MaxParamVals:"
    for v in prof.mk_maxvals(PARAMSLIST):
        maxstring += " %f" % v
    f.write(maxstring + "\n")
    f.write("DoParamScaling: 1\n")
    # Number of inputs per bin
    f.write("NumInputs: %i\n" % len(PARAMSLIST))
    s_runs = "Runs:"
    for r in runs:
        s_runs +=" %s"%r
    f.write("%s\n"%s_runs)
    f.write("---\n")

    ## Write out numerical data for all interpolations
    s = ""
    HNAMES=sorted(list(set([x[0] for x in ipolDict.keys()])))
    for hn in sorted(HNAMES):
        thisbins=sorted(filter(lambda x: x[0]==hn, ipolDict.keys()))
        for ipolstring in [ipolDict[x] for x in thisbins]:
            s+=zlib.decompress(ipolstring)

    f.write(s)
    f.close()
    if not fname=="temp":
        print "\nOutput written to %s" % fname
    else:
        return f
