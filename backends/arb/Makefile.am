INCLUDES = -I$(top_srcdir)/src

shbackenddir = $(prefix)/lib/sh

shbackend_LTLIBRARIES = libsharb.la
libsharb_la_LDFLAGS = -L$(prefix)/lib -module
libsharb_la_LIBADD = $(top_srcdir)/src/libsh.la -lGL
libsharb_la_SOURCES = ShArb.cpp ShArb.hpp ShArbStream.cpp ShArbTexture.cpp ShArbReg.cpp \
		      ShArbExtensions.cpp ShArbMemory.cpp
