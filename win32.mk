# install location
SH_INSTALLDIR = \dev\install

# OpenGL
OPENGL_DIR = \dev\opengl
INCLUDES += -I$(OPENGL_DIR)\include

# libpng
LIBPNG_DIR = \dev\libpng
INCLUDES += -I$(LIBPNG_DIR)
LIBPNG_RELEASE_LDADD = $(LIBPNG_DIR)\libpng.lib
LIBPNG_DEBUG_LDADD = $(LIBPNG_DIR)\libpngd.lib

# zlib
ZLIB_DIR = \dev\zlib
INCLUDES += -I$(ZLIB_DIR)
ZLIB_RELEASE_LDADD = $(ZLIB_DIR)\zlib.lib
ZLIB_DEBUG_LDADD = $(ZLIB_DIR)\zlibd.lib

CXX = cl /nologo
AR = link /lib /nologo
LD = link /nologo

CPPFLAGS = -DWIN32 -DNOMINMAX -D_USE_MATH_DEFINES $(INCLUDES)
CXXFLAGS = /GR /EHsc /wd4003

RELEASE_CPPFLAGS = $(CPPFLAGS)
RELEASE_CXXFLAGS = $(CXXFLAGS) $(RELEASE_CPPFLAGS) /MD /O2

DEBUG_CPPFLAGS = $(CPPFLAGS) -D_DEBUG -DSH_DEBUG
DEBUG_CXXFLAGS = $(CXXFLAGS) $(DEBUG_CPPFLAGS) /MDd /Zi /Od

LDFLAGS = 
RELEASE_LDFLAGS = $(LDFLAGS)
DEBUG_LDFLAGS = $(LDFLAGS) /DEBUG

clean:
	del *.lib *.obj *.d $(CLEANFILES)

%.r.obj: %.cpp
	$(CXX) /Fo$@ /c $< $(RELEASE_CXXFLAGS)

%.d.obj: %.cpp
	$(CXX) /Fo$@ /c $< $(DEBUG_CXXFLAGS)

%.r.d: %.cpp
	@makedepend -f- -o.r.obj $< -- $(RELEASE_CPPFLAGS) > $@ 2>devnull
	-@del devnull

%.d.d: %.cpp
	@makedepend -f- -o.d.obj $< -- $(RELEASE_CPPFLAGS) > $@ 2>devnull
	-@del devnull
