# Change this location to where you installed Sh
# E.g. /usr/local
SH_INSTALL_DIR = ${HOME}/install-sh

SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)
DEPENDS = $(SOURCES:.cpp=.d)
LIBS = -lglut -lGL -lGLU -lsh -lshutil -lpthread
LDFLAGS = -L$(SH_INSTALL_DIR)/lib $(LIBS) `pkg-config libxml++-2.6 --libs` `wx-config --libs std,gl`
INCLUDES = -I$(SH_INSTALL_DIR)/include `pkg-config libxml++-2.6 --cflags` `wx-config --cflags std,gl`
CPPFLAGS = $(INCLUDES) 
CXXFLAGS = $(CPPFLAGS) -Wall -g -O2
CXX = g++
MAIN = shimpl 

all: $(MAIN)

depend: $(DEPENDS)

testclean:
	rm *.csv *.dot *.vars *.ps; rm [^h]*.png

clean:
	rm -f -- *.o *.d $(MAIN) [^h]*.png *.vars *.ps *.dot *.csv *0x*

$(MAIN): $(OBJECTS)
	libtool --mode=link $(CXX) -o $@ $(OBJECTS) $(LDFLAGS)

%.o: %.cpp
	$(CXX) -o $@ -c $(CXXFLAGS) $<

%.d: %.cpp
	set -e; $(CC) -M $(CPPFLAGS) $< \
                  | sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@; \
                [ -s $@ ] || rm -f $@
	$(CXX) $(CXXFLAGS) -M $< > $@


include $(DEPENDS)

