SOURCES = $(wildcard *.cpp)
EXECUTABLES = $(SOURCES:.cpp=)
OBJECTS = $(SOURCES:.cpp=.o)
DEPENDS = $(SOURCES:.cpp=.d)
CPPFLAGS = -I../../src 
CXXFLAGS = -O2 -Wall
LDFLAGS = ../../src/libsh.la
LDADD = -lGL

all: $(EXECUTABLES)

clean:
	@rm -f *.o $(EXECUTABLES) *.d
	@rm -rf .libs

%: %.o
	@libtool --mode=link $(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $< $(LDFLAGS) $(LDADD) > /dev/null

%.o: %.cpp
	@echo Compiling $@
	@$(CXX) -c -o $@ $(CPPFLAGS) $(CXXFLAGS) -I../../src $<

%.d: %.cpp
	@$(CXX) -M $(CPPFLAGS) $< > $@.$$$$; \
            sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
            rm -f $@.$$$$

-include $(DEPENDS)
