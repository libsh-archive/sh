INCLUDES = -I../../src
AM_CXXFLAGS = -Wall -g -O0
AM_LDFLAGS = -g -L../../src
LDADD = -lsh -lGL
TESTS = add add_im cbrt cmul cos csum div mul rnd sin tex
check_PROGRAMS = $(TESTS)
BACKENDS = arb cc host
EXTRA_DIST = shtest.py common.py

checkall:
	for i in $(BACKENDS) ; do SH_REGRESS_BACKEND=$$i $(MAKE) check ; done

clean:
	rm -f *.log
	rm -f testlist
	rm -f $(TESTS)
	rm -f *.o
	rm -f $(TESTS:=.cpp)
	rm -rf .libs

%.cpp: %.cpp.py shtest.py common.py
	python $< > $@

add_SOURCES = add.cpp test.cpp test.hpp
EXTRA_DIST += add.cpp.py add_test.py
add_im_SOURCES = add_im.cpp test.cpp test.hpp
EXTRA_DIST += add_im.cpp.py add_test.py
cbrt_SOURCES = cbrt.cpp test.cpp test.hpp
EXTRA_DIST += cbrt.cpp.py
cmul_SOURCES = cmul.cpp test.cpp test.hpp
EXTRA_DIST += cmul.cpp.py
cos_SOURCES = cos.cpp test.cpp test.hpp
EXTRA_DIST += cos.cpp.py
csum_SOURCES = csum.cpp test.cpp test.hpp
EXTRA_DIST += csum.cpp.py
div_SOURCES = div.cpp test.cpp test.hpp
EXTRA_DIST += div.cpp.py
mul_SOURCES = mul.cpp test.cpp test.hpp
EXTRA_DIST += mul.cpp.py
rnd_SOURCES = rnd.cpp test.cpp test.hpp
EXTRA_DIST += rnd.cpp.py
sin_SOURCES = sin.cpp test.cpp test.hpp
EXTRA_DIST += sin.cpp.py
tex_SOURCES = tex.cpp test.cpp test.hpp
EXTRA_DIST += tex.cpp.py

