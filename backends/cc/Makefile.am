INCLUDES = -I$(top_srcdir)/src

shbackenddir = $(prefix)/lib/sh

shbackend_LTLIBRARIES = libshcc.la

libshcc_la_LDFLAGS = -L$(prefix)/lib -module
libshcc_la_LIBADD = $(top_srcdir)/src/libsh.la 
libshcc_la_SOURCES = Cc.cpp Cc.hpp CcEmit.cpp 
