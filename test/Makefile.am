check_PROGRAMS = do if for matrix ops point simple texture uniforms shvector while

AM_CPPFLAGS = -I../src

LDADD = -lGL ../../sm/src/libsm.la ../src/libsh.la ../backends/sm/ShSmBackend.o ../backends/arb/ShArb.o
# LDFLAGS = -all-static

do_SOURCES = do.cpp
if_SOURCES = if.cpp
for_SOURCES = for.cpp
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
