CXXFLAGS = -Wall -g -O0 -I../../src/
LDFLAGS = -g -L../../src
LDLIBS = -lsh -lGL
TESTS = ADD MUL DIV DOT NORM XPD POW
BACKENDS = arb cpu

all: $(TESTS)

clean:
	rm -f testlist
	rm -f $(TESTS)
	rm -f *.o
	rm -f $(TESTS:=.cpp)
	rm -rf .libs

check: $(TESTS)
	for j in $(BACKENDS) ; do echo $$j backend ; for i in $(TESTS) ; do ./$$i $$j 2> /dev/null ; done ; done

testlist: ./regress.py
	./regress.py > testlist

$(TESTS:=.cpp): %.cpp: testlist

$(TESTS): %: %.o test.o
	libtool --mode=link $(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

# include testlist
