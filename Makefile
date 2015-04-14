#CXXFLAGS := -g -O3
CXXFLAGS := -O3

LIBHEADERS := $(wildcard include/Professor/*.h)
LIBSOURCES := $(wildcard src/*.cc)
TESTSOURCES := $(wildcard test/*.cc)
CYTHONSOURCES := $(wildcard pyext/professor2/*.pxd) $(wildcard pyext/professor2/*.pyx)

.PHONY := all lib pyext tests cxxtests pytests

# TODO: Split the make rule into more atomic targets, to allow parallel builds of the object files

all: lib pyext tests
	@true

lib: lib/libProfessor2.so
	@true

lib/libProfessor2.so: $(LIBHEADERS) $(LIBSOURCES)
	mkdir -p obj lib
	g++ -std=c++11 $(CXXFLAGS) -c -fPIC src/Ipol.cc -Iinclude -o obj/Ipol.o
	g++ -std=c++11 $(CXXFLAGS) -c -fPIC src/ParamPoints.cc -Iinclude -o obj/ParamPoints.o
	g++ -std=c++11 $(CXXFLAGS) -c -fPIC src/ProfMaster.cc -Iinclude -o obj/ProfMaster.o
	g++ -std=c++11 -shared -Wl,-soname,libProfessor2.so -o lib/libProfessor2.so $(wildcard obj/*.o)

pyext: pyext/professor2/core.so
	@true

pyext/professor2/core.so: lib/libProfessor2.so $(CYTHONSOURCES)
	cython pyext/professor2/core.pyx --cplus
	python pyext/setup.py build_ext -i --force
	python pyext/setup.py install --prefix=.

tests: cxxtests pytests
	@true

pytests: pyext
	@true

cxxtests: lib
	g++ -std=c++11 $(CXXFLAGS) test/testIpol.cc -Iinclude -Llib -lProfessor2 -o test/testIpol
	g++ -std=c++11 $(CXXFLAGS) test/testParamPoints.cc -Iinclude -Llib -lProfessor2 -o test/testParamPoints
	g++ -std=c++11 $(CXXFLAGS) test/testMaster.cc -Iinclude -Llib -lProfessor2 -o test/testMaster

clean:
	rm -rf obj/*.o lib/*
