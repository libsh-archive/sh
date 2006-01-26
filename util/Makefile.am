INCLUDES = -I$(includedir)/src $(LTDLINCL) 

lib_LTLIBRARIES = libshutil.la

libshutil_la_LDFLAGS = -export-dynamic # -no-undefined

libshutil_la_SOURCES = ShKernelLib.cpp ShKernelSurfMap.cpp ShObjMesh.cpp ShPngImage.cpp shutil.cpp

incincdir = $(includedir)/shutil

incinc_HEADERS = ShFunc.hpp \
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
                 ShPngImage.hpp \
                 ShPngImageImpl.hpp \
                 shutil.hpp
