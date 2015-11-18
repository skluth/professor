# -*- python -*-

from professor2.histos import *
from professor2.paramsio import *


def read_histos(path):
    "Load histograms from file, into a dict of path -> yoda.Histo[DataBin]"
    histos = {}
    if path.endswith(".root"):
        ## Try to use ROOT
        # TODO: Could just use YODA for everything, including ROOT reading?
        try:
            import ROOT
            ROOT.gROOT.SetBatch(True)
        except ImportError:
            print "PyROOT not available... skipping"
            return histos

        def _getallrootobjs(d, basepath="/"):
            "Recurse through a ROOT file/dir and generate (path, obj) pairs"
            for key in d.GetListOfKeys():
                kname = key.GetName()
                if key.IsFolder():
                    # TODO: -> "yield from" in Py3
                    for i in _getallrootobjs(d.Get(kname), basepath+kname+"/"):
                        yield i
                else:
                    yield basepath+kname, d.Get(kname)
        try:
            f = ROOT.TFile(path)
            for rname, robj in _getallrootobjs(f):
                bins = []
                if robj.InheritsFrom("TH1"):
                    # TODO: allow 2D histos
                    if robj.InheritsFrom("TH2"):
                        continue
                    for ib in xrange(robj.GetBins()):
                        xmin = robj.GetXaxis().GetBinLowEdge(ib+1)
                        xmax = robj.GetXaxis().GetBinUpEdge(ib+1)
                        y = robj.GetBinContent(ib+1)
                        ey = robj.GetBinError(ib+1)
                        bins.append(DataBin(xmin, xmax, y, ey))
                    histos[rname] = Histo(bins, rname)
                elif robj.InheritsFrom("TGraph"):
                    for ip in xrange(robj.GetN()):
                        x, y = ROOT.Double(), ROOT.Double()
                        robj.GetPoint(ip, x, y)
                        xmin = x - robj.GetErrorXlow(ip)
                        xmax = x + robj.GetErrorXhigh(ip)
                        ey = (robj.GetErrorXlow(ip) + robj.GetErrorXhigh(ip)) / 2.0
                        bins.append(DataBin(xmin, xmax, y, ey))
                histos[rname] = Histo(bins, rname)
            f.close()
        except Exception, e:
            print "Can't load histos from ROOT file '%s': %s" % (path, e)

    # TODO: can do this more generically with a try..except block, but requires that yoda.read throws a *Python* exception
    elif any(path.endswith(ext) for ext in [".yoda", ".aida", ".flat"]):
        ## Try YODA (handles .yoda, .aida, and .flat)
        try:
            import yoda
            s2s = []
            for ao in yoda.read(path, asdict=False):
                import os
                ## Skip the Rivet cross-section and event counter objects
                # TODO: Avoid Rivet-specific behaviour by try block handling & scatter.dim requirements
                if os.path.basename(ao.path).startswith("_"):
                    continue
                ##
                s2s.append(ao.mkScatter())
            #s2s = [ao.mkScatter() for ao in yoda.read(path, asdict=False)]
            for s2 in s2s:
                bins = [DataBin(p.xMin, p.xMax, p.y, p.yErrAvg) for p in s2.points]
                #bins = [DataBin(p.xMin, p.xMax, p.y*(p.xMax-p.xMin), p.yErrAvg) for p in s2.points]
                # bins = [DataBin(p.xMin, p.xMax, p.y, p.yErrs) for p in s2.points]
                histos[s2.path] = Histo(bins, s2.path)
        except Exception, e:
            print "Can't load histos from file '%s': %s" % (path, e)
    return histos


def read_rundata(dirs, pfname="params.dat"): #, formats="yoda,root,aida,flat"):
    params, histos = {}, {}
    import os, glob
    for num, d in enumerate(dirs):
        run = os.path.basename(d)
        files = glob.glob(os.path.join(d, "*"))
        for f in files:
            ## Params file
            if os.path.basename(f) == pfname:
                params[run] = read_paramsfile(f)
            ## Histo file
            else:
                try:
                    ## Read as a path -> Histo dict
                    hs = read_histos(f)
                    ## Restructure into the path -> run -> Histo return dict
                    for path, hist in hs.iteritems():
                        histos.setdefault(path, {})[run] = hist
                except:
                    pass #< skip files that can't be read as histos
        ## Check that a params file was found and read in this dir
        if run not in params.keys():
            raise Exception("No params file '%s' found in run dir '%s'" % (pfname, d))
    return params, histos


# TODO: remove this alias
load_rundata = read_rundata
