INCLUDES = -I$(top_srcdir)/src
AM_CXXFLAGS = -DSH_DEBUG -g

libshrds_la_PROGRAMS = rdsdriver
libshrds_ladir = $(prefix)/lib/sh

shbackenddir = $(prefix)/lib/sh

shbackend_LTLIBRARIES = libshrds.la

libshrds_la_LDFLAGS = -L$(prefix)/lib -module
libshrds_la_LIBADD = $(top_srcdir)/src/libsh.la 
libshrds_la_SOURCES = DAG.cpp PDomTree.cpp Partition.cpp RDS.cpp RDSBackend.cpp

rdsdriver_LDADD = libshrds.la
rdsdriver_SOURCES = RDSDriver.cpp