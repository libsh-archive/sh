INCLUDES = -I$(top_srcdir)/src
AM_CXXFLAGS = -DSH_DEBUG

shbackenddir = $(prefix)/lib/sh

shbackend_LTLIBRARIES = libshrds.la

libshrds_la_LDFLAGS = -L$(prefix)/lib -module
libshrds_la_LIBADD = $(top_srcdir)/src/libsh.la 
libshrds_la_SOURCES = PDomTree.cpp Partition.cpp RDS.cpp
