INCLUDES = -I$(top_srcdir)/src

EXTRA_DIST = Makefile.win32

shbackenddir = $(prefix)/lib/sh

if ENABLE_ARB_BACKEND
ARB_LIBS = libsharb.la
else
ARB_LIBS =
endif

shbackend_LTLIBRARIES = $(ARB_LIBS)

GLBACKEND_SOURCES = GlBackend.cpp GlBackend.hpp \
		  GlTextureName.hpp GlTextureName.cpp \
		  PCScheduler.hpp PCScheduler.cpp \
		  Utils.hpp Utils.cpp

GLTEXTURE_SOURCES = GlTextureStorage.hpp GlTextureStorage.cpp \
		     GlTextures.hpp GlTextures.cpp 
PBUFFERSTREAM_SOURCES = PBufferStreams.hpp PBufferStreams.cpp \
			GLXPBufferStreams.hpp GLXPBufferStreams.cpp \
			PBufferContext.hpp PBufferContext.cpp \
			GLXPBufferContext.hpp GLXPBufferContext.cpp

if ENABLE_ARB_BACKEND
libsharb_la_LDFLAGS = -L$(prefix)/lib -module
libsharb_la_LIBADD = $(top_srcdir)/src/libsh.la 
libsharb_la_SOURCES = Arb.cpp ArbCode.cpp ArbEmit.cpp ArbInst.cpp ArbLimits.cpp \
		      ArbReg.cpp \
		      Arb.hpp ArbCode.hpp ArbInst.hpp ArbLimits.hpp \
		      ArbReg.hpp ArbBackend.cpp \
                       $(GLBACKEND_SOURCES) $(GLTEXTURE_SOURCES) $(PBUFFERSTREAM_SOURCES)
endif
