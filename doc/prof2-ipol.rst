prof2-ipol
============

Example:

.. raw:: html

    <script type="text/javascript" src="https://asciinema.org/a/6pwquht37n62wh9466zv81yl8.js" id="asciicast-6pwquht37n62wh9466zv81yl8" async></script>


Advanced usage
--------------
Example command line for usage with gnu-parallel::
    seq 0 10 | parallel prof2-ipol download/ --pfile used_params  --order 3 ipol_test_rc_{} --wfile weights_eta --rc runcombs.dat:{}
