INCLUDES = -I../../src
AM_CXXFLAGS = -Wall -g -O0
AM_LDFLAGS = -g -L../../src
LDADD = -lsh -lGL
TESTS = abs acos add add_im asin atan atan2 cbrt ceil clamp cmul cond \
	cos cross csum div dot exp exp10 exp2 eq floor frac ge gt le \
	length lerp lerp_im lit lit_im log log2 log10 lt mad madd \
	matrix_add matrix_div matrix_madd matrix_mdiv matrix_mmul \
	matrix_msub matrix_mul matrix_neg matrix_sub max max1 mdiv \
	min min1 mmod mmul mod msub mul ne neg pos pow rcp rnd rsqrt \
	sat sign sin sort sqrt sub tan tex
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
	rm -f *.o
	rm -f $(TESTS:=.cpp)
	rm -rf .libs

%.cpp: %.cpp.py shtest.py common.py
	python $< > $@

add.cpp: add_test.py
add_im.cpp: add_test.py
div.cpp: div_test.py
lerp.cpp: lerp_test.py
lerp_im.cpp: lerp_test.py
lit.cpp: lit_test.py
lit_im.cpp: lit_test.py
madd.cpp: add_test.py
matrix_add.cpp: matrix_add_test.py
matrix_div.cpp: matrix_div_test.py
matrix_madd.cpp: matrix_add_test.py
matrix_mdiv.cpp: matrix_div_test.py
matrix_mmul.cpp: matrix_mul_test.py
matrix_msub.cpp: matrix_sub_test.py
matrix_mul.cpp: matrix_mul_test.py
matrix_sub.cpp: matrix_sub_test.py
mdiv.cpp: div_test.py
mmod.cpp: mod_test.py
mmul.cpp: mul_test.py
mod.cpp: mod_test.py
msub.cpp: sub_test.py
mul.cpp: mul_test.py
sub.cpp: sub_test.py

EXTRA_DIST += add_test.py div_test.py lerp_test.py lit_test.py \
	mod_test.py mul_test.py sub_test.py matrix_add_test.py \
	matrix_div_test.py matrix_mul_test.py matrix_sub_test.py

abs_SOURCES = abs.cpp $(common)
EXTRA_DIST += abs.cpp.py
acos_SOURCES = acos.cpp $(common)
EXTRA_DIST += acos.cpp.py
add_SOURCES = add.cpp $(common)
EXTRA_DIST += add.cpp.py
add_im_SOURCES = add_im.cpp $(common)
EXTRA_DIST += add_im.cpp.py
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
eq_SOURCES = eq.cpp $(common)
EXTRA_DIST += eq.cpp.py
exp_SOURCES = exp.cpp $(common)
EXTRA_DIST += exp.cpp.py
exp10_SOURCES = exp10.cpp $(common)
EXTRA_DIST += exp10.cpp.py
exp2_SOURCES = exp2.cpp $(common)
EXTRA_DIST += exp2.cpp.py
floor_SOURCES = floor.cpp $(common)
EXTRA_DIST += floor.cpp.py
frac_SOURCES = frac.cpp $(common)
EXTRA_DIST += frac.cpp.py
ge_SOURCES = ge.cpp $(common)
EXTRA_DIST += ge.cpp.py
gt_SOURCES = gt.cpp $(common)
EXTRA_DIST += gt.cpp.py
le_SOURCES = le.cpp $(common)
EXTRA_DIST += le.cpp.py
length_SOURCES = length.cpp $(common)
EXTRA_DIST += length.cpp.py
lerp_SOURCES = lerp.cpp $(common)
EXTRA_DIST += lerp.cpp.py
lerp_im_SOURCES = lerp_im.cpp $(common)
EXTRA_DIST += lerp_im.cpp.py
lit_SOURCES = lit.cpp $(common)
EXTRA_DIST += lit.cpp.py
lit_im_SOURCES = lit_im.cpp $(common)
EXTRA_DIST += lit_im.cpp.py
log_SOURCES = log.cpp $(common)
EXTRA_DIST += log.cpp.py
log10_SOURCES = log10.cpp $(common)
EXTRA_DIST += log10.cpp.py
log2_SOURCES = log2.cpp $(common)
EXTRA_DIST += log2.cpp.py
lt_SOURCES = lt.cpp $(common)
EXTRA_DIST += lt.cpp.py
max_SOURCES = max.cpp $(common)
EXTRA_DIST += max.cpp.py
max1_SOURCES = max1.cpp $(common)
EXTRA_DIST += max1.cpp.py
min_SOURCES = min.cpp $(common)
EXTRA_DIST += min.cpp.py
min1_SOURCES = min1.cpp $(common)
EXTRA_DIST += min1.cpp.py
mad_SOURCES = mad.cpp $(common)
EXTRA_DIST += mad.cpp.py
madd_SOURCES = madd.cpp $(common)
EXTRA_DIST += madd.cpp.py
matrix_add_SOURCES = matrix_add.cpp $(common)
EXTRA_DIST += matrix_add.cpp.py
matrix_div_SOURCES = matrix_div.cpp $(common)
EXTRA_DIST += matrix_div.cpp.py
matrix_madd_SOURCES = matrix_madd.cpp $(common)
EXTRA_DIST += matrix_madd.cpp.py
matrix_mdiv_SOURCES = matrix_mdiv.cpp $(common)
EXTRA_DIST += matrix_mdiv.cpp.py
matrix_mmul_SOURCES = matrix_mmul.cpp $(common)
EXTRA_DIST += matrix_mmul.cpp.py
matrix_msub_SOURCES = matrix_msub.cpp $(common)
EXTRA_DIST += matrix_msub.cpp.py
matrix_mul_SOURCES = matrix_mul.cpp $(common)
EXTRA_DIST += matrix_mul.cpp.py
matrix_neg_SOURCES = matrix_neg.cpp $(common)
EXTRA_DIST += matrix_neg.cpp.py
matrix_sub_SOURCES = matrix_sub.cpp $(common)
EXTRA_DIST += matrix_sub.cpp.py
mdiv_SOURCES = mdiv.cpp $(common)
EXTRA_DIST += mdiv.cpp.py
mmod_SOURCES = mmod.cpp $(common)
EXTRA_DIST += mmod.cpp.py
mmul_SOURCES = mmul.cpp $(common)
EXTRA_DIST += mmul.cpp.py
mod_SOURCES = mod.cpp $(common)
EXTRA_DIST += mod.cpp.py
msub_SOURCES = msub.cpp $(common)
EXTRA_DIST += msub.cpp.py
mul_SOURCES = mul.cpp $(common)
EXTRA_DIST += mul.cpp.py
ne_SOURCES = ne.cpp $(common)
EXTRA_DIST += ne.cpp.py
neg_SOURCES = neg.cpp $(common)
EXTRA_DIST += neg.cpp.py
pos_SOURCES = pos.cpp $(common)
EXTRA_DIST += pos.cpp.py
pow_SOURCES = pow.cpp $(common)
EXTRA_DIST += pow.cpp.py
rcp_SOURCES = rcp.cpp $(common)
EXTRA_DIST += rcp.cpp.py
rnd_SOURCES = rnd.cpp $(common)
EXTRA_DIST += rnd.cpp.py
rsqrt_SOURCES = rsqrt.cpp $(common)
EXTRA_DIST += rsqrt.cpp.py
sat_SOURCES = sat.cpp $(common)
EXTRA_DIST += sat.cpp.py
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
