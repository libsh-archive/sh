INCLUDES = -I$(top_srcdir)/src

EXTRA_DIST = Makefile.win32

shbackenddir = $(prefix)/lib/sh

if ENABLE_ARB_BACKEND
ARB_LIBS = libsharb.la
else
ARB_LIBS =
endif

if ENABLE_GLSL_BACKEND
GLSL_LIBS = libshglsl.la
else
GLSL_LIBS =
endif

shbackend_LTLIBRARIES = $(ARB_LIBS) $(GLSL_LIBS)

GLBACKEND_SOURCES = GlBackend.cpp GlBackend.hpp \
		  GlTextureName.hpp GlTextureName.cpp
GLTEXTURE_SOURCES = GlTextureStorage.hpp GlTextureStorage.cpp \
		     GlTextures.hpp GlTextures.cpp 
PBUFFERSTREAM_SOURCES = PBufferStreams.hpp PBufferStreams.cpp \
			GLXPBufferStreams.hpp GLXPBufferStreams.cpp

if ENABLE_ARB_BACKEND
libsharb_la_LDFLAGS = -L$(prefix)/lib -module
libsharb_la_LIBADD = $(top_srcdir)/src/libsh.la 
libsharb_la_SOURCES = Arb.cpp ArbCode.cpp ArbEmit.cpp ArbInst.cpp ArbLimits.cpp \
		      ArbReg.cpp \
		      Arb.hpp ArbCode.hpp ArbInst.hpp ArbLimits.hpp \
		      ArbReg.hpp ArbBackend.cpp \
                       $(GLBACKEND_SOURCES) $(GLTEXTURE_SOURCES) $(PBUFFERSTREAM_SOURCES)
endif

if ENABLE_GLSL_BACKEND
libshglsl_la_LDFLAGS = -L$(prefix)/lib -module
libshglsl_la_LIBADD = $(top_srcdir)/src/libsh.la 
libshglsl_la_SOURCES = Glsl.cpp Glsl.hpp GlslCode.cpp GlslCode.hpp \
		       GlslBackend.cpp \
		       $(GLBACKEND_SOURCES) $(GLTEXTURE_SOURCES) \
		       $(PBUFFERSTREAM_SOURCES)
endif
