#!/bin/bash

fpath=`readlink -f ${BASH_SOURCE[0]}`

profdir=`dirname ${fpath}`

PYV=`python -c "import sys;v=sys.version_info[0:2];print '%i.%i'%(v)"`

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${profdir}/lib
export PYTHONPATH=$PYTHONPATH:${profdir}/lib/python${PYV}/site-packages
