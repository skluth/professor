cimport profdriver as c

cdef class ProfDriver:
    cdef c.ProfDriver *_ptr

    def __cinit__(self):
        self._ptr = new c.ProfDriver()

    def __del__(self):
        del self._ptr

    def writeProfDF(self, fname):
        self._ptr.writeProfDF(fname)

    def numOfCoefficients(self, dim, order):
        return self._ptr.numOfCoefficients(dim, order)

    def dim(self):
        return self._ptr.dim()

    def order(self):
        return self._ptr.order()

    def anchors(self):
        return self._ptr.anchors()

    def calc(self, binid, order):
        self._ptr.calc(binid, order)

    def printMeta(self):
        self._ptr.printMeta()

    def printNbins(self):
        self._ptr.printNbins()
    def printBinIds(self):
        self._ptr.printBinIds()
    def printCoeffs(self):
        self._ptr.printCoeffs()

    def addAnchor(self, anchor):
        self._ptr.addAnchor(anchor)

    def addValue(self, binid, val):
        self._ptr.addValue(binid, val)

    def calcCenter(self):
        return self._ptr.calcCenter()
    def calcMin(self):
        return self._ptr.calcMin()
    def calcMax(self):
        return self._ptr.calcMax()
    def getCenter(self):
        return self._ptr.getCenter()
    def setCenter(self, ctr):
        self._ptr.setCenter(ctr)

    def setParamNames(self, pnames):
        self._ptr.setParamNames(pnames)


    def getValue(self, key, P):
        return self._ptr.getValue(key, P)

