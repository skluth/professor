#CXXFLAGS := -g -O3
CXXFLAGS := -O3

.PHONY := all

# TODO: Build tests against whole lib

# TODO: Split the make rule into multiple targets, with proper 'atomic' rebuild triggering

all:
	mkdir -p obj lib bin test
	g++ -std=c++11 $(CXXFLAGS) -c -fPIC src/Ipol.cc -Iinclude -o obj/Ipol.o
	g++ -std=c++11 $(CXXFLAGS) -c -fPIC src/ParamPoints.cc -Iinclude -o obj/ParamPoints.o
	g++ -std=c++11 $(CXXFLAGS) -c -fPIC src/ProfMaster.cc -Iinclude -o obj/ProfMaster.o
	g++ -std=c++11 -shared -Wl,-soname,libProfessor2.so -o lib/libProfessor2.so   obj/ProfMaster.o obj/ParamPoints.o obj/Ipol.o
	g++ -std=c++11 $(CXXFLAGS) src/testIpol.cc obj/ParamPoints.o obj/Ipol.o -Iinclude -o test/testIpol
	g++ -std=c++11 $(CXXFLAGS) src/testParamPoints.cc obj/ParamPoints.o -Iinclude -o test/testParamPoints
	#g++ -std=c++11 $(CXXFLAGS) src/testMaster.cc obj/ParamPoints.o obj/Ipol.o obj/ProfMaster.o -Iinclude -o test/testMaster
	cython pyext/professor2/core.pyx --cplus
	python pyext/setup.py build_ext -i --force
	python pyext/setup.py install --prefix=.


clean:
	rm -rf obj/*.o test/*
