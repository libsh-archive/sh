INCLUDES = -I$(top_srcdir)/src

shbackenddir = $(prefix)/lib/sh

shbackend_LTLIBRARIES = libshsm.la
libshsm_la_LDFLAGS = -L$(prefix)/lib -module
libshsm_la_LIBADD = $(top_srcdir)/src/libsh.la -lsm
libshsm_la_SOURCES = ShSmBackend.cpp ShSmBackend.hpp
