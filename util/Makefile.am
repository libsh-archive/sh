INCLUDES = -I../src $(LTDLINCL) 

lib_LTLIBRARIES = libshutil.la

libshutil_la_LIBADD = $(LIBLTDL)
libshutil_la_LDFLAGS = -export-dynamic -no-undefined

libshutil_la_SOURCES = ShKernelLib.cpp ShObjMesh.cpp

incincdir = $(includedir)/sh

incinc_HEADERS = ShNoise.hpp \
								 ShNoiseImpl.hpp \
                 ShWorley.hpp \
                 ShWorleyImpl.hpp \
                 ShUtil.hpp \
                 ShUtilImpl.hpp \
								 ShKernelLib.hpp \
								 ShKernelLibImpl.hpp \
								 ShMesh.hpp \
								 ShMeshImpl.hpp \
								 ShObjMesh.hpp \
                 shutil.hpp
