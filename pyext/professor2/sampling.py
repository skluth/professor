# -*- python -*-

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


## Test biased sampler machinery if run as main
if __name__ == "__main__":
    s = Sampler(1, 10, "exp(x)")
    import yoda
    h = yoda.Histo1D(20, 0, 10)
    for _ in xrange(10000):
        h.fill( s() )
    yoda.plot(h, "foo.pdf")
