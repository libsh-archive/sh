LDADD = @GLUT_LIBS@ @GL_LIBS@ -lsh
AM_LDFLAGS = -L$(libdir)
INCLUDES = -I$(includedir)

EXTRA_DIST = Makefile.win32 ks.png kd.png rustks.png rustkd.png

bin_PROGRAMS = shtexturesex
shtexturesex_SOURCES = main.cpp ShTrackball.cpp ShTrackball.hpp Camera.cpp Camera.hpp
