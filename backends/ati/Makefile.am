INCLUDES = -I$(top_srcdir)/src

shbackenddir = $(prefix)/lib/sh

shbackend_LTLIBRARIES = libshati.la
libshati_la_LDFLAGS = -L$(prefix)/lib -module
libshati_la_LIBADD = $(top_srcdir)/src/libsh.la -lGL
libshati_la_SOURCES = ShAti.cpp ShAti.hpp extensions.c
