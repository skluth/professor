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
    return a**2 + 0.5*b**2 - c

def scan(*args, **kwargs):
    vec = kwargs.get("vec", [])
    if args:
        # print "@", len(args) #, args
        var, npts, low, high = args[0]
        for val in np.linspace(low, high, npts):
            newargs = args[1:]
            newvec = vec + [(var, val)]
            newkwargs = kwargs
            newkwargs["vec"] = newvec
            for i in scan(*newargs, **newkwargs):
                yield i
            # yield from scan(*newargs, **newkwargs):
    else:
        # print vec
        yield vec
        # yield dict(vec)
        # fn = kwargs.get("fn", None)
        # fn(**dict(vec))

gen = scan(["a", 3, 0, 1],
           ["b", 5, 0, 4],
           ["c", 2, 0, 2]) #, fn=f)
best = (None, None)
for v in gen:
    val = f(**dict(v))
    print val
    if best[1] is None or val < best[1]:
        best = (v,val)
        print "New best:", v, "with f =", val
print "Absolute best =", best
