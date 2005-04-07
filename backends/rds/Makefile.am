INCLUDES = -I$(top_srcdir)/src
# AM_CXXFLAGS = -DSH_DEBUG -DRDS_FAKE_LIMITS -g
AM_CXXFLAGS = -DRDS_FAKE_LIMITS -g

libshrds_ladir = $(prefix)/lib/sh

shbackenddir = $(prefix)/lib/sh

shbackend_LTLIBRARIES = libshrds.la

libshrds_la_LDFLAGS = -L$(prefix)/lib -module
libshrds_la_LIBADD = $(top_srcdir)/src/libsh.la $(top_srcdir)/backends/gl/libsharb.la
libshrds_la_SOURCES = DAG.cpp PDomTree.cpp FullSearch.cpp RDSBackend.cpp RDS.cpp CallBack.cpp Partition.cpp NextKSubset.cpp Limits.cpp Pass.cpp
