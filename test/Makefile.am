check_PROGRAMS = algebra combine connect constants copy do if for julia matrix ops point \
		 rch simple texture uniforms shvector while neg stream io2temp ref mem \
		 longtuple inout error stream_inverse kill stream_timing manip

INCLUDES = -I../src

AM_LDFLAGS = -L/usr/X11R6/lib
LDADD =  -lGL ../src/libsh.la

AM_CXXFLAGS = -g -O0

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
mem_SOURCES = mem.cpp
neg_SOURCES = neg.cpp
rch_SOURCES = rch.cpp
ref_SOURCES = ref.cpp
ops_SOURCES = ops.cpp
point_SOURCES = point.cpp
simple_SOURCES = simple.cpp
texture_SOURCES = texture.cpp
uniforms_SOURCES = uniforms.cpp
stream_inverse_SOURCES = stream_inverse.cpp
stream_inverse_LDADD = ../src/libsh.la -lX11 -lGL
stream_LDADD = ../src/libsh.la -lX11 -lGL
shvector_SOURCES = shvector.cpp
stream_SOURCES = stream.cpp
stream_timing_SOURCES = stream_timing.cpp
stream_timing_LDADD = ../src/libsh.la -lX11 -lGL
while_SOURCES = while.cpp
io2temp_SOURCES = io2temp.cpp
longtuple_SOURCES = longtuple.cpp
inout_SOURCES = inout.cpp
error_SOURCES = error.cpp
kill_SOURCES = kill.cpp
manip_SOURCES = manip.cpp

%.cpp: %.sht test.header test.footer
	cat test.header > $@
	cat $< >> $@
	cat test.footer >> $@

%.ps: %.dot
	dot -Tps -o $@ $<
