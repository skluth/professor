Professor installation
===============================

.. note::

    If you have access to the CERN AFS file system you can try to use
    a version of Professor installed under
    :file:`/afs/cern.ch/sw/lcg/external/MCGenerators/professor`.


Required packages
-----------------

The base system for parametrisation requires:

gcc >=4.6
    Compiler, C++11 compatible

`Eigen 3 <http://eigen.tuxfamily.org/>`_
   Linear algebra --- headers only, version 3.26 and later recommended


In order to use the Professor tools you will need the following external
packages:

`cython <http://cython.org/>`_
    For the Python extensions
    

`Numpy <http://numpy.scipy.org>`_
    The numpy datatypes are used for most numerical calculations.


`iMinuit <https://github.com/iminuit>`_ or `PyMinuit <https://github.com/jpivarski/pyminuit>`_
    iMinuit (preferred) or PyMinuit are used to interface with the well known MINUIT package.

`Matplotlib <http://matplotlib.sourceforge.net>`_
    Some plotting tools require matplotlib to produce output.

    :program:`prof2-I` requires the `wxWindows <http://www.wxwindows.org>`_
    backend for matplotlib.



Installing Professor
--------------------

The latest release of professor can always be found `here <http://www.hepforge.org/downloads/professor>`_.

Extract the tarball and compile as such::

  CXXFLAGS="-I/LOCATION_OF_EIGEN_HEADERS -O4" make all


To make the Professor scripts and modules available you have several
options:

1. Quick and dirty: Add the :file:`bin` directory to your :envvar:`$PATH` and
   :file:`professor` to your :envvar:`$PYTHONPATH` by doing::

       export PATH="/path/to/bin:$PATH"
       export LD_LIBRARY_PATH="/path/to/lib:$LD_LIBRARY_PATH"
       export PYTHONPATH="/path/to/pyext/professor/:$PYTHONPATH"

   in your shell (for C-shells the above commands must be modified).
   This should allow you to run the Professor scripts from the command
   line, e.g.::

       prof2-tune -h

   To make this persistent between shell sessions you can add the above
   lines to your shell's runtime configuration, e.g.
   :file:`$HOME/.bashrc`.


2. More systematic: Alternatively, you can use the :command:`setup.py`
   script that is distributed with Professor and offers the usual
   options, try::

       ./setup.py --help

   in the :file:`trunk` directory.

   :option:`--prefix` allows you to give a directory under which
   Professor is installed similar to the usual `.configure && make &&
   make install` triple jump, e.g.::

      ./setup.py --prefix $HOME/local install

   will install the scripts in :file:`$HOME/local/bin` and the modules
   in :file:`$HOME/local/lib/python{XY}/site-packages/` where `XY` is
   the Python version available at this instance.

gun-parallel
============

This is awesome::

        seq 0 10 | parallel prof2-ipol download/ --pfile used_params  --order 3 ipol_test_rc_{} --wfile weights_eta --rc runcombs.dat:{}
