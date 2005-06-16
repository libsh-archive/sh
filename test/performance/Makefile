SOURCES = $(wildcard *.cpp)
EXECUTABLES = $(SOURCES:.cpp=)
OBJECTS = $(SOURCES:.cpp=.o)
DEPENDS = $(SOURCES:.cpp=.d)
CPPFLAGS = -I../../src 
#CXXFLAGS = -g -O0 -Wall
CXXFLAGS = -O2 -Wall
LDFLAGS = ../../src/libsh.la

# LDFLAGS = ../../src/libsh.la

all: $(EXECUTABLES)

clean:
	@rm -f *.o $(EXECUTABLES) *.d *.log *.out *.prof *.temp
	@rm -f *.gprof gmon.out
	@rm -rf .libs

%: %.o
	@libtool --mode=link $(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $< $(LDFLAGS) > /dev/null

%.prof: %.gprof
	@echo Running $< to generate gprof data
	@./$<
	@libtool --mode=execute gprof $< > $@
	@rm -f gmon.out

%.gprof: %.gprof.o
	@libtool --mode=link $(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $< $(LDFLAGS) -pg > /dev/null

%.gprof.o: %.cpp
	@echo Compiling $@ with gprof support
	@$(CXX) -c -o $@ $(CPPFLAGS) $(CXXFLAGS) -pg -I../../src $<

%.o: %.cpp
	@echo Compiling $@
	@$(CXX) -c -o $@ $(CPPFLAGS) $(CXXFLAGS) -I../../src $<

%.d: %.cpp
	@$(CXX) -M $(CPPFLAGS) $< > $@.$$$$; \
            sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
            rm -f $@.$$$$

-include $(DEPENDS)
