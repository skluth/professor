from professor2.core import *


## Provide faff-free Minuit objects
Minuit, MinuitError = None, None
try:
    from iminuit import Minuit, MinuitError
except ImportError:
    try:
        from minuit import Minuit, MinuitError
    except ImportError:
        try:
            from minuit2 import Minuit2 as Minuit
            from minuit2 import MinuitError
        except ImportError:
            pass #print "Couldn't import a minimizer"


# TODO: move this stuff into submodules:


## Define a sampler type
class Sampler(object):

    def __init__(self, low, high, bias=None):
        self.low = float(low)
        self.high = float(high)
        self.f, self.invf = None, None
        if bias:
            ## Import clever machinery
            try:
                import sympy as sp
                from sympy.abc import x, y
                import numpy as np
            except ImportError:
                print "Bias functions require SymPy and NumPy to be installed... exiting"
                exit(1) #< TODO: don't exit from inside a lib function...
            ## Make transformation and its inverse
            try:
                #print bias
                f_expr = sp.sympify(bias)
            except sp.SympifyError, e:
                print "Bias function could not be parsed by SymPy:"
                print e
                exit(1) #< TODO: don't exit from inside a lib function...
            try:
                finv_exprs = sp.solve(sp.Eq(y, f_expr), x)
                finv_expr = finv_exprs[0]
                #print f_expr, finv_exprs
                self.f = sp.lambdify(x, f_expr, "numpy")
                self.finv = sp.lambdify(y, finv_expr, "numpy")
                self.lowf, self.highf = self.f(self.low), self.f(self.high)
            except Exception, e:
                print "Bias function could not be used/inverted by SymPy:"
                print e
                exit(1) #< TODO: don't exit from inside a lib function...

    def shoot(self):
        import random
        if not self.f:
            ## Just uniform sampling between low..high
            val = random.uniform(self.low, self.high)
        else:
            ## Uniform sample in transformed space, and transform the result back
            valf = random.uniform(self.lowf, self.highf)
            val = self.finv(valf)
        return val

    def __call__(self):
        return self.shoot()

## Test biased sampler machinery
# s = Sampler(1, 10, "exp(x)")
# import yoda
# h = yoda.Histo1D(20, 0, 10)
# for _ in xrange(10000):
#     h.fill( s() )
# yoda.plot(h, "foo.pdf")


class Histo(object):
    "A simple histogram -- just a Bin container with an optional path name"

    def __init__(self, bins=None, path=None):
        self.bins = bins if bins else []
        self.path = path


class Bin(object):
    "A base class for binned data, handling the common x-edge stuff"

    def __init__(self, xmin, xmax):
        self.xmin = xmin
        self.xmax = xmax

    @property
    def xmid(self):
        return (self.xmin + self.xmax) / 2.0

    @property
    def xedges(self):
        return (self.xmin, self.xmax)

    def __cmp__(self, other):
        return cmp(self.xmin, other.xmin)


class DataBin(Bin):
    "A bin containing a data value and its error(s)"

    def __init__(self, xmin, xmax, val=None, errs=None):
        Bin.__init__(self, xmin, xmax)
        self.val = val
        self._errs = errs

    # TODO: return numerical 0 if _errs is None?
    @property
    def err(self):
        "Get a scalar error value, by averaging if necessary"
        if self._errs is None:
            return 0.0
        elif hasattr(self._errs, "__len__"):
            assert len(self._errs) == 2
            return sum(self._errs) / 2.0
        return self._errs
    @err.setter
    def err(self, e):
        "Set a scalar error value"
        assert not hasattr(self._errs, "__len__")
        self._errs = e

    @property
    def errs(self):
        "Get a pair of error values, by construction if necessary"
        if self._errs is None:
            return (0.0, 0.0)
        elif hasattr(self._errs, "__len__"):
            assert len(self._errs) == 2
            return self._errs
        return (self._errs, self._errs)
    @errs.setter
    def errs(self, e):
        "Set a pair of error values"
        if e is None:
            self._errs = None
        elif hasattr(e, "__len__"):
            assert len(e) == 2
            self._errs = e
        else:
            self._errs = [e,e]


class IpolBin(Bin):
    "A bin containing a value interpolation and its error(s)"

    def __init__(self, xmin, xmax, ival=None, ierrs=None):
        Bin.__init__(self, xmin, xmax)
        self.ival = ival
        self.ierrs = ierrs

    def val(self, params):
        return self.ival.value(params)

    # TODO: Provide ierr and ierrss getter/setter pairs cf. err/errs on DataBin? They can't be averaged, so not sure it makes sense...

    def err(self, params):
        if self.ierrs is None:
            return 0.0
        elif hasattr(self.ierrs, "__len__"):
            assert len(self.ierrs) == 2
            return (self.ierrs[0].value(params) + self.ierrs[1].value(params))/2.0
        else:
            return self.ierrs.value(params)

    def errs(self, params):
        if self.ierrs is None:
            return (0.0, 0.0)
        elif hasattr(self.ierrs, "__len__"):
            assert len(self.ierrs) == 2
            return (self.ierrs[0].value(params), self.ierrs[1].value(params))
        else:
            e = self.ierrs.value(params)
            return (e, e)


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
            s2s = [ao.mkScatter() for ao in yoda.read(path, asdict=False)]
            for s2 in s2s:
                bins = [DataBin(p.xMin, p.xMax, p.y, p.yErrAvg) for p in s2.points]
                # bins = [DataBin(p.xMin, p.xMax, p.y, p.yErrs) for p in s2.points]
                histos[s2.path] = Histo(bins, s2.path)
        except Exception, e:
            print "Can't load histos from file '%s': %s" % (path, e)
    return histos


def load_rundata(dirs, pfname="params.dat", debug=False): #, formats="yoda,root,aida,flat"):
    params, histos = {}, {}
    import os, glob
    for num, d in enumerate(dirs):
        run = os.path.basename(d)
        files = glob.glob(os.path.join(d, "*"))
        if debug:
            print "Reading from %s"%run
        else:
            print "\r%.1f per cent read"%((float(num+1)/len(dirs))*100),
        for f in files:
            ## Params file
            if os.path.basename(f) == pfname:
                params[run] = read_paramsfile(f)
            ## Histo file
            else:
                ## Read as a path -> Histo dict
                hs = read_histos(f)
                ## Restructure into the path -> run -> Histo return dict
                for path, hist in hs.iteritems():
                    histos.setdefault(path, {})[run] = hist
        ## Check that a params file was found and read in this dir
        if run not in params.keys():
            raise Exception("No params file '%s' found in run dir '%s'" % (pfname, d))
    return params, histos


def mk_ipolinputs(params):
    "Make sorted run and parameter lists suitable for passing to prof.Ipol"
    runs = sorted(params.keys())
    paramnames = sorted(params[runs[0]].keys())
    paramslist = [[params[pn] for pn in paramnames] for (run, params) in sorted(params.iteritems())]
    return runs, paramnames, paramslist


def mk_ipolhisto(histos, runs, paramslist, order, errmode="none"):
    """\
    Make a prof.Histo filled with prof.IpolBins, from a dict of prof.DataBin
    histos and the corresponding runs and params lists, at the given polynomial order.

    If errs is non-null, the data histo errors will also be interpolated.
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
            erripols = Ipol(paramslist, [meanerr for run in runs], 1)
        elif errmode == "median":
            medianerr = [histos[run].bins[n].err for run in runs][nbins//2]
            erripols = Ipol(paramslist, [medianerr for run in runs], 1)
        elif errmode == "symm":
            errs = [histos[run].bins[n].err for run in runs]
            erripols = Ipol(paramslist, errs, order)
        elif errmode == "asymm":
            # raise Exception("Error interpolation mode 'asymm' not yet supported")
            errs0 = [histos[run].bins[n].errs[0] for run in runs]
            erripol0 = Ipol(paramslist, errs0, order)
            errs1 = [histos[run].bins[n].errs[1] for run in runs]
            erripol1 = Ipol(paramslist, errs1, order)
            erripols = [erripol0, erripol1]
        else:
            raise Exception("Unknown error interpolation mode '%s'" % errmode)
        ibins.append(IpolBin(xmin, xmax, valipol, erripols))
    return Histo(ibins, histos.values()[0].path)


def mk_timestamp():
    """
    Time stamp, taken from http://stackoverflow.com/questions/13890935/timestamp-python
    """
    import time
    ts = time.time()
    import datetime
    st = datetime.datetime.fromtimestamp(ts).strftime('%Y-%m-%d %H:%M:%S')
    return st

def mk_versionstring():
    """
    Get version from setup.py, courtesy of
    http://stackoverflow.com/questions/2058802/how-can-i-get-the-version-defined-in-setup-py-setuptools-in-my-package
    """
    import pkg_resources  # part of setuptools
    return pkg_resources.require("professor2")[0].version

sequels = [
"""
                    Interpolation day
""",
"""
                    Professor Rebinned
""",
"""
                      2 Pro 2 Fessor
""",
"""
                      Interpolations
""",
"""
                    Professor Returns
""",
"""
                       The Dark Bin
""",
"""
                   Some Code has Survived
""",
"""
               Professor and the chi-2 of doom
""",
"""
                The Minimisation strikes back
""",
"""
                     The Bin Supremacy
""",
]

logo = \
"""
 ______           __                             _____ _____
 | ___ \         / _|                           |_   _|_   _|
 | |_/ / __ ___ | |_ ___  ___ ___  ___  _ __      | |   | |
 |  __/ '__/ _ \|  _/ _ \/ __/ __|/ _ \| '__|     | |   | |
 | |  | | | (_) | ||  __/\__ \__ \ (_) | |       _| |_ _| |_
 \_|  |_|  \___/|_| \___||___/___/\___/|_|       \___/ \___/

"""

from random import randint
logo+=sequels[randint(0,len(sequels)-1)]

logo+="""

Andy Yaml Buckley
Holger Schulz
Copyright MMXV

"""

