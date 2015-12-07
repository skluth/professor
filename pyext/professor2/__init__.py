import sys
pyversion = sys.version_info
if sys.version_info[0] < 2 or (sys.version_info[0] == 2 and sys.version_info[1] < 6):
    raise Exception("Professor2 requires Python 2.6 or greater")

## Import Cython wrappings on the C++ core library
from professor2.core import *

## Import high-level Python functionality
__all__ = ["ui"]
