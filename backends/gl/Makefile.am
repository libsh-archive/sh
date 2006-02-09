INCLUDES = -I$(top_srcdir)/src/sh

EXTRA_DIST = WGLPBufferContext.hpp WGLPBufferContext.cpp

shbackenddir = $(prefix)/lib/sh

if ENABLE_GL_BACKENDS
GL_COMMON_LIBS = libshgl.la
else
GL_COMMON_LIBS = 
endif

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

shbackend_LTLIBRARIES = $(GL_COMMON_LIBS) $(ARB_LIBS) $(GLSL_LIBS)

GLBACKEND_SOURCES = GlBackend.cpp GlBackend.hpp \
		  GlTextureName.hpp GlTextureName.cpp
GLTEXTURE_SOURCES = GlTextureStorage.hpp GlTextureStorage.cpp \
		     GlTextures.hpp GlTextures.cpp 
if INCLUDE_PBUFFERSTREAMS
PBUFFERSTREAM_SOURCES = PBufferStreams.hpp PBufferStreams.cpp \
			PBufferContext.hpp PBufferContext.cpp \
			GLXPBufferContext.hpp GLXPBufferContext.cpp \
			Utils.hpp Utils.cpp \
			FBOStreams.hpp FBOStreams.cpp
else
PBUFFERSTREAM_SOURCES = 
endif

if ENABLE_GL_BACKENDS
libshgl_la_LDFLAGS = -L$(prefix)/lib
libshgl_la_LIBADD = $(top_builddir)/src/sh/libsh.la
libshgl_la_SOURCES = $(GLBACKEND_SOURCES) $(GLTEXTURE_SOURCES) $(PBUFFERSTREAM_SOURCES)
endif

if ENABLE_ARB_BACKEND
libsharb_la_LDFLAGS = -L$(prefix)/lib -module
libsharb_la_LIBADD = $(top_builddir)/src/sh/libsh.la libshgl.la
libsharb_la_SOURCES = Arb.cpp ArbCode.cpp ArbEmit.cpp ArbInst.cpp ArbLimits.cpp \
		      ArbReg.cpp \
		      Arb.hpp ArbCode.hpp ArbInst.hpp ArbLimits.hpp \
		      ArbReg.hpp ArbBackend.cpp
endif

if ENABLE_GLSL_BACKEND
libshglsl_la_LDFLAGS = -L$(prefix)/lib -module
libshglsl_la_LIBADD = $(top_builddir)/src/sh/libsh.la libshgl.la
libshglsl_la_SOURCES = Glsl.cpp Glsl.hpp GlslCode.cpp GlslCode.hpp \
		       GlslBackend.cpp GlslVariable.cpp GlslVariable.hpp \
		       GlslVariableMap.cpp GlslVariableMap.hpp GlslEmit.cpp \
		       GlslSet.cpp GlslSet.hpp
endif
