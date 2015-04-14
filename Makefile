#CXXFLAGS := -g -O3
CXXFLAGS := -O3

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
	g++ -std=c++11 -shared -Wl,-soname,libProfessor2.so -o $@ $(LIBOBJECTS)

obj/%.o: src/%.cc $(LIBHEADERS)
	mkdir -p obj lib
	g++ -std=c++11 $(CXXFLAGS) -c -fPIC $< -Iinclude -o $@

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
	g++ -std=c++11 $(CXXFLAGS) $< -Iinclude -Llib -lProfessor2 -o $@

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
