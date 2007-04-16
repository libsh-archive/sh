INCLUDES = -I$(top_srcdir)/src $(LTDLINCL) 

lib_LTLIBRARIES = libshutil.la

libshutil_la_LDFLAGS = -export-dynamic # -no-undefined

libshutil_la_SOURCES = KernelLib.cpp KernelSurfMap.cpp ObjMesh.cpp PngImage.cpp shutil.cpp

incincdir = $(includedir)/shutil

incinc_HEADERS = Func.hpp \
                 FuncImpl.hpp \
                 KernelLib.hpp \
                 KernelLibImpl.hpp \
                 KernelLight.hpp \
                 KernelLightImpl.hpp \
                 KernelSurface.hpp \
                 KernelSurfaceImpl.hpp \
                 KernelSurfMap.hpp \
                 KernelPost.hpp \
                 KernelPostImpl.hpp \
                 Mesh.hpp \
                 MeshImpl.hpp \
                 ObjMesh.hpp \
                 PngImage.hpp \
                 PngImageImpl.hpp \
                 shutil.hpp
