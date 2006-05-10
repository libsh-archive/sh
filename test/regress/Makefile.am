INCLUDES = -I$(top_srcdir)/src
AM_CXXFLAGS = -Wall -g -O0
AM_LDFLAGS = -g -L$(top_builddir)/src/sh -L$(top_builddir)/src/shutil
LDADD = -lsh -lshutil $(GL_LIBS)
GENERATED_TESTS = \
	abs add cbrt ceil clamp comparisons cross dec_inc div dot exp \
	floor frac length_distance lerp lighting log logical mad \
	matrix_add matrix_div matrix_functions matrix_mul matrix_neg \
	matrix_sub max_min misc mod mul neg poly pow prod_sum rcp rnd \
	sign smooth_clamp sqrt sub tex trig
TESTS = $(GENERATED_TESTS) gather offset_stride scatter
check_PROGRAMS = $(TESTS)
BACKENDS = arb cc host glsl
EXTRA_DIST = $(GENERATED_TESTS:=.cpp.py) shtest.py common.py tex_squares.png

common = test.cpp test.hpp

clean:
	@rm -f *.log
	@rm -f testlist
	@rm -f $(GENERATED_TESTS:=.cpp) ccstream.cpp

%.cpp: %.cpp.py shtest.py common.py
	python $< > $@

abs_SOURCES = abs.cpp $(common)
add_SOURCES = add.cpp $(common)
cbrt_SOURCES = cbrt.cpp $(common)
ceil_SOURCES = ceil.cpp $(common)
clamp_SOURCES = clamp.cpp $(common)
comparisons_SOURCES = comparisons.cpp $(common)
cross_SOURCES = cross.cpp $(common)
dec_inc_SOURCES = dec_inc.cpp $(common)
div_SOURCES = div.cpp $(common)
dot_SOURCES = dot.cpp $(common)
exp_SOURCES = exp.cpp $(common)
floor_SOURCES = floor.cpp $(common)
frac_SOURCES = frac.cpp $(common)
gather_SOURCES = gather.cpp $(common)
length_distance_SOURCES = length_distance.cpp $(common)
lerp_SOURCES = lerp.cpp $(common)
lighting_SOURCES = lighting.cpp $(common)
log_SOURCES = log.cpp $(common)
logical_SOURCES = logical.cpp $(common)
max_min_SOURCES = max_min.cpp $(common)
mad_SOURCES = mad.cpp $(common)
matrix_add_SOURCES = matrix_add.cpp $(common)
matrix_div_SOURCES = matrix_div.cpp $(common)
matrix_functions_SOURCES = matrix_functions.cpp $(common)
matrix_mul_SOURCES = matrix_mul.cpp $(common)
matrix_neg_SOURCES = matrix_neg.cpp $(common)
matrix_sub_SOURCES = matrix_sub.cpp $(common)
misc_SOURCES = misc.cpp $(common)
mod_SOURCES = mod.cpp $(common)
mul_SOURCES = mul.cpp $(common)
neg_SOURCES = neg.cpp $(common)
offset_stride_SOURCES = offset_stride.cpp $(common)
poly_SOURCES = poly.cpp $(common)
pow_SOURCES = pow.cpp $(common)
prod_sum_SOURCES = prod_sum.cpp $(common)
rcp_SOURCES = rcp.cpp $(common)
rnd_SOURCES = rnd.cpp $(common)
scatter_SOURCES = scatter.cpp $(common)
sign_SOURCES = sign.cpp $(common)
smooth_clamp_SOURCES = smooth_clamp.cpp $(common)
sqrt_SOURCES = sqrt.cpp $(common)
sub_SOURCES = sub.cpp $(common)
tex_SOURCES = tex.cpp $(common)
trig_SOURCES = trig.cpp $(common)
