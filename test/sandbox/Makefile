SOURCES = $(wildcard *.cpp)
EXECUTABLES = $(SOURCES:.cpp=)
OBJECTS = $(SOURCES:.cpp=.o)
DEPENDS = $(SOURCES:.cpp=.d)
CPPFLAGS = -I../../src 
#CXXFLAGS = -g -O0 -Wall
CXXFLAGS = -g -O2 -Wall
LDFLAGS = ../../src/sh/libsh.la -lGL -lglut

all:

clean:
	@rm -f *.o $(EXECUTABLES) *.d

%: %.o
	@libtool --mode=link $(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $< $(LDFLAGS) > /dev/null

%.o: %.cpp
	@echo Compiling $@
	@$(CXX) -c -o $@ $(CPPFLAGS) $(CXXFLAGS) -I../../src $<

%.d: %.cpp
	@$(CXX) -M $(CPPFLAGS) $< > $@.$$$$; \
            sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
            rm -f $@.$$$$

-include $(DEPENDS)
