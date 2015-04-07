#!/usr/bin/env python

from numpy import linspace, array

def f(x,y):
    return 3*x**3 + 4*y**3 + 0.5*y**2 + 2*x + 5*x*y +1 +x +9*y

A=linspace(0, 20, 20)
from numpy import linspace, asarray, meshgrid, zeros
M_X, M_Y = meshgrid(A, A)
M_Z = Z=zeros(M_X.shape)

P=[]
Y=[]
for i in xrange(M_X.shape[0]):
    for j in xrange(M_X.shape[1]):
        P.append( [i,j])
        Y.append(f(i,j))

from professor2 import Ipol
ipols = [Ipol(P, Y, i, "order%i"%i) for i in xrange(1,7)]

for I in ipols[2:3]:
    for i in xrange(M_X.shape[0]):
        for j in xrange(M_X.shape[1]):
            M_Z[i][j] = I.value(asarray([M_X[i][j], M_Y[i][j]]))

    import matplotlib.pyplot as plt
    from mpl_toolkits.mplot3d import Axes3D
    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')
    # Draw anchors as scatter graph first
    X1=map(lambda x:x[0], P)
    Y1=map(lambda x:x[1], P)
    Z1=Y
    ax.scatter(X1,Y1,Z1, c="r")
    ax.plot_wireframe(M_X, M_Y, M_Z,rstride=1, cstride=1)
    fig.show()

raw_input("Press any key")
