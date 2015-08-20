# -*- python -*-

## Provide faff-free Minuit objects
Minuit, MinuitError = None, Exception
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


def mk_fitfunc(fname, pnames):
    """
    Dynamically make a fit function for the given param names, to be passed to Minuit.
    """
    funcargs = ", ".join(pnames)
    funcdef = "def profGoF("
    funcdef += funcargs
    funcdef += "): "
    funcdef += "return %s(["%fname
    funcdef += funcargs
    funcdef += "])"
    return funcdef
