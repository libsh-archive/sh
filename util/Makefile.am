INCLUDES = $(LTDLINCL)

lib_LTLIBRARIES = libshutil.la

libshutil_la_LIBADD = $(LIBLTDL)
libshutil_la_LDFLAGS = -export-dynamic -no-undefined

libshutil_la_SOURCES = Perlin.cpp \
                       Worley.cpp

incincdir = $(includedir)/shutil

incinc_HEADERS = Perlin.hpp \
                 Worley.hpp \
                 ShUtilLib.hpp
