#!/bin/bash

# This needs to be set to the PREFIX specified in make install step
INSTALLDIR=/Users/ippp/professor/local

export DYLD_LIBRARY_PATH=${INSTALLDIR}/lib:$DYLD_LIBRARY_PATH
export PYTHONPATH=${INSTALLDIR}/lib/python2.7/site-packages:$PYTHONPATH
export PATH=${INSTALLDIR}/bin:$PATH
