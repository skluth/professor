# -*- python -*-

from professor2.histos import *
from professor2.paramsio import *


def read_histos_root(path):
    "Load histograms from a ROOT file, into a dict of path -> yoda.Histo[DataBin]"
    histos = {}

    # TODO: Could just use YODA for everything, including ROOT reading?
    try:
        import ROOT
        ROOT.gROOT.SetBatch(True)
    except ImportError:
        print "PyROOT not available... skipping"
        return histos

    # TODO: use yoda.root.getall
    def _getallrootobjs(d, basepath="/"):
        "Recurse through a ROOT file/dir and generate (path, obj) pairs"
        for key in d.GetListOfKeys():
            kname = key.GetName()
            if key.IsFolder():
                # TODO: -> "yield from" in Py3
                for i in _getallrootobjs(d.Get(kname), basepath+kname+"/"):
                    yield i
            else:
                yield basepath+kname, d.Get(kname)
    try:
        f = ROOT.TFile(path)
        for rname, robj in _getallrootobjs(f):
            bins = []
            if robj.InheritsFrom("TH1"):
                # TODO: allow 2D histos
                if robj.InheritsFrom("TH2"):
                    continue
                for ib in xrange(robj.GetNbinsX()):
                    xmin = robj.GetXaxis().GetBinLowEdge(ib+1)
                    xmax = robj.GetXaxis().GetBinUpEdge(ib+1)
                    y = robj.GetBinContent(ib+1)
                    ey = robj.GetBinError(ib+1)
                    bins.append(DataBin(xmin, xmax, y, ey))
                histos[rname] = Histo(bins, rname)
            elif robj.InheritsFrom("TGraph"):
                for ip in xrange(robj.GetN()):
                    x, y = ROOT.Double(), ROOT.Double()
                    robj.GetPoint(ip, x, y)
                    xmin = x - robj.GetErrorXlow(ip)
                    xmax = x + robj.GetErrorXhigh(ip)
                    ey = (robj.GetErrorXlow(ip) + robj.GetErrorXhigh(ip)) / 2.0
                    bins.append(DataBin(xmin, xmax, y, ey))
            histos[rname] = Histo(bins, rname)
        f.Close()
    except Exception, e:
        print "Can't load histos from ROOT file '%s': %s" % (path, e)

    return histos


def read_histos_yoda(path):
    "Load histograms from a YODA-supported file type, into a dict of path -> yoda.Histo[DataBin]"
    histos = {}
    try:
        import yoda
        s2s = []
        aos = yoda.read(path, asdict=False)
        for ao in aos:
            import os
            ## Skip the Rivet cross-section and event counter objects
            # TODO: Avoid Rivet-specific behaviour by try block handling & scatter.dim requirements
            if os.path.basename(ao.path).startswith("_"):
                continue
            ##
            s2s.append(ao.mkScatter())
        del aos #< pro-active YODA memory clean-up
        #
        #s2s = [ao.mkScatter() for ao in yoda.read(path, asdict=False)]
        for s2 in s2s:
            bins = [DataBin(p.xMin, p.xMax, p.y, p.yErrAvg) for p in s2.points]
            #bins = [DataBin(p.xMin, p.xMax, p.y*(p.xMax-p.xMin), p.yErrAvg) for p in s2.points]
            # bins = [DataBin(p.xMin, p.xMax, p.y, p.yErrs) for p in s2.points]
            histos[s2.path] = Histo(bins, s2.path)
        del s2s #< pro-active YODA memory clean-up
    except Exception, e:
        print "Can't load histos from file '%s': %s" % (path, e)
    return histos


def read_histos(path):
    "Load histograms from file, into a dict of path -> yoda.Histo[DataBin]"
    histos = {}
    if path.endswith(".root"):
        histos.update(read_histos_root(path))
    elif any(path.endswith(ext) for ext in [".yoda", ".aida", ".flat"]):
        histos.update(read_histos_yoda(path))
    return histos


def read_rundata(dirs, pfname="params.dat", verbosity=1): #, formats="yoda,root,aida,flat"):
    """
    Read interpolation anchor point data from a provided set of run directory paths.

    Returns a pair of dicts, the first mapping run names (i.e. rundir basenames) to
    the parameter value list for each run, and the second mapping observable names
    (i.e. histogram paths) to a run -> histo dict.
    """
    params, histos = {}, {}
    import os, glob, re
    re_pfname = re.compile(pfname) if pfname else None
    numruns = len(dirs)
    for num, d in enumerate(sorted(dirs)):
        run = os.path.basename(d)
        if verbosity >= 2 or (verbosity >= 1 and (num % 100 == 0 or num == numruns)):
            pct = 100*num/float(numruns)
            print "Reading run '%s' data: %d/%d = %2.0f%%" % (run, num, numruns, pct)
        files = glob.glob(os.path.join(d, "*"))
        for f in files:
            ## Params file
            #if os.path.basename(f) == pfname:
            if re_pfname and re_pfname.search(os.path.basename(f)):
                params[run] = read_paramsfile(f)
            ## Histo file
            else:
                try:
                    ## Read as a path -> Histo dict
                    hs = read_histos(f)
                    ## Restructure into the path -> run -> Histo return dict
                    for path, hist in hs.iteritems():
                        histos.setdefault(path, {})[run] = hist
                except:
                    pass #< skip files that can't be read as histos

        ## Check that a params file was found and read in this dir... or that no attempt was made to find one
        if pfname:
            if run not in params.keys():
                raise Exception("No params file '%s' found in run dir '%s'" % (pfname, d))
        else:
            params = None
    return params, histos


def find_maxerrs(histos):
    """
    Helper function to find the maximum error values found for each bin in the histos double-dict.

    histos is a nested dict of DataHisto objects, indexed first by histo path and then by
    run name, i.e. it is the second of the two objects returned by read_histos().

    Returns a dict of lists of floats, indexed by histo path. Each list of floats contains the
    max error size seen for each bin of the named observable, across the collection of runs
    histos.keys().

    This functions is useful for regularising chi2 etc. computation by constraining interpolated
    uncertainties to within the range seen in the run data used to create the ipols, to avoid
    blow-up of uncertainties with corresponding distortion of fit optimisation.

    TODO: asymm version?
    """
    rtn = {}
    for hn, hs in histos.iteritems():
        numbins_h = hs.next().nbins
        maxerrs_h = []
        for ib in xrange(numbins_h):
            emax = max(h.bins[ib].err for h in hs.values())
            maxerrs_h.append(emax)
        rtn[hn] = maxerrs_h
    return rtn
