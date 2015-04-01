#!/usr/bin/env python

from numpy import linspace, array

def f(x):
    return 3*x**3 + 2*x +1

A=linspace(0, 10, 20)
X = array([[a] for a in A])
Y=f(A)

from profmaster import ProfMaster

pm = ProfMaster(X)

pm.addIpol("f1", Y, 1)
pm.addIpol("f2", Y, 2)
pm.addIpol("f3", Y, 3)
pm.addIpol("f4", Y, 4)
pm.addIpol("f5", Y, 5)
pm.addIpol("f6", Y, 6)

Z1 = map(lambda x: pm.getValue("f1", x), X)
Z2 = map(lambda x: pm.getValue("f2", x), X)
Z3 = map(lambda x: pm.getValue("f3", x), X)
Z4 = map(lambda x: pm.getValue("f4", x), X)
Z5 = map(lambda x: pm.getValue("f5", x), X)
Z6 = map(lambda x: pm.getValue("f6", x), X)

import pylab
pylab.plot(A, Y, "ro", label="Anchors")
pylab.plot(X, Z1, "--", label="Ipol 1")
pylab.plot(X, Z2, "--", label="Ipol 2")
pylab.plot(X, Z3, "--", label="Ipol 3")
pylab.plot(X, Z4, "--", label="Ipol 4")
pylab.plot(X, Z5, "--", label="Ipol 5")
pylab.plot(X, Z6, "--", label="Ipol 6")
pylab.legend(loc=2)
pylab.show()
