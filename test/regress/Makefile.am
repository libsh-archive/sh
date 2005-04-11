INCLUDES = -I../../src
AM_CXXFLAGS = -Wall -g -O0
AM_LDFLAGS = -g -L../../src
LDADD = -lsh $(GL_LIBS)
TESTS = abs add cbrt ceil clamp comparisons cond cross dec_inc div dot \
	exp floor frac length lerp lit log mad matrix_add matrix_div \
	matrix_mul matrix_neg matrix_sub max_min mod mul neg pow \
	prod_sum rcp rnd sign sort sqrt sub tex trace trig
check_PROGRAMS = $(TESTS)
BACKENDS = arb cc host glsl
EXTRA_DIST = shtest.py common.py

common = test.cpp test.hpp

checkall:
	for i in $(BACKENDS) ; do SH_REGRESS_BACKEND=$$i $(MAKE) check ; done

clean:
	rm -f *.log
	rm -f testlist
	rm -f $(TESTS)
	rm -f *.o *.pyc
	rm -f $(TESTS:=.cpp) ccstream.cpp
	rm -rf .libs

%.cpp: %.cpp.py shtest.py common.py
	python $< > $@

abs_SOURCES = abs.cpp $(common)
EXTRA_DIST += abs.cpp.py
add_SOURCES = add.cpp $(common)
EXTRA_DIST += add.cpp.py
cbrt_SOURCES = cbrt.cpp $(common)
EXTRA_DIST += cbrt.cpp.py
ceil_SOURCES = ceil.cpp $(common)
EXTRA_DIST += ceil.cpp.py
clamp_SOURCES = clamp.cpp $(common)
EXTRA_DIST += clamp.cpp.py
comparisons_SOURCES = comparisons.cpp $(common)
EXTRA_DIST += comparisons.cpp.py
cond_SOURCES = cond.cpp $(common)
EXTRA_DIST += cond.cpp.py
cross_SOURCES = cross.cpp $(common)
EXTRA_DIST += cross.cpp.py
dec_inc_SOURCES = dec_inc.cpp $(common)
EXTRA_DIST += dec_inc.cpp.py
div_SOURCES = div.cpp $(common)
EXTRA_DIST += div.cpp.py
dot_SOURCES = dot.cpp $(common)
EXTRA_DIST += dot.cpp.py
exp_SOURCES = exp.cpp $(common)
EXTRA_DIST += exp.cpp.py
floor_SOURCES = floor.cpp $(common)
EXTRA_DIST += floor.cpp.py
frac_SOURCES = frac.cpp $(common)
EXTRA_DIST += frac.cpp.py
length_SOURCES = length.cpp $(common)
EXTRA_DIST += length.cpp.py
lerp_SOURCES = lerp.cpp $(common)
EXTRA_DIST += lerp.cpp.py
lit_SOURCES = lit.cpp $(common)
EXTRA_DIST += lit.cpp.py
log_SOURCES = log.cpp $(common)
EXTRA_DIST += log.cpp.py
max_min_SOURCES = max_min.cpp $(common)
EXTRA_DIST += max_min.cpp.py
mad_SOURCES = mad.cpp $(common)
EXTRA_DIST += mad.cpp.py
matrix_add_SOURCES = matrix_add.cpp $(common)
EXTRA_DIST += matrix_add.cpp.py
matrix_div_SOURCES = matrix_div.cpp $(common)
EXTRA_DIST += matrix_div.cpp.py
EXTRA_DIST += matrix_msub.cpp.py
matrix_mul_SOURCES = matrix_mul.cpp $(common)
EXTRA_DIST += matrix_mul.cpp.py
matrix_neg_SOURCES = matrix_neg.cpp $(common)
EXTRA_DIST += matrix_neg.cpp.py
matrix_sub_SOURCES = matrix_sub.cpp $(common)
EXTRA_DIST += matrix_sub.cpp.py
EXTRA_DIST += mmul.cpp.py
mod_SOURCES = mod.cpp $(common)
EXTRA_DIST += mod.cpp.py
mul_SOURCES = mul.cpp $(common)
EXTRA_DIST += mul.cpp.py
neg_SOURCES = neg.cpp $(common)
EXTRA_DIST += neg.cpp.py
pow_SOURCES = pow.cpp $(common)
EXTRA_DIST += pow.cpp.py
prod_sum_SOURCES = prod_sum.cpp $(common)
EXTRA_DIST += prod_sum.cpp.py
rcp_SOURCES = rcp.cpp $(common)
EXTRA_DIST += rcp.cpp.py
rnd_SOURCES = rnd.cpp $(common)
EXTRA_DIST += rnd.cpp.py
sign_SOURCES = sign.cpp $(common)
EXTRA_DIST += sign.cpp.py
sort_SOURCES = sort.cpp $(common)
EXTRA_DIST += sort.cpp.py
sqrt_SOURCES = sqrt.cpp $(common)
EXTRA_DIST += sqrt.cpp.py
sub_SOURCES = sub.cpp $(common)
EXTRA_DIST += sub.cpp.py
tex_SOURCES = tex.cpp $(common)
EXTRA_DIST += tex.cpp.py
trace_SOURCES = trace.cpp $(common)
EXTRA_DIST += trace.cpp.py
trig_SOURCES = trig.cpp $(common)
EXTRA_DIST += trig.cpp.py
