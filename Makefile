PREFIX := /usr/local

CXXSTD := c++11

ifndef CXX
  CXX := g++
endif

ifndef CPPFLAGS
  CPPFLAGS := -I/usr/include/eigen3
endif

ifndef CXXFLAGS
  CXXFLAGS := -O3
  ifdef DEBUG
	ifneq ($(DEBUG),0)
	  CXXFLAGS += -g
	endif
  endif
endif

ifndef PYTHON
  PYTHON := python
endif

ifndef CYTHON
  CYTHON := cython
endif


###################

# TODO: automatically pass this into the C++ and Python sources
VERSION := "2.1.0a"
DISTNAME := Professor-$(VERSION)

HAVE_ROOT := $(shell which root-config 2> /dev/null)
HAVE_CYTHON := $(shell which $(CYTHON) 2> /dev/null)

LIBHEADERS := $(wildcard include/Professor/*.h)
LIBSOURCES := $(wildcard src/*.cc)
LIBOBJECTS := $(patsubst %,obj/%.o, ParamPoints Counter Ipol)
TESTSOURCES := $(wildcard test/*.cc test/testPython*)
TESTPROGS  := test/testParamPoints test/testIpol
BINPROGS := $(wildcard bin/*)
PYTHONSOURCES := $(wildcard pyext/professor2/*.py)
CYTHONSOURCES := $(wildcard pyext/professor2/*.pxd) $(wildcard pyext/professor2/*.pyx)

.PHONY := all lib pyext tests cxxtests pytests check icheck clean root dist


all: lib pyext tests
	@true

lib: lib/libProfessor2.so
	@true

lib/libProfessor2.so: $(LIBOBJECTS)
	mkdir -p lib
	$(CXX) -shared -Wl,-soname,libProfessor2.so -o $@ $(LIBOBJECTS)

obj/%.o: src/%.cc $(LIBHEADERS)
	mkdir -p obj
	$(CXX) -std=$(CXXSTD) -Iinclude $(CPPFLAGS) $(CXXFLAGS) -c -fPIC $< -o $@

pyext: pyext/professor2/core.so $(wildcard pyext/professor2/*.py)
	$(PYTHON) pyext/setup.py install --prefix=.

ifdef HAVE_CYTHON
pyext/professor2/core.cpp: $(LIBHEADERS) $(CYTHONSOURCES) lib
	$(CYTHON) pyext/professor2/core.pyx --cplus
else
pyext/professor2/core.cpp: $(LIBHEADERS) $(CYTHONSOURCES) lib
	$(error "Cython not available; can't build $@")
endif

pyext/professor2/core.so: pyext/professor2/core.cpp
	$(PYTHON) pyext/setup.py build_ext -i --force

tests: cxxtests pytests
	@true

cxxtests: $(TESTPROGS)
	@true

test/%: test/%.cc $(LIBHEADERS) lib
	$(CXX) -std=$(CXXSTD) -Iinclude $(CPPFLAGS) $(CXXFLAGS) $< -Llib -lProfessor2 -o $@

ifdef HAVE_ROOT
root: src/testRoot.cc $(LIBHEADERS) lib
	$(CXX) -std=$(CXXSTD) $(CPPFLAGS) $(CXXFLAGS) $< -Iinclude `root-config --cflags --libs` -Llib -lProfessor2 -o test/test$@
endif

pytests: pyext
	@true

check: tests
	@echo
	@echo "testParamPoints" && test/testParamPoints && echo "\n\n"
	@echo "testIpol" && test/testIpol && echo "\n\n"

icheck: tests
	test/testPython
	test/testPython1D
	test/testPython2D

install: all
	mkdir -p $(PREFIX)/bin && cp bin/* $(PREFIX)/bin/
	mkdir -p $(PREFIX)/include && cp -r include/Professor $(PREFIX)/include/
	test -d lib   && mkdir -p $(PREFIX)/lib   && cp -r lib/* $(PREFIX)/lib/ || true
	test -d lib64 && mkdir -p $(PREFIX)/lib64 && cp -r lib64/* $(PREFIX)/lib64/ || true
#	cp setup.sh $(PREFIX)

dist: all
	rm -rf $(DISTNAME)
	mkdir -p $(DISTNAME)
	cp --parents \
       README Makefile \
       $(LIBHEADERS) \
       $(LIBSOURCES) \
       $(BINPROGS) \
       $(TESTSOURCES) \
       $(PYTHONSOURCES) pyext/setup.py \
       $(CYTHONSOURCES) $(wildcard pyext/professor2/*.cpp) \
       $(wildcard contrib/*) \
       $(DISTNAME)/
	tar czf $(DISTNAME).tar.gz $(DISTNAME)

clean:
	rm -rf obj/*.o lib/*
	rm -f pyext/professor2/core.cpp pyext/professor2/core.so
	rm -f $(TESTPROGS)
	rm -rf $(DISTNAME) $(DISTNAME).tar.gz
