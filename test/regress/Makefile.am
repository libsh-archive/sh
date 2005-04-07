INCLUDES = -I../../src
AM_CXXFLAGS = -Wall -g -O0
AM_LDFLAGS = -g -L../../src
LDADD = -lsh $(GL_LIBS)
TESTS = abs acos add asin atan atan2 cbrt ceil clamp cmul comparisons \
	cond cos cross csum div dot exp floor frac length lerp lit log \
	mad matrix_add matrix_div matrix_mul matrix_neg matrix_sub max \
	min mod mul neg pow rcp rnd rsqrt sign sin sort sqrt sub tan \
	tex trace
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
	rm -f $(TESTS:=.cpp)
	rm -rf .libs

%.cpp: %.cpp.py shtest.py common.py
	python $< > $@

abs_SOURCES = abs.cpp $(common)
EXTRA_DIST += abs.cpp.py
acos_SOURCES = acos.cpp $(common)
EXTRA_DIST += acos.cpp.py
add_SOURCES = add.cpp $(common)
EXTRA_DIST += add.cpp.py
asin_SOURCES = asin.cpp $(common)
EXTRA_DIST += asin.cpp.py
atan_SOURCES = atan.cpp $(common)
EXTRA_DIST += atan.cpp.py
atan2_SOURCES = atan2.cpp $(common)
EXTRA_DIST += atan2.cpp.py
cbrt_SOURCES = cbrt.cpp $(common)
EXTRA_DIST += cbrt.cpp.py
ceil_SOURCES = ceil.cpp $(common)
EXTRA_DIST += ceil.cpp.py
clamp_SOURCES = clamp.cpp $(common)
EXTRA_DIST += clamp.cpp.py
cmul_SOURCES = cmul.cpp $(common)
EXTRA_DIST += cmul.cpp.py
comparisons_SOURCES = comparisons.cpp $(common)
EXTRA_DIST += comparisons.cpp.py
cond_SOURCES = cond.cpp $(common)
EXTRA_DIST += cond.cpp.py
cos_SOURCES = cos.cpp $(common)
EXTRA_DIST += cos.cpp.py
cross_SOURCES = cross.cpp $(common)
EXTRA_DIST += cross.cpp.py
csum_SOURCES = csum.cpp $(common)
EXTRA_DIST += csum.cpp.py
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
max_SOURCES = max.cpp $(common)
EXTRA_DIST += max.cpp.py
min_SOURCES = min.cpp $(common)
EXTRA_DIST += min.cpp.py
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
rcp_SOURCES = rcp.cpp $(common)
EXTRA_DIST += rcp.cpp.py
rnd_SOURCES = rnd.cpp $(common)
EXTRA_DIST += rnd.cpp.py
rsqrt_SOURCES = rsqrt.cpp $(common)
EXTRA_DIST += rsqrt.cpp.py
sign_SOURCES = sign.cpp $(common)
EXTRA_DIST += sign.cpp.py
sin_SOURCES = sin.cpp $(common)
EXTRA_DIST += sin.cpp.py
sort_SOURCES = sort.cpp $(common)
EXTRA_DIST += sort.cpp.py
sqrt_SOURCES = sqrt.cpp $(common)
EXTRA_DIST += sqrt.cpp.py
sub_SOURCES = sub.cpp $(common)
EXTRA_DIST += sub.cpp.py
tan_SOURCES = tan.cpp $(common)
EXTRA_DIST += tan.cpp.py
tex_SOURCES = tex.cpp $(common)
EXTRA_DIST += tex.cpp.py
trace_SOURCES = trace.cpp $(common)
EXTRA_DIST += trace.cpp.py
