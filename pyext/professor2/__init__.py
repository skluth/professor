## Import Cython wrappings on the C++ core library
from professor2.core import *

## Import high-level Python functionality
from professor2.errors import *
from professor2.dataio import *
from professor2.histos import *
from professor2.ipolio import *
from professor2.ipol import *
from professor2.minimize import *
from professor2.paramsio import *
from professor2.params import *
from professor2.sampling import *

# TODO: DEFUNCT & REMOVE?
# from professor2.generator import *
# from professor2.param import *


# TODO: move to a stats submodule, or similar
def pull(dbin, cbin, ppoint=None):
    """
    Pull between databin dbin and comparison bin (cbin).
    If ppoint is None, assume mc bins, otherwise assume ipol bin.

    TODO: "assume XXXX bins" is a nasty design: it would be better to compute the pull on the results list from the val calls
    """
    if dbin.err > 0:
        if ppoint is not None:
            return (dbin.val - cbin.val(ppoint))/dbin.err
        else:
            return (dbin.val - cbin.val)/dbin.err
    else:
        return 0


# TODO: move the functions below to prof2.ui or similar?

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

logo = \
"""
Visit us on http://professor.hepforge.org/
Please cite arXiv:0907.2973 [hep-ph]
                                          %s
 ______           __                            _____ _____
 | ___ \         / _|                          |_   _|_   _|
 | |_/ / __ ___ | |_ ___  ___ ___  ___  _ __     | |   | |
 |  __/ '__/ _ \|  _/ _ \/ __/ __|/ _ \| '__|    | |   | |
 | |  | | | (_) | ||  __/\__ \__ \ (_) | |      _| |_ _| |_
 |_|  |_|  \___/|_| \___||___/___/\___/|_|     |_____|_____|

Andy Buckley, Holger Schulz
Copyright 2015

""" % mk_timestamp()
