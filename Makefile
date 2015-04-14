#CXXFLAGS := -g -O3
CXXFLAGS := -O3

LIBHEADERS := $(wildcard include/Professor/*.h)
LIBSOURCES := $(wildcard src/*.cc)
LIBOBJECTS := $(patsubst %,obj/%.o, ParamPoints Ipol ProfMaster)

CYTHONSOURCES := $(wildcard pyext/professor2/*.pxd) $(wildcard pyext/professor2/*.pyx)

.PHONY := all lib pyext tests cxxtests pytests check clean


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

pytests: pyext
	@true

cxxtests: test/testParamPoints test/testIpol test/testMaster
	@true

test/test%:
	g++ -std=c++11 $(CXXFLAGS) $^.cc -Iinclude -Llib -lProfessor2 -o $@

check: all
	echo && test/testParamPoints
	echo && test/testIpol
	echo && test/testMaster

clean:
	rm -rf obj/*.o lib/* pyext/professor2/core.cpp pyext/professor2/core.so
