check_PROGRAMS = constants do if for julia matrix ops point simple texture uniforms shvector while

INCLUDES = -I../src

LDADD = ../src/libsh.la

constants_SOURCES = constants.cpp
do_SOURCES = do.cpp
if_SOURCES = if.cpp
for_SOURCES = for.cpp
julia_SOURCES = julia.cpp
matrix_SOURCES = matrix.cpp
ops_SOURCES = ops.cpp
point_SOURCES = point.cpp
simple_SOURCES = simple.cpp
texture_SOURCES = texture.cpp
uniforms_SOURCES = uniforms.cpp
shvector_SOURCES = shvector.cpp
while_SOURCES = while.cpp

%.cpp: %.sht test.header test.footer
	cat test.header > $@
	cat $< >> $@
	cat test.footer >> $@

%.ps: %.dot
	dot -Tps -o $@ $<
