INCLUDES = -I$(top_srcdir)/src

shbackenddir = $(prefix)/lib/sh

shbackend_LTLIBRARIES = libshgcc.la

libshgcc_la_LDFLAGS = -L$(prefix)/lib -module
libshgcc_la_LIBADD = $(top_srcdir)/src/libsh.la 
libshgcc_la_SOURCES = gcc.cpp gcc.hpp
