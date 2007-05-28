# Change this location to where you installed Sh
# E.g. /usr/local
SH_INSTALL_DIR = ${HOME}/install-sh

SOURCES = $(wildcard [A-Z]*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)
DEPENDS = $(SOURCES:.cpp=.d) 
LIBS =  -lGL -lGLU -lsh -lshutil -lglut
LDFLAGS = -L$(SH_INSTALL_DIR)/lib $(LIBS) `pkg-config libxml++-2.6 --libs` `wx-config --libs std,gl`
INCLUDES = -I$(SH_INSTALL_DIR)/include `pkg-config libxml++-2.6 --cflags` `wx-config --cflags std,gl`
CPPFLAGS = $(INCLUDES) 
CXXFLAGS = $(CPPFLAGS) -Wall -g -O2
CXX = g++
MAIN = shgo 

all: $(MAIN) 

depend: $(DEPENDS)

clean:
	rm -f *.o *.d $(MAIN) [^h]*.png *.svg *.ps *.dot *.vars *~

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

