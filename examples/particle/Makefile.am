LDADD = @GLUT_LIBS@ @GL_LIBS@ -lsh
AM_LDFLAGS = -L$(libdir)
INCLUDES = -I$(includedir) -I/home/ababkin/install/sh/include -L/home/ababkin/install/sh/lib

EXTRA_DIST = Makefile.win32 README

bin_PROGRAMS = shparticleex
shparticleex_SOURCES = main.cpp ShTrackball.cpp ShTrackball.hpp Camera.cpp Camera.hpp
