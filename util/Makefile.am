INCLUDES = -I$(top_srcdir)/src $(LTDLINCL) 

EXTRA_DIST = Makefile.win32

lib_LTLIBRARIES = libshutil.la

libshutil_la_LDFLAGS = -export-dynamic # -no-undefined

libshutil_la_SOURCES = ShKernelLib.cpp ShKernelSurfMap.cpp ShObjMesh.cpp shutil.cpp

incincdir = $(includedir)/sh

incinc_HEADERS = ShNoise.hpp \
                 ShNoiseImpl.hpp \
                 ShWorley.hpp \
                 ShWorleyImpl.hpp \
                 ShFunc.hpp \
                 ShFuncImpl.hpp \
                 ShKernelLib.hpp \
                 ShKernelLibImpl.hpp \
                 ShKernelLight.hpp \
                 ShKernelLightImpl.hpp \
                 ShKernelSurface.hpp \
                 ShKernelSurfaceImpl.hpp \
                 ShKernelSurfMap.hpp \
                 ShKernelPost.hpp \
                 ShKernelPostImpl.hpp \
                 ShMesh.hpp \
                 ShMeshImpl.hpp \
                 ShObjMesh.hpp \
                 shutil.hpp
