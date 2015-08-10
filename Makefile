PREFIX := /usr/local

CXXSTD := c++11

#CXXFLAGS := -g -O3
CXXFLAGS  := -O3

CPPFLAGS  :=

ROOTINC := $(wildcard $(shell root-config --incdir) )
ROOTLIB := $(wildcard $(shell root-config --libdir) )

LIBHEADERS := $(wildcard include/Professor/*.h)
LIBSOURCES := $(wildcard src/*.cc)
LIBOBJECTS := $(patsubst %,obj/%.o, ParamPoints Ipol ProfMaster)
TESTPROGS  := test/testParamPoints test/testIpol test/testMaster
CYTHONSOURCES := $(wildcard pyext/professor2/*.pxd) $(wildcard pyext/professor2/*.pyx)

.PHONY := all lib pyext tests cxxtests pytests check icheck clean


all: lib pyext tests
	@true

lib: lib/libProfessor2.so
	@true

lib/libProfessor2.so: $(LIBOBJECTS)
	@true
	g++ -shared -Wl,-soname,libProfessor2.so -o $@ $(LIBOBJECTS)

obj/%.o: src/%.cc $(LIBHEADERS)
	mkdir -p obj lib
	g++ -std=$(CXXSTD) $(CPPFLAGS) $(CXXFLAGS) -c -fPIC $< -Iinclude -o $@

pyext: pyext/professor2/core.so $(wildcard pyext/professor2/*.py)
	python pyext/setup.py install --prefix=.

pyext/professor2/core.so: $(LIBHEADERS) $(CYTHONSOURCES)
	cython pyext/professor2/core.pyx --cplus
	python pyext/setup.py build_ext -i --force

tests: cxxtests pytests
	@true

cxxtests: $(TESTPROGS)
	@true

test/%: test/%.cc $(LIBHEADERS) lib
	g++ -std=$(CXXSTD) $(CPPFLAGS) $(CXXFLAGS) $< -Iinclude -Llib -lProfessor2 -o $@

root: src/testRoot.cc  $(LIBHEADERS) lib
	g++ -std=$(CXXSTD) $(CPPFLAGS) $(CXXFLAGS) $< -Iinclude -I$(ROOTINC) -L$(ROOTLIB) -lHist -lCore -Llib -lProfessor2 -o test/test$@

driver: prof-driver/ProfDriver.cc  $(LIBHEADERS) lib
	g++ -fPIC -std=$(CXXSTD) $(CPPFLAGS) $(CXXFLAGS) $< -Iinclude -Iprof-driver -Llib -lProfessor2 -c -o prof-driver/ProfDriver.o
	g++ -std=$(CXXSTD) $(CPPFLAGS) $(CXXFLAGS) prof-driver/testDriver.cc prof-driver/ProfDriver.o -Iinclude -Iprof-driver -Llib -lProfessor2 -o prof-driver/test

pytests: pyext
	@true

check: tests
	@echo
	@echo "testParamPoints" && test/testParamPoints && echo "\n\n"
	@echo "testIpol" && test/testIpol && echo "\n\n"
	@echo "testMaster" && test/testMaster

icheck: tests
	test/testPython
	test/testPython1D
	test/testPython2D

clean:
	rm -rf obj/*.o lib/*
	rm -f pyext/professor2/core.cpp pyext/professor2/core.so
	rm -f $(TESTPROGS)

install: all
	mkdir -p $(PREFIX)/bin && cp bin/* $(PREFIX)/bin/
	mkdir -p $(PREFIX)/include && cp -r include/Professor $(PREFIX)/include/
	test -d lib   && mkdir -p $(PREFIX)/lib   && cp -r lib/* $(PREFIX)/lib/ || true
	test -d lib64 && mkdir -p $(PREFIX)/lib64 && cp -r lib64/* $(PREFIX)/lib64/ || true
