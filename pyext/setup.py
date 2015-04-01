from distutils.core import setup
from distutils.extension import Extension
from glob import glob

import os
libdir=os.path.abspath("lib")
incdir = os.path.abspath("include")

curdir=os.environ["PWD"] # assume makefile is called from basedir


lookupdirs = [
    libdir
    ]

def ext(name, depends=[], statics=[]):
    return Extension(
        "profmaster.%s" % name,
        ["pyext/profmaster/%s.cpp" % name] + statics,
        language="C++",
        depends=depends,
        include_dirs=[incdir, "pyext/profmaster"],
        extra_compile_args= str("-I%s/include -Wno-unused-but-set-variable -Wno-sign-compare -std=c++11"%(curdir)).split(),
        library_dirs=lookupdirs,
        runtime_library_dirs=lookupdirs[1:],
        libraries=["ProfMaster"])

header_files = glob("../include/*.h")

extns = [ext("core", header_files)]

setup(name = "profmaster",
      version="1.0.0",
      ext_modules = extns,
      packages = ["profmaster"],
      package_dir = {"": "pyext"},
      description="Professor C++ driver bindings",
      author="Professor collaboration",
      author_email="professor@projects.hepforge.org",
      url="http://professor.hepforge.org"
     )
