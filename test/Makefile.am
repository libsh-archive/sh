check_PROGRAMS = algebra combine connect constants copy do if for julia matrix ops point \
		 rch simple texture uniforms shvector while neg stream io2temp longtuple ref

INCLUDES = -I../src

LDADD = ../src/libsh.la

algebra_SOURCES = algebra.cpp
combine_SOURCES = combine.cpp
connect_SOURCES = connect.cpp
constants_SOURCES = constants.cpp
copy_SOURCES = copy.cpp
do_SOURCES = do.cpp
if_SOURCES = if.cpp
for_SOURCES = for.cpp
julia_SOURCES = julia.cpp
matrix_SOURCES = matrix.cpp
neg_SOURCES = neg.cpp
rch_SOURCES = rch.cpp
ref_SOURCES = ref.cpp
ops_SOURCES = ops.cpp
point_SOURCES = point.cpp
simple_SOURCES = simple.cpp
texture_SOURCES = texture.cpp
uniforms_SOURCES = uniforms.cpp
shvector_SOURCES = shvector.cpp
stream_SOURCES = stream.cpp
while_SOURCES = while.cpp
io2temp_SOURCES = io2temp.cpp
longtuple_SOURCES = longtuple.cpp

%.cpp: %.sht test.header test.footer
	cat test.header > $@
	cat $< >> $@
	cat test.footer >> $@

%.ps: %.dot
	dot -Tps -o $@ $<
