#YODAPATH=/home/hschulz/src/rivet-2.1.1/local
YODAPATH=$(wildcard $(shell yoda-config --prefix))

.PHONY = lib

all: 
	mkdir -p obj lib bin
	g++  -O3 -c -fPIC src/AnyOption.cxx -Iinclude/AnyOption -o obj/AnyOption.o
	g++  -O3 -c -fPIC src/ProfDriver.cc -Iinclude -o obj/ProfDriver.o
	g++  -O3 -c -fPIC src/main.cc -Iinclude -o obj/test.o
	g++  -O3 obj/test.o obj/ProfDriver.o -o bin/test -Iinclude
	g++ -shared -Wl,-soname,libProfDriver.so -o lib/libProfDriver.so   obj/ProfDriver.o
	cython pyext/profdriver/core.pyx --cplus
	python pyext/setup.py build_ext -i --force
	python pyext/setup.py install --prefix=.



yoda:
	g++ -g  -O3 -L$(YODAPATH)/lib -lYODA src/profyoda.cc obj/ProfDriver.o  obj/AnyOption.o -Iinclude/AnyOption -Iinclude -I$(YODAPATH)/include  -o bin/profyoda

clean:
	rm -rf obj/*.o bin/test bin/profyoda lib/*
