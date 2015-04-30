PREFIX := /usr/local

CXXSTD := c++11

#CXXFLAGS := -g -O3
CXXFLAGS  := -O3

CPPFLAGS  :=



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

pyext: pyext/professor2/core.so
	@true

pyext/professor2/core.so: $(LIBHEADERS) $(CYTHONSOURCES)
	cython pyext/professor2/core.pyx --cplus
	python pyext/setup.py build_ext -i --force
	python pyext/setup.py install --prefix=.

tests: cxxtests pytests
	@true

cxxtests: $(TESTPROGS)
	@true

test/%: test/%.cc $(LIBHEADERS) lib
	g++ -std=$(CXXSTD) $(CPPFLAGS) $(CXXFLAGS) $< -Iinclude -Llib -lProfessor2 -o $@

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

install:
	cp bin/* $(PREFIX)/bin/
	cp -r include/Professor $(PREFIX)/include/
	cp -r lib/* $(PREFIX)/lib/
	cp -r lib64/* $(PREFIX)/lib64/
