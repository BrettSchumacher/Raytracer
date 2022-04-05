CXX=clang++
CXXFLAGS=-g -std=c++11 -Wall -pthread
LDFLAGS=-pthread
SOURCES := $(shell find . -name "*.cpp")
OBJFILES = $(addprefix ./, $(SOURCES:.cpp=.o))
SUBDIRS = $(shell find . -type d)
CWD = $(notdir $(CURDIR))
CPATH = -I./src

all: raytracer

double: 
	$(MAKE) CXXFLAGS="$(CXXFLAGS) -DUSE_DOUBLE=1"

clean: 
	rm -f *.o *.h.gch raytracer
	rm -f $(OBJFILES)

.PHONY: all clean

demo: raytracer
	./raytracer demo.txt

raytracer: src/main/Main.o $(OBJFILES)
	$(CXX) $(LDFLAGS) -o $(@) $(^)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(CPATH) -c -o $(@) $(<)
	
zip: clean
	rm -f raytracer.zip
	cd .. && zip $(CWD)/raytracer.zip -r $(CWD) -x ./$(CWD)/.vscode/\*