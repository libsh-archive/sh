LDADD = @GLUT_LIBS@ @GL_LIBS@ -lsh
AM_LDFLAGS = -L$(libdir)
INCLUDES = -I$(includedir)

EXTRA_DIST = Makefile.win32 README

bin_PROGRAMS = shglutex
shglutex_SOURCES = main.cpp ShTrackball.cpp ShTrackball.hpp Camera.cpp Camera.hpp
