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
        "profdriver.%s" % name,
        ["pyext/profdriver/%s.cpp" % name] + statics,
        language="C++",
        depends=depends,
        include_dirs=[incdir, "pyext/profdriver"],
        extra_compile_args= str("-I%s/include -Wno-unused-but-set-variable -Wno-sign-compare"%(curdir)).split(),
        library_dirs=lookupdirs,
        runtime_library_dirs=lookupdirs[1:],
        libraries=["ProfDriver"])

header_files = glob("../include/*.h")

extns = [ext("core", header_files)]

setup(name = "profdriver",
      version="0.1.2",
      ext_modules = extns,
      packages = ["profdriver"],
      package_dir = {"": "pyext"},
      description="Professor C++ driver bindings",
      author="Professor collaboration",
      author_email="professor@projects.hepforge.org",
      url="http://professor.hepforge.org"
     )
