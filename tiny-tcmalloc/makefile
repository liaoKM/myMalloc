ver = release
allObj:=utilTest.o span.o pageCache.o testPageCache.o util.o centralCache.o testCentralCache.o threadCache.o tinyMalloc.o main.o
allEx:=testUtil testPageCache testCentralCache mainTest

ifeq ($(ver), debug)
	CXXFLAGS=-c -g  -Wall
else
	CXXFLAGS=-c -O2 -fopenmp -Wall -D NDEBUG
endif

all:testUtil testPageCache testCentralCache

testUtil:utilTest.o util.o
	g++ -Wall -fopenmp -o $@ $^
utilTest.o:util.h utilTest.cpp
	g++ $(CXXFLAGS) utilTest.cpp
util.o:util.h util.cpp
	g++ $(CXXFLAGS) util.cpp

testPageCache:testPageCache.o pageCache.o span.o util.o
	g++ -Wall -fopenmp -o $@ $^
testPageCache.o: testPageCache.cpp pageCache.h
	g++ $(CXXFLAGS) testPageCache.cpp
pageCache.o:pageCache.h pageCache.cpp
	g++ $(CXXFLAGS) pageCache.cpp
span.o:span.h span.cpp
	g++ $(CXXFLAGS) span.cpp 

threadCache.o:threadCache.cpp threadCache.h
	g++ $(CXXFLAGS) threadCache.cpp 

testCentralCache:testCentralCache.o centralCache.o pageCache.o span.o util.o
	g++ -Wall -fopenmp -o $@ $^
testCentralCache.o:testCentralCache.cpp centralCache.h
	g++ $(CXXFLAGS) testCentralCache.cpp
centralCache.o:centralCache.cpp centralCache.h
	g++ $(CXXFLAGS) centralCache.cpp

tinyMalloc.o:tinyMalloc.cpp tinyMalloc.h threadCache.h centralCache.h
	g++ $(CXXFLAGS) tinyMalloc.cpp
main.o:main.cpp tinyMalloc.h
	g++ $(CXXFLAGS) main.cpp
mainTest:main.o tinyMalloc.o threadCache.o util.o span.o pageCache.o centralCache.o 
	g++ -Wall -fopenmp -o $@ $^

.PHONY: clean

.PHONY: cleanObj

.PHONY: cleanTest

clean:cleanObj cleanTest

cleanObj:
	-rm $(allObj)
cleanTest:
	-rm $(allEx)
