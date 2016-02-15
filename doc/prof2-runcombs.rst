prof2-runcombs
==============

Make random unique combinations of subsets of all available inputs.

::

    prof2-runcombs RUNDIR  <combination syntax> [options]


Options::

    -o # Output file name


Combination syntax
__________________

Suppose we have a directory with 100 valid runs in them.
The combination syntax is always such that the user provides
how many of these valid runs should be left out and how many
unique combinations are to be generated.

For example :code:`0:1` would leave out 0, i.e. use all inputs
and return the one trivially possible combination.

:code:`33:200` would create 200 combinations of 100-33=67 
input runs.

Any number of such combination strings can be supplied. They will
all be written into the same file.

Example:

::

    prof2-runcombs RUNDIR 0:1 10:200 20:300 30:400


.. raw:: html

    <script type="text/javascript" src="https://asciinema.org/a/185hc1enfq5ngtnc5xva5jg0k.js" id="asciicast-185hc1enfq5ngtnc5xva5jg0k" async></script>
