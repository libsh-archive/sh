CXXFLAGS = -Wall -g -O0 -I../../src/
LDFLAGS = -g -L../../src
LDLIBS = -lsh -lGL
TESTS = ADD MUL DIV SLT SLE SGT SGE SEQ SNE ABS ACOS ASIN ATAN ATAN2 CEIL COS DOT \
	EXP EXP2 EXP10 FLR FRAC LRP MAD MAX MIN MOD LOG LOG2 LOG10 POW SIN SQRT TAN NORM XPD COND
BACKENDS = arb cpu

all: $(TESTS)

clean:
	rm -f testlist
	rm -f $(TESTS)
	rm -f *.o
	rm -f $(TESTS:=.cpp)
	rm -rf .libs

%.check: %
	@for j in $(BACKENDS) ; do ./$< $$j 2> $*.$$j.log ; done

check: $(TESTS:=.check)

testlist: ./regress.py
	./regress.py > testlist

$(TESTS:=.cpp): %.cpp: testlist

$(TESTS): %: %.o test.o
	libtool --mode=link $(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

# include testlist
