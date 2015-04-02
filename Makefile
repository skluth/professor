
all: 
	mkdir -p obj lib bin test
	g++ -std=c++11  -O3 -c -fPIC src/ParamPoints.cc -Iinclude -o obj/ParamPoints.o
	g++ -std=c++11 -g  -O3 src/testParamPoints.cc obj/ParamPoints.o -Iinclude -o test/testParamPoints
	g++ -std=c++11 -O3 -c -fPIC src/Ipol.cc -Iinclude -o obj/Ipol.o
	g++ -std=c++11 -g  -O3 src/testIpol.cc obj/ParamPoints.o obj/Ipol.o -Iinclude -o test/testIpol
	g++ -std=c++11 -O3 -c -fPIC src/ProfMaster.cc -Iinclude -o obj/ProfMaster.o
	g++ -std=c++11 -g  -O3 src/testMaster.cc obj/ParamPoints.o obj/Ipol.o obj/ProfMaster.o -Iinclude -o test/testMaster
	g++ -std=c++11 -shared -Wl,-soname,libProfMaster.so -o lib/libProfMaster.so   obj/ProfMaster.o obj/ParamPoints.o obj/Ipol.o
	cython pyext/profmaster/core.pyx --cplus
	python pyext/setup.py build_ext -i --force
	python pyext/setup.py install --prefix=.


clean:
	rm -rf obj/*.o test/*
