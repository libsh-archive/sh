INCLUDES = -I../../src
AM_CXXFLAGS = -Wall -g -O0
AM_LDFLAGS = -g -L../../src
LDADD = -lsh -lGL
TESTS = add add_im cbrt cmul cos csum div mul rnd sin tex lt le gt ge eq ne mod floor ceil abs pow dot frac sign max max1 min min1 pos sat clamp sqrt exp
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
lt_SOURCES = lt.cpp test.cpp test.hpp
EXTRA_DIST += lt.cpp.py
le_SOURCES = le.cpp test.cpp test.hpp
EXTRA_DIST += le.cpp.py
gt_SOURCES = gt.cpp test.cpp test.hpp
EXTRA_DIST += gt.cpp.py
ge_SOURCES = ge.cpp test.cpp test.hpp
EXTRA_DIST += ge.cpp.py
eq_SOURCES = eq.cpp test.cpp test.hpp
EXTRA_DIST += eq.cpp.py
ne_SOURCES = ne.cpp test.cpp test.hpp
EXTRA_DIST += ne.cpp.py
mod_SOURCES = mod.cpp test.cpp test.hpp
EXTRA_DIST += mod.cpp.py
floor_SOURCES = floor.cpp test.cpp test.hpp
EXTRA_DIST += floor.cpp.py
ceil_SOURCES = ceil.cpp test.cpp test.hpp
EXTRA_DIST += ceil.cpp.py
abs_SOURCES = abs.cpp test.cpp test.hpp
EXTRA_DIST += abs.cpp.py
pow_SOURCES = pow.cpp test.cpp test.hpp
EXTRA_DIST += pow.cpp.py
dot_SOURCES = dot.cpp test.cpp test.hpp
EXTRA_DIST += dot.cpp.py
frac_SOURCES = frac.cpp test.cpp test.hpp
EXTRA_DIST += frac.cpp.py
sign_SOURCES = sign.cpp test.cpp test.hpp
EXTRA_DIST += sign.cpp.py
max_SOURCES = max.cpp test.cpp test.hpp
EXTRA_DIST += max.cpp.py
max1_SOURCES = max1.cpp test.cpp test.hpp
EXTRA_DIST += max1.cpp.py
min_SOURCES = min.cpp test.cpp test.hpp
EXTRA_DIST += min.cpp.py
min1_SOURCES = min1.cpp test.cpp test.hpp
EXTRA_DIST += min1.cpp.py
pos_SOURCES = pos.cpp test.cpp test.hpp
EXTRA_DIST += pos.cpp.py
sat_SOURCES = sat.cpp test.cpp test.hpp
EXTRA_DIST += sat.cpp.py
clamp_SOURCES = clamp.cpp test.cpp test.hpp
EXTRA_DIST += clamp.cpp.py
sqrt_SOURCES = sqrt.cpp test.cpp test.hpp
EXTRA_DIST += sqrt.cpp.py
exp_SOURCES = exp.cpp test.cpp test.hpp
EXTRA_DIST += exp.cpp.py
