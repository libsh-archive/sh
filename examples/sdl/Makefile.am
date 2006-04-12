LDADD = @GLUT_LIBS@ @GL_LIBS@ @SDL_LIBS@ -lsh
AM_LDFLAGS = -L$(libdir)
INCLUDES = -I$(includedir)

EXTRA_DIST = README

bin_PROGRAMS = shsdlex
shsdlex_CXXFLAGS = @SDL_CFLAGS@
shsdlex_SOURCES = main.cpp Trackball.cpp Trackball.hpp Camera.cpp Camera.hpp
