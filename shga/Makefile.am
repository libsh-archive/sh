INCLUDES = -I../src $(LTDLINCL) 

lib_LTLIBRARIES = libshga.la

libshga_la_LIBADD = $(LIBLTDL)
libshga_la_LDFLAGS = -export-dynamic -no-undefined

libshga_la_SOURCES = ShgaConstants.cpp 
                       

incincdir = $(includedir)/sh

incinc_HEADERS = ShgaBase.hpp \
								 ShgaBaseImpl.hpp \
								 ShgaConstants.hpp \
								 ShgaVersor.hpp \
								 ShgaVersorImpl.hpp \
                 shga.hpp
