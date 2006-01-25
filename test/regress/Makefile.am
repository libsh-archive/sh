INCLUDES = -I$(top_srcdir)/src -I$(top_srcdir)/util
AM_CXXFLAGS = -Wall -g -O0
AM_LDFLAGS = -g -L$(top_builddir)/src -L$(top_builddir)/util
LDADD = -lsh -lshutil $(GL_LIBS)
TESTS = abs add cbrt ceil clamp comparisons cross dec_inc div dot exp \
	floor frac length_distance lerp lighting log logical mad \
	matrix_add matrix_div matrix_functions matrix_mul matrix_neg \
	matrix_sub max_min misc mod mul neg poly pow prod_sum rcp rnd \
	sign sqrt sub tex trig
check_PROGRAMS = $(TESTS)
BACKENDS = arb cc host glsl
EXTRA_DIST = shtest.py common.py tex_squares.png

common = test.cpp test.hpp

clean:
	@rm -f *.log
	@rm -f testlist
	@rm -f $(TESTS:=.cpp) ccstream.cpp

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
length_distance_SOURCES = length_distance.cpp $(common)
EXTRA_DIST += length_distance.cpp.py
lerp_SOURCES = lerp.cpp $(common)
EXTRA_DIST += lerp.cpp.py
lighting_SOURCES = lighting.cpp $(common)
EXTRA_DIST += lighting.cpp.py
log_SOURCES = log.cpp $(common)
EXTRA_DIST += log.cpp.py
logical_SOURCES = logical.cpp $(common)
EXTRA_DIST += logical.cpp.py
max_min_SOURCES = max_min.cpp $(common)
EXTRA_DIST += max_min.cpp.py
mad_SOURCES = mad.cpp $(common)
EXTRA_DIST += mad.cpp.py
matrix_add_SOURCES = matrix_add.cpp $(common)
EXTRA_DIST += matrix_add.cpp.py
matrix_div_SOURCES = matrix_div.cpp $(common)
EXTRA_DIST += matrix_div.cpp.py
matrix_functions_SOURCES = matrix_functions.cpp $(common)
EXTRA_DIST += matrix_functions.cpp.py
matrix_mul_SOURCES = matrix_mul.cpp $(common)
EXTRA_DIST += matrix_mul.cpp.py
matrix_neg_SOURCES = matrix_neg.cpp $(common)
EXTRA_DIST += matrix_neg.cpp.py
matrix_sub_SOURCES = matrix_sub.cpp $(common)
EXTRA_DIST += matrix_sub.cpp.py
misc_SOURCES = misc.cpp $(common)
EXTRA_DIST += misc.cpp.py
mod_SOURCES = mod.cpp $(common)
EXTRA_DIST += mod.cpp.py
mul_SOURCES = mul.cpp $(common)
EXTRA_DIST += mul.cpp.py
neg_SOURCES = neg.cpp $(common)
EXTRA_DIST += neg.cpp.py
poly_SOURCES = poly.cpp $(common)
EXTRA_DIST += poly.cpp.py
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
sqrt_SOURCES = sqrt.cpp $(common)
EXTRA_DIST += sqrt.cpp.py
sub_SOURCES = sub.cpp $(common)
EXTRA_DIST += sub.cpp.py
tex_SOURCES = tex.cpp $(common)
EXTRA_DIST += tex.cpp.py
trig_SOURCES = trig.cpp $(common)
EXTRA_DIST += trig.cpp.py
