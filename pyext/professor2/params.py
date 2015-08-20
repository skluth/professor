# -*- python -*-

def mk_minvals(anchors):
    from numpy import array
    a = array(anchors)
    mins = [a[:,i].min() for i in xrange(len(a[0]))]
    return mins

def mk_maxvals(anchors):
    from numpy import array
    a = array(anchors)
    maxs = [a[:,i].max() for i in xrange(len(a[0]))]
    return maxs

def mk_center(anchors):
    """
    Calculate center from all anchor points --- needed?
    Requires knowledge of all anchor points, hmm
    """
    mins = mk_minvals(anchors)
    maxs = mk_maxvals(anchors)
    center = []
    for num, m in enumerate(mins):
        center.append(m+0.5*(maxs[num]-m))
    return center

def is_inrange(ppoint, minv, maxv):
    dec = True
    notinrange = []
    for i in xrange(len(ppoint)):
        if ppoint[i] < minv[i] or ppoint[i] > maxv[i]:
            dec=False
            notinrange.append(i)
    return dec, notinrange
