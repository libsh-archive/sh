INCLUDES = -I$(top_srcdir)/src

shbackenddir = $(prefix)/lib/sh

shbackend_LTLIBRARIES = libsharb.la libshati.la

GLBACKEND_SOURCES = GlBackend.cpp GlBackend.hpp \
		    GlTextureName.hpp GlTextureName.cpp
GLTEX_SOURCES = GlTextureStorage.hpp GlTextureStorage.cpp \
		GlTextures.hpp GlTextures.cpp 

libsharb_la_LDFLAGS = -L$(prefix)/lib -module
libsharb_la_LIBADD = $(top_srcdir)/src/libsh.la -lGL
libsharb_la_SOURCES = Arb.cpp ArbCode.cpp ArbInst.cpp ArbLimits.cpp \
		      ArbReg.cpp \
		      Arb.hpp ArbCode.hpp ArbInst.hpp ArbLimits.hpp \
		      ArbReg.hpp ArbBackend.cpp \
		      $(GLBACKEND_SOURCES) $(GLTEX_SOURCES)

libshati_la_LDFLAGS = -L$(prefix)/lib -module
libshati_la_LIBADD = $(top_srcdir)/src/libsh.la -lGL
nodist_libshati_la_SOURCES = UberTextures.hpp UberTextures.cpp \
			     UberStorage.hpp UberStorage.cpp \
		             AtiBackend.cpp uberbuffers.hpp

dist_libshati_la_SOURCES = Arb.cpp ArbCode.cpp ArbInst.cpp ArbLimits.cpp \
			   ArbReg.cpp \
			   Arb.hpp ArbCode.hpp ArbInst.hpp ArbLimits.hpp \
		      	   ArbReg.hpp \
			   $(GLBACKEND_SOURCES)

