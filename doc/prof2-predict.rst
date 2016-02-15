prof2-predict
=============

Produce YODA Scatter 2D representations of parametrisation at a certain point in the parameter space. Requires :code:`YODA`.

::

    prof2-predict PFILE  IPOLFILE <options>


:code:`PFILE` must point to a plain text file that looks like that:

::
 
  PARAMA 1.2
  PARAMB 5.3
  PARAMC 5.6


Options::

  -o      # Specify output folder
  --wfile # Specify a weight file to limit plotting to a subset of observables
