# -*- python -*-

from professor2.errors import *
from professor2.ipol import *
from professor2.histos import *


def read_ipolmeta(ifile):
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
                key, value = map(str.strip, l.split(":",1))
                meta[key] = value
            except:
                print "Couldn't extract key-value pair from '%s'" % l
    return meta


def read_simpleipols(ifile):
    """
    Read ipol data back in from ifile
    """
    IOBJECTS = {}
    with open(ifile, "r") as f:
        name = ""
        for line in f:
            sline = line.strip()
            if sline.startswith("/"):
                name = sline.split()[0]
            elif sline.startswith("val"):
                IOBJECTS[name]= Ipol(sline)
    return IOBJECTS


def read_binnedipols(ifile):
    """
    Read binned ipol data back in from ifile
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
                #print currentib.ival.coeffs()
            elif sline.startswith("err"):
                currentib.ierrs = Ipol(sline)
                #print currentib.ierrs.coeffs()
            # TODO: read back asymm errs as two ipols
    return IHISTOS


def read_ipolhistos(ifile):
    "Return both the metadata object and collection of IpolHistos from a binned ipol file"
    imeta = read_ipolmeta(ifile)
    if not imeta["DataFormat"].startswith('binned'):
        raise IpolIOError("Error, DataFormat of ipol file %s is not binned" % ifile)
    return imeta, read_binnedipols(ifile)
