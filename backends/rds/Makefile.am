INCLUDES = -I$(top_srcdir)/src
# AM_CXXFLAGS = -DSH_DEBUG -DRDS_FAKE_LIMITS -g
AM_CXXFLAGS = -DRDS_FAKE_LIMITS -g

libshrds_la_PROGRAMS = rdsdriver
libshrds_ladir = $(prefix)/lib/sh

shbackenddir = $(prefix)/lib/sh

shbackend_LTLIBRARIES = libshrds.la

libshrds_la_LDFLAGS = -L$(prefix)/lib -module
libshrds_la_LIBADD = $(top_srcdir)/src/libsh.la
libshrds_la_SOURCES = DAG.cpp PDomTree.cpp RDS.cpp RDSBackend.cpp Limits.cpp

rdsdriver_LDADD = -lGL -lshrds
rdsdriver_SOURCES = RDSDriver.cpp
