INCLUDES = -I$(top_srcdir)/src

shbackenddir = $(prefix)/lib/sh

shbackend_LTLIBRARIES = libshcpu.la

libshcpu_la_LDFLAGS = -L$(prefix)/lib -module
libshcpu_la_LIBADD = $(top_srcdir)/src/libsh.la 
libshcpu_la_SOURCES = CPU.cpp
