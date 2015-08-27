#! /usr/bin/env python
# -*- python -*-

# def scan(*args, **kwargs):
#     sofar = kwargs.get("sofar", "")
#     if args:
#         #print "@", len(args), args
#         sofar += "(" + str(args[0]) + ")"
#         scan(*args[1:], sofar=sofar)
#     else:
#         print "SOFAR", sofar

# scan(1,2,3,'a','b','c')


import numpy as np

# def f(**kwargs):
#     print kwargs
def f(a,b,c):
    print a,b,c

def scan(*args, **kwargs):
    vec = kwargs.get("vec", [])
    if args:
        #print "@", len(args) #, args
        var, npts, low, high = args[0]
        for val in np.linspace(low, high, npts):
            newargs = args[1:]
            newvec = vec + [(var, val)]
            newkwargs = kwargs
            newkwargs["vec"] = newvec
            scan(*newargs, **newkwargs)
    else:
        fn = kwargs.get("fn", None)
        fn(**dict(vec))

scan(["a", 3, 0, 1],
     ["b", 5, 0, 4],
     ["c", 2, 0, 2], fn=f)
