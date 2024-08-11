# Every new app needs a name!
appname := octopus

# Compiler flags: use C++11 and POSIX threads
CXX := g++
CXXFLAGS := -I $(srcdir)/ -std=c++11 -pthread 
LDFLAGS :=
LDLIBS := 

# Sources
srcdir := .
srcfiles := $(wildcard *.cpp)
objects  := $(patsubst %.cpp, %.o, $(srcfiles))

# Targets
all: $(appname)

$(appname): $(objects)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $(appname) $(objects) $(LDLIBS)

depend: .depend

.depend: $(srcfiles)
	rm -f ./.depend
	$(CXX) $(CXXFLAGS) -MM $^>>./.depend;

clean:
	rm -f $(objects)

dist-clean: clean
	rm -f *~ .depend

include .depend