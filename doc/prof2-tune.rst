prof2-tune
==========

.. note::

    This script requires :code:`iminuit` to be installed.

Run minimisation of a goodness-of-fit measure of the parametrisation and data.


::

  prof2-tune DATADIR IPOLFILE --wfile WEIGHTFILE

Options:

::

   --limits LIMITFILE # Specify a text file to fix parameters to certain values or force the parameter to be in certain bounds
   --scan NPOINTS     # Scan a grid of NPOINTS^P points to look for a good starting point (by default the centre of the parameter space is used)
   -s STRATEGY        # Set Minuit strategy

Weight syntax
_____________

Weigts are used to enhance the importance of observables or bins in the goodness-of-fit measure.

The weight for a range of bins of a histogram can be specified wither depending on the value of the observable (bin edges)
using :code:`@` or the directly specifying the bin index using :code:`#`.

A few examples:

::

  /ATLAS_2010_S8918562/d03-x01-y01 1        # Set weight to 1   for   each bin of this histo
  /ATLAS_2010_S8918562/d05-x01-y01 100      # Set weight to 100 for   each bin of this histo
  /ATLAS_2010_S8918562/d07-x01-y01@0:20  10 # Set weight to  10 for   bins with binEDGES in [0,20)
  /ATLAS_2010_S8918562/d07-x01-y01@20:40 50 # Set weight to  10 for   bins with binEDGES in [20,50)
  /TOTEM_2012_I1115294/d01-x01-y01#0:20  10 # Set weight to  10 for   bins with binINCIDES in [0,20)     
  /TOTEM_2012_I1115294/d01-x01-y01#20:40 50 # Set weight to  10 for   bins with binINDICES in [20,50)    

Limits syntax
_____________

* Simple text file
* Parameter name followed by one number is interpreted as fixing the parameter to that value
* Parameter name followed by two numbers is interpreted as imposing bounds on the parameter




Example:

.. raw:: html

    <script type="text/javascript" src="https://asciinema.org/a/029488a8la3xudqb7vqm28eth.js" id="asciicast-029488a8la3xudqb7vqm28eth" async></script>
