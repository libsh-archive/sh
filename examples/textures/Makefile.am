LDADD = @GLUT_LIBS@ @GL_LIBS@ -lsh
AM_LDFLAGS = -L$(libdir)
INCLUDES = -I$(includedir)

EXTRA_DIST = ks.png kd.png rustks.png rustkd.png kd1.png \
	kd2.png kd3.png kd4.png kd5.png kd6.png kd7.png kd8.png kd9.png

bin_PROGRAMS = shtexturesex
shtexturesex_SOURCES = main.cpp ShTrackball.cpp ShTrackball.hpp Camera.cpp Camera.hpp
