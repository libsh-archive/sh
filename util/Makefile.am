INCLUDES = -I../src $(LTDLINCL) 

lib_LTLIBRARIES = libshutil.la

libshutil_la_LIBADD = $(LIBLTDL)
libshutil_la_LDFLAGS = -export-dynamic -no-undefined

libshutil_la_SOURCES = ShPerlin.cpp \
                       ShWorley.cpp

incincdir = $(includedir)/sh

incinc_HEADERS = ShPerlin.hpp \
                 ShWorley.hpp \
                 ShUtilLib.hpp \
                 shutil.hpp
