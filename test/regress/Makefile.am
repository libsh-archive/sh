INCLUDES = -I../../src
AM_CXXFLAGS = -Wall -g -O0
AM_LDFLAGS = -g -L../../src
LDADD = -lsh -lGL
TESTS = add add_im cbrt cmul cos csum div mul rnd sin tex lt le gt ge eq ne \
	mod floor ceil abs pow dot frac sign max max1 min min1 pos sat clamp \
	sqrt exp sub madd msub mmul mdiv mmod matrix_mul lit lit_im mad lerp \
	rsqrt rcp tan acos asin atan length exp2 exp10 log log2 log10 atan2 \
	cross sort cond matrix_add
check_PROGRAMS = $(TESTS)
BACKENDS = arb cc host glsl
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

add.cpp: add_test.py
madd.cpp: add_test.py
add_im.cpp: add_test.py
sub.cpp: sub_test.py
msub.cpp: sub_test.py
mul.cpp: mul_test.py
mmul.cpp: mul_test.py
div.cpp: div_test.py
mdiv.cpp: div_test.py
mod.cpp: mod_test.py
mmod.cpp: mod_test.py
lit.cpp: lit_test.py
lit_im.cpp: lit_test.py
matrix_mul.cpp: matrix_mul_test.py
matrix_add.cpp: matrix_add_test.py

common = test.cpp test.hpp

add_SOURCES = add.cpp add_test.py $(common)
EXTRA_DIST += add.cpp.py add_test.py
add_im_SOURCES = add_im.cpp add_test.py $(common)
EXTRA_DIST += add_im.cpp.py add_test.py
cbrt_SOURCES = cbrt.cpp $(common)
EXTRA_DIST += cbrt.cpp.py
cmul_SOURCES = cmul.cpp $(common)
EXTRA_DIST += cmul.cpp.py
cos_SOURCES = cos.cpp $(common)
EXTRA_DIST += cos.cpp.py
csum_SOURCES = csum.cpp $(common)
EXTRA_DIST += csum.cpp.py
div_SOURCES = div.cpp div_test.py $(common)
EXTRA_DIST += div.cpp.py div_test.py
mul_SOURCES = mul.cpp mul_test.py $(common)
EXTRA_DIST += mul.cpp.py mul_test.py
rnd_SOURCES = rnd.cpp $(common)
EXTRA_DIST += rnd.cpp.py
sin_SOURCES = sin.cpp $(common)
EXTRA_DIST += sin.cpp.py
tex_SOURCES = tex.cpp $(common)
EXTRA_DIST += tex.cpp.py
lt_SOURCES = lt.cpp $(common)
EXTRA_DIST += lt.cpp.py
le_SOURCES = le.cpp $(common)
EXTRA_DIST += le.cpp.py
gt_SOURCES = gt.cpp $(common)
EXTRA_DIST += gt.cpp.py
ge_SOURCES = ge.cpp $(common)
EXTRA_DIST += ge.cpp.py
eq_SOURCES = eq.cpp $(common)
EXTRA_DIST += eq.cpp.py
ne_SOURCES = ne.cpp $(common)
EXTRA_DIST += ne.cpp.py
mod_SOURCES = mod.cpp mod_test.py $(common)
EXTRA_DIST += mod.cpp.py mod_test.py
floor_SOURCES = floor.cpp $(common)
EXTRA_DIST += floor.cpp.py
ceil_SOURCES = ceil.cpp $(common)
EXTRA_DIST += ceil.cpp.py
abs_SOURCES = abs.cpp $(common)
EXTRA_DIST += abs.cpp.py
pow_SOURCES = pow.cpp $(common)
EXTRA_DIST += pow.cpp.py
dot_SOURCES = dot.cpp $(common)
EXTRA_DIST += dot.cpp.py
frac_SOURCES = frac.cpp $(common)
EXTRA_DIST += frac.cpp.py
sign_SOURCES = sign.cpp $(common)
EXTRA_DIST += sign.cpp.py
max_SOURCES = max.cpp $(common)
EXTRA_DIST += max.cpp.py
max1_SOURCES = max1.cpp $(common)
EXTRA_DIST += max1.cpp.py
min_SOURCES = min.cpp $(common)
EXTRA_DIST += min.cpp.py
min1_SOURCES = min1.cpp $(common)
EXTRA_DIST += min1.cpp.py
pos_SOURCES = pos.cpp $(common)
EXTRA_DIST += pos.cpp.py
sat_SOURCES = sat.cpp $(common)
EXTRA_DIST += sat.cpp.py
clamp_SOURCES = clamp.cpp $(common)
EXTRA_DIST += clamp.cpp.py
sqrt_SOURCES = sqrt.cpp $(common)
EXTRA_DIST += sqrt.cpp.py
exp_SOURCES = exp.cpp $(common)
EXTRA_DIST += exp.cpp.py
sub_SOURCES = sub.cpp sub_test.py $(common)
EXTRA_DIST += sub.cpp.py sub_test.py
madd_SOURCES = madd.cpp add_test.py $(common)
EXTRA_DIST += madd.cpp.py add_test.py
msub_SOURCES = msub.cpp sub_test.py $(common)
EXTRA_DIST += msub.cpp.py sub_test.py
mmul_SOURCES = mmul.cpp mul_test.py $(common)
EXTRA_DIST += mmul.cpp.py mul_test.py
mdiv_SOURCES = mdiv.cpp div_test.py $(common)
EXTRA_DIST += mdiv.cpp.py div_test.py
mmod_SOURCES = mmod.cpp mod_test.py $(common)
EXTRA_DIST += mmod.cpp.py mod_test.py
matrix_mul_SOURCES = matrix_mul.cpp matrix_mul_test.py $(common)
EXTRA_DIST += matrix_mul.cpp.py
matrix_add_SOURCES = matrix_add.cpp matrix_add_test.py $(common)
EXTRA_DIST += matrix_add.cpp.py
lit_SOURCES = lit.cpp lit_test.py $(common)
EXTRA_DIST += lit.cpp.py
lit_im_SOURCES = lit_im.cpp lit_test.py $(common)
EXTRA_DIST += lit_im.cpp.py
mad_SOURCES = mad.cpp $(common)
EXTRA_DIST += mad.cpp.py
lerp_SOURCES = lerp.cpp $(common)
EXTRA_DIST += lerp.cpp.py
rsqrt_SOURCES = rsqrt.cpp $(common)
EXTRA_DIST += rsqrt.cpp.py
rcp_SOURCES = rcp.cpp $(common)
EXTRA_DIST += rcp.cpp.py
tan_SOURCES = tan.cpp $(common)
EXTRA_DIST += tan.cpp.py
acos_SOURCES = acos.cpp $(common)
EXTRA_DIST += acos.cpp.py
asin_SOURCES = asin.cpp $(common)
EXTRA_DIST += asin.cpp.py
atan_SOURCES = atan.cpp $(common)
EXTRA_DIST += atan.cpp.py
length_SOURCES = length.cpp $(common)
EXTRA_DIST += length.cpp.py
exp2_SOURCES = exp2.cpp $(common)
EXTRA_DIST += exp2.cpp.py
exp10_SOURCES = exp10.cpp $(common)
EXTRA_DIST += exp10.cpp.py
log_SOURCES = log.cpp $(common)
EXTRA_DIST += log.cpp.py
log2_SOURCES = log2.cpp $(common)
EXTRA_DIST += log2.cpp.py
log10_SOURCES = log10.cpp $(common)
EXTRA_DIST += log10.cpp.py
atan2_SOURCES = atan2.cpp $(common)
EXTRA_DIST += atan2.cpp.py
cross_SOURCES = cross.cpp $(common)
EXTRA_DIST += cross.cpp.py
sort_SOURCES = sort.cpp $(common)
EXTRA_DIST += sort.cpp.py
cond_SOURCES = cond.cpp $(common)
EXTRA_DIST += cond.cpp.py
