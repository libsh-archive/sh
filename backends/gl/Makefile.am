INCLUDES = -I$(top_srcdir)/src

shbackenddir = $(prefix)/lib/sh

if ENABLE_ARB_BACKEND
ARB_LIBS = libsharb.la
else
ARB_LIBS =
endif

if ENABLE_ATI_BACKEND
ATI_LIBS = libshati.la
else
ATI_LIBS =
endif

shbackend_LTLIBRARIES = $(ARB_LIBS) $(ATI_LIBS)

GLBACKEND_SOURCES = GlBackend.cpp GlBackend.hpp \
		  GlTextureName.hpp GlTextureName.cpp
GLTEXTURE_SOURCES = GlTextureStorage.hpp GlTextureStorage.cpp \
		     GlTextures.hpp GlTextures.cpp 
PBUFFERSTREAM_SOURCES = PBufferStreams.hpp PBufferStreams.cpp

if ENABLE_ARB_BACKEND
libsharb_la_LDFLAGS = -L$(prefix)/lib -module
libsharb_la_LIBADD = $(top_srcdir)/src/libsh.la 
libsharb_la_SOURCES = Arb.cpp ArbCode.cpp ArbInst.cpp ArbLimits.cpp \
		      ArbReg.cpp \
		      Arb.hpp ArbCode.hpp ArbInst.hpp ArbLimits.hpp \
		      ArbReg.hpp ArbBackend.cpp \
                       $(GLBACKEND_SOURCES) $(GLTEXTURE_SOURCES) $(PBUFFERSTREAM_SOURCES)
endif

if ENABLE_ATI_BACKEND
libshati_la_LDFLAGS = -L$(prefix)/lib -module
libshati_la_LIBADD = $(top_srcdir)/src/libsh.la
nodist_libshati_la_SOURCES = UberTextures.hpp UberTextures.cpp \
			     UberStorage.hpp UberStorage.cpp \
		             AtiBackend.cpp uberbuffers.hpp

dist_libshati_la_SOURCES = Arb.cpp ArbCode.cpp ArbInst.cpp ArbLimits.cpp \
			   ArbReg.cpp \
			   Arb.hpp ArbCode.hpp ArbInst.hpp ArbLimits.hpp \
		      	   ArbReg.hpp \
			   $(GLBACKEND_SOURCES)
endif
