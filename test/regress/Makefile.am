INCLUDES = -I../../src
AM_CXXFLAGS = -Wall -g -O0
AM_LDFLAGS = -g -L../../src
LDADD = -lsh -lGL
TESTS = add mul div cos sin
check_PROGRAMS = $(TESTS)
BACKENDS = arb gcc
EXTRA_DIST = streamtest.py common.py

clean:
	rm -f *.log
	rm -f testlist
	rm -f $(TESTS)
	rm -f *.o
	rm -f $(TESTS:=.cpp)
	rm -rf .libs

%.cpp: %.cpp.py streamtest.py common.py
	python $< > $@

add_SOURCES = add.cpp test.cpp test.hpp
EXTRA_DIST += add.cpp.py
mul_SOURCES = mul.cpp test.cpp test.hpp
EXTRA_DIST += mul.cpp.py
div_SOURCES = div.cpp test.cpp test.hpp
EXTRA_DIST += div.cpp.py
cos_SOURCES = cos.cpp test.cpp test.hpp
EXTRA_DIST += cos.cpp.py
sin_SOURCES = sin.cpp test.cpp test.hpp
EXTRA_DIST += sin.cpp.py

