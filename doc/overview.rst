Overview
========

Basic work cycle
----------------

This document sketches the basic work cycle when (but is not limited to) tuning generators.


#. Defining and sampling points from a parameter space  (:file:`prof2-sample`)
#. Running the parametrisation (:file:`prof2-ipol`)
#. Minimisation of a goodness-of-fit measure of the parametrisation and data (:file:`prof2-tune`)

Visualisation
-------------

=============== ================================================= =====================
Script          Purpose                                           Dependencies
=============== ================================================= =====================
prof2-I         Interactive parametrisation explorer              matplotlib, wxWindows
prof2-envelopes Plot sampling envelope                            matplotlib
prof2-sens      Plots of sensitivity of observables to parameters matplotlib
prof2-residuals Parametrisation quality plots                     matplotlib, YODA
=============== ================================================= =====================


Other
-----

============== ===============================================
Script          Purpose                                       
============== ===============================================
prof2-ls       Summary of existing parametrisations
prof2-runcombs Allow for systematic checks of parametrisations
prof2-config   Get system relevant information
============== ===============================================

