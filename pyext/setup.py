from distutils.core import setup
from distutils.extension import Extension
from glob import glob

import os
libdir=os.path.abspath("lib")
incdir = os.path.abspath("include")

curdir=os.environ["PWD"] # assume makefile is called from basedir
os.environ["CC"] = "g++"
os.environ["CXX"] = "g++"

lookupdirs = [
    libdir
    ]

def ext(name, depends=[], statics=[]):
    return Extension(
        "professor2.%s" % name,
        ["pyext/professor2/%s.cpp" % name] + statics,
        language="C++",
        depends=depends,
        include_dirs=[incdir, "pyext/professor2"],
        extra_compile_args= str("-I%s/include -Wno-unused-but-set-variable -Wno-sign-compare -std=c++11"%(curdir)).split(),
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
