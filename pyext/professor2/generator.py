
"""
Generator tools for sampling points in a space.
"""

import numpy

from professor.tools import log as logging
import param


class PointGeneratorBase(object):
    """
    Base class for point generators.
    """
    def __init__(self, SEED=0):
        self._functions=dict()
        self._ranges=dict()
        self._seed = self.setSeed(SEED)
        self._conditions=[]
        logging.info( "Initialised with seed %i"%self._seed)

    def setSeed(self, SEED):
        """Pass-through to numpy.random.seed."""

        try:
            seed = int(SEED) # Attempt to use manual seed
        except:
            seed = -1 #
            logging.info("Using random seed")
        if seed < 0:
            seed = numpy.random.randint(2**63-1)

        numpy.random.seed(seed)
        return seed

    def newPoint(self):
        """Get a new random point."""
        pass

    def alongAxis(self, r, xmin, xmax):
        return xmin + r*(xmax-xmin)

    # TODO: Poissondisk sampling?


    # TODO: Is this working in parallel,i.e. two generators from one
    # PointGenerator instance?
    def generate(self, num):
        """Return a Python generator with new points.

        Parameters
        ----------
        num : int
            The number of points.
        """
        i = 0
        while i < num:
            yield self.newPoint()
            i += 1


class SimpleRandomGenerator(PointGeneratorBase):
    """
    Most simple generator, a list of ranges is taken to
    produce points.
    """
    def addRange(self, param, xmin, xmax):
        if not self._ranges.has_key(param):
            if xmin < xmax:
                self._ranges[param] = (float(xmin), float(xmax))
            elif xmin > xmax:
                self._ranges[param] = (float(xmax), float(xmin))

    def addFunction(self, param, function):
        if not self._functions.has_key(function):
            self._functions[param] = function

    def newPoint(self):
        """
        Default behaviour: flat sampling
        Otherwise: from distribution, see
        http://docs.scipy.org/doc/numpy/reference/routines.random.html for a list
        """
        pd = dict()
        for k, v in self._ranges.iteritems():
            if not self._functions.has_key(k):
                r = numpy.random.random()
                pd[k] = self.alongAxis(r , v[0], v[1]) # For uniform thingies
            else:
                r= -1
                while r<=v[0] or r>=v[1]: # ensure sampling is in bounds
                    r = eval("%s"%self._functions[k])
                pd[k] = r

        return param.ParamBase.mkFromDict(pd)

class ConditionalRandomGenerator(SimpleRandomGenerator):
    """
    A more sophisticated Random Generator that allows conditional
    sampling.
    """
    def addCondition(self, cond):
        self._conditions.append(cond.split(":"))


    def generate(self, num):
        """Return a Python generator with new points.

        Parameters
        ----------
        num : int
            The number of points.
        """
        i = 0
        while i < num:
            a = self.newPoint()
            if a.checkCondition(self._conditions):
                yield a
                i += 1
