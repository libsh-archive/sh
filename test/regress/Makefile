CXXFLAGS = -Wall -g -O0 -I../../src/
LDFLAGS = -g -L../../src
LDLIBS = -lsh -lGL
TESTS = ADD MUL DIV DOT NORM XPD POW

all: $(TESTS)

clean:
	rm -f testlist
	rm -f $(TESTS)
	rm -f *.o
	rm -f $(TESTS:=.cpp)
	rm -rf .libs

testlist: ./regress.py
	./regress.py > testlist

$(TESTS:=.cpp): %.cpp: testlist

$(TESTS): %: %.o test.o
	libtool --mode=link $(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

# include testlist
