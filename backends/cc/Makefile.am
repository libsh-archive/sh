INCLUDES = -I$(top_srcdir)/src/sh

shbackenddir = $(prefix)/lib/sh

shbackend_LTLIBRARIES = libshcc.la

libshcc_la_LDFLAGS = -L$(prefix)/lib -module
libshcc_la_LIBADD = $(top_builddir)/src/sh/libsh.la 
BUILT_SOURCES=CcTexturesString.hpp
libshcc_la_SOURCES = Cc.cpp Cc.hpp CcEmit.cpp CcTexturesString.hpp CcTextures.hpp

GENERATED=CcTexturesString.hpp

CcTexturesString.hpp: CcTextures.hpp
	sed 's/"/\\"/g' CcTextures.hpp | awk 'BEGIN { printf "const char* cc_texture_string[] = {\n" } {printf "\"%s\\n\",\n", $$0} END { printf "\"\"};\n"}' > CcTexturesString.hpp
