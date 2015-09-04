from distutils.core import setup
from distutils.extension import Extension
from glob import glob

import os
srcdir = os.environ["PWD"] #< assume makefile is called from base dir  TODO: use cwd()?
libdir = os.path.abspath("lib") #< assume makefile is called from base dir  TODO: use srcdir var?
incdir = os.path.abspath("include") #< assume makefile is called from base dir  TODO: use srcdir var?
os.environ.setdefault("CC", "g++")
os.environ.setdefault("CXX", "g++")

# TODO: simplify!! The ext function isn't actually needed

lookupdirs = [libdir]

def ext(name, depends=[], statics=[]):
    return Extension(
        "professor2.%s" % name,
        ["pyext/professor2/%s.cpp" % name] + statics,
        language="C++",
        depends=depends,
        include_dirs=[incdir, os.path.join(srcdir, "pyext", "professor2")],
        extra_compile_args="-std=c++11 -Wno-unused-but-set-variable -Wno-sign-compare".split(),
        library_dirs=lookupdirs,
        runtime_library_dirs=lookupdirs[1:],
        libraries=["Professor2"])

header_files = glob("../include/*.h")

extns = [ext("core", header_files)]

setup(name = "professor2",
      version="2.0.0a",
      ext_modules = extns,
      packages = ["professor2"],
      package_dir = {"": "pyext"},
      description="Professor version 2",
      author="Professor collaboration",
      author_email="professor@projects.hepforge.org",
      url="http://professor.hepforge.org"
     )
