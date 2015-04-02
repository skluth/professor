#!/usr/bin/env python

from numpy import linspace, array

def f(x):
    return 3*x**3 + 2*x +1

A=linspace(0, 10, 20)
X = array([[a] for a in A])
Y=f(A)

from profmaster import Ipol

ipols = [Ipol(X, Y, i, "order%i"%i) for i in xrange(1,7)]

Z=[]
for i in ipols:
    Z.append(map(lambda x: i.value(x), X))

import pylab
pylab.plot(A, Y, "ro", label="Anchors")
for num, z in enumerate(Z):
    pylab.plot(X, z, "-", label="Ipol order %i"%(num+1))
pylab.legend(loc=2)
pylab.show()
