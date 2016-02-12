prof2-sample
============

Parameter sampling, template instantiation

::

  prof2-sample PARAMETER_TEXTFILE <options>

Commonly used options::

  -n NUM      # Specify number of points to sample
  -o OUTDIR   # Specify output folder
  -t TEMPLATE # Select a template to fill

Basic usage
-----------

Random uniform sampling of a P-dimensional parameter space.

The parameter space is defined using a text file that contains the name and the
sampling range for each parameter as such::

    gVXD        0.1  1.0
    gAXD        0.1  1.0
    gVQ         0.1  1.0
    gAQ         0.1  1.0

For each sampled point (number determined by option :code:`-n`) , a new
subdirectory is created in the outputfolder (see option :code:`-o`) that will
contain a file (default file name is :file:`params.dat`, see option
:code:`--pfile`) with the sampled parameter values::

  gVXD 6.263030e-01
  gAXD 7.299448e-01
  gVQ 7.497046e-01
  gAQ 7.876020e-01

In addition, a template can be specified with the :code:`-t` switch. The
instantiation will be written to the same subdirectory as the parameter file.
Variables to be substituted need to be in given curly braces, e.g.::

    (ufo){{ # Double curly braces to prevent parsing
    block dminputs
            1  {gVXD}
            2  {gAXD}
            3  {gVQ}
            4  {gAQ}
            5  {gVQ}
            6  {gAQ}
    }}(ufo) # Double curly braces to prevent parsing
  


Example:

.. raw:: html

    <script type="text/javascript" src="https://asciinema.org/a/eq7rnnu3jf9ih8jo7vjx9mtwu.js" id="asciicast-eq7rnnu3jf9ih8jo7vjx9mtwu" async></script>


Advanced usage
--------------

To bias the sampling, a veto function can be written in python and passed to
:file:`prof2-sample` with the :code:`-v` switch.
