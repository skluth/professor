# -*- python -*-

from professor2.errors import *
from professor2.ipol import *
from professor2.histos import *


class IpolMeta(dict):

    def __init__(self, ifile=None):
        self.pnames = None
        if ifile:
            self.update(self.read_ipolmeta(ifile))
            self.pnames = self["ParamNames"].split()

    def read_ipolmeta(self, ifile):
        """
        Read in meta data from prof-ipol output 'ifile'
        """
        meta = {}
        with open(ifile) as f:
            for l in f:
                ## Strip out comments
                if "#" in l:
                    l = l[:l.find("#")]
                ## Ignore blank / pure whitespace lines
                l = l.strip()
                if not l:
                    continue
                ## Exit if we see the end-of-header indicator
                if l == "---":
                    break
                ## Extract the key-value pair from the line
                try:
                    key, value = [str.strip(s) for s in l.split(":", 1)]
                    meta[key] = value
                except:
                    print "Couldn't extract key-value pair from '%s'" % l
        return meta


def read_ipolmeta(ifile):
    return IpolMeta(ifile)


def read_simpleipols(ifile, paramlimits=None):
    """
    Read ipol data back in from ifile.

    If the paramlimits argument is non-null, it will be used internally by
    the Ipol objects to stabilise the SVD calculation. For this to make sense,
    the persisted ipols must have been created with the same scaling factors.
    paramlimits should be a 2-tuple of lists for min and max param values
    respectively.
    """
    IOBJECTS = {}
    with open(ifile, "r") as f:
        name = ""
        for line in f:
            sline = line.strip()
            if sline.startswith("/"):
                name = sline.split()[0]
            elif sline.startswith("val"):
                IOBJECTS[name] = Ipol(sline)
                if paramlimits:
                    IOBJECTS[name].setParamLimits(*paramlimits)
    return IOBJECTS


def read_binnedipols(ifile, paramlimits=None):
    """
    Read binned ipol data back in from ifile.

    If the paramlimits argument is non-null, it will be used internally by
    the Ipol objects to stabilise the SVD calculation. For this to make sense,
    the persisted ipols must have been created with the same scaling factors.
    paramlimits should be a 2-tuple of lists for min and max param values
    respectively.
    """
    IHISTOS = {}
    with open(ifile, "r") as f:
        for line in f:
            sline = line.strip()
            if sline.startswith("/"):
                fullpath, sxmin, sxmax = sline.split()
                hpath, nbin = fullpath.split("#")
                currentib = IpolBin(float(sxmin), float(sxmax))
                IHISTOS.setdefault(hpath, IpolHisto(path=hpath)).bins.append(currentib)
            elif sline.startswith("val"):
                currentib.ival = Ipol(sline)
                if paramlimits:
                    currentib.ival.setParamLimits(*paramlimits)
                #print currentib.ival.coeffs()
            elif sline.startswith("err"):
                currentib.ierrs = Ipol(sline)
                if paramlimits:
                    currentib.ierrs.setParamLimits(*paramlimits)
                #print currentib.ierrs.coeffs()
            # TODO: read back asymm errs as two ipols
    return IHISTOS


def read_ipoldata(ifile):
    "Return both the metadata object and collection of IpolHistos from a binned ipol file"
    imeta = read_ipolmeta(ifile)
    if not imeta["DataFormat"].startswith('binned'):
        raise IpolIOError("Error, DataFormat of ipol file %s is not binned" % ifile)
    paramlimits = None
    if bool(int(imeta.get("DoParamScaling", 0))):
        assert imeta.has_key("MinParamVals") and imeta.has_key("MaxParamVals")
        minparamvals = [float(s) for s in imeta["MinParamVals"].split()]
        maxparamvals = [float(s) for s in imeta["MaxParamVals"].split()]
        paramlimits = (minparamvals, maxparamvals)
    return read_binnedipols(ifile, paramlimits), imeta

# TODO: Backward compatibility alias: remove
read_ipolhistos = read_ipoldata
