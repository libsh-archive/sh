OPENGL_DIR = \dev\opengl
MSSDK_DIR = \dev\mssdk
LIBPNG_DIR = \dev\libpng
ZLIB_DIR = \dev\zlib

# libpng/zlib
INCLUDES = -I$(LIBPNG_DIR)\include -I$(ZLIB_DIR)\include
# MS platform SDK
INCLUDES += -I$(MSSDK_DIR)\include
# OpenGL
INCLUDES += -I$(OPENGL_DIR)\include

CPPFLAGS = -DWIN32 -DSH_DEBUG $(INCLUDES)
CXXFLAGS = $(CPPFLAGS) /GR /GX /wd4003
CXX = cl
LIBRARIAN = lib
LDFLAGS = /link /LIBPATH:\DEV\MSSDK\LIB

clean:
	del *.lib *.obj *.d

%.obj: %.cpp
	$(CXX) /Fo$@ /c $< $(CXXFLAGS)

%.d: %.cpp
	makedepend -f- -o.obj $< -- $(CPPFLAGS) > $@ 2>devnull
	-@del devnull
