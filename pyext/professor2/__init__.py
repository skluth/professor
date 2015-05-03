from professor2.core import *


# TODO: move this stuff into submodules:

class Histo(object):

    def __init__(self, bins=None):
        self.bins = bins if bins else []


class DataBin(object):

    def __init__(self, xmin, xmax, val=None, err=None):
        self.xmin = xmin
        self.xmax = xmax
        self.val = val
        self.err = err

    def __cmp__(self, other):
        return cmp(self.xmin, other.xmin)


class IpolBin(object):

    def __init__(self, xmin, xmax, ival=None, ierr=None):
        self.xmin = xmin
        self.xmax = xmax
        self.ival = ival
        self.ierr = ierr

    def val(self, params):
        return self.ival.value(params)

    def err(self, params):
        return self.ierr.value(params)

    def __cmp__(self, other):
        return cmp(self.xmin, other.xmin)


def read_paramsfile(path):
    rtn = {}
    with open(path, "r") as f:
        for line in f:
            parts = line.split("#")[0].strip().split()
            assert len(parts) == 2
            rtn[parts[0]] = float(parts[1])
    return rtn

def read_histos(path):
    "Load histograms from file, into a dict of path -> yoda.Histo[DataBin]"
    histos = {}
    if path.endswith(".root"):
        ## Try ROOT
        try:
            import ROOT
            raise Exception("Can't yet read ROOT format files!")
        except:
            print "Can't yet read ROOT format files... TODO!"
    else:
        ## Try YODA (handles .yoda, .aida, and .flat)
        try:
            import yoda
            s2s = [ao.mkScatter() for ao in yoda.read(yf, asdict=False)]
            for s2 in s2s:
                bins = [DataBin(p.xMin, p.xMax, p.y) for p in s2.points]
                histos[s2.path] = Histo(bins)
        except:
            print "Can't load histos from file " + path

def load_rundata(dirs, pfname="params.dat"): #, formats="yoda,root,aida,flat"):
    params, histos = {}, {}
    for d in dirs:
        run = os.path.basename(d)
        files = glob.glob(os.path.join(d, "*"))
        for f in files:
            ## Params file
            if os.path.basename(f) == pfname:
                params[run] = read_paramsfile(pf)
            ## Histo file
            else:
                ## Read as a path -> Histo dict
                hs = read_histos(f)
                ## Restructure into the path -> run -> Histo return dict
                for path, hist in hs.iteritems():
                    histos.setdefault(path, {})[run] = hist
        ## Check that a params file was found and read in this dir
        if run not in params.keys():
            raise Exception("No params file found in run dir " + d)
    return params, histos

def mk_ipolinputs(params):
    "Make sorted run and parameter lists suitable for passing to prof.Ipol"
    runs = sorted(params.keys())
    paramnames = sorted(params[runs[0]].keys())
    paramslist = [[params[pn] for pn in paramnames] for (run, params) in sorted(params.iteritems())]
    return runs, paramnames, paramslist

def mk_ipolhisto(histos, runs, paramslist, order, errs=None):
    """\
    Make a prof.Histo filled with prof.IpolBins, from a dict of prof.DataBin
    histos and the corresponding runs and params lists, at the given polynomial order.

    If errs is non-null, the data histo errors will also be interpolated.
    """
    nbins = len(histos.itervalues().next().points)
    ibins = []
    for n in xrange(nbins):
        ## Check that the bin edges are consistent and extract their values
        xmins = set([histos[run].bins[n].xmin for run in runs])
        xmaxs = set([histos[run].bins[n].xmax for run in runs])
        assert len(xmins) == len(xmaxs) == 1
        xmin, xmax = xmins.pop(), xmaxs.pop()
        ## Build the value interpolation
        vals = [histos[run].bins[n].val for run in runs] #< to guarantee correct order w.r.t. paramslist
        valipol = prof.Ipol(paramslist, vals, order)
        ## Build the (average-only, for now) error interpolation
        errs = [histos[run].bins[n].err for run in runs] #< to guarantee correct order w.r.t. paramslist
        # TODO: what if there are (some) real zero-valued errors?
        erripol = prof.Ipol(paramslist, errs, order) if all(errs) else None
        ibins.append(IpolBin(xmin, xmax, valipol, erripol))
    return IpolHisto(bins)
