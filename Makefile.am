AUTOMAKE_OPTIONS = 1.7 foreign

if ENABLE_DOXYGEN_DOCS
DOC_SUBDIR = doc
else
DOC_SUBDIR =
endif

## If you add a directory, don't forget to add it to DIST_SUBDIRS as well
SUBDIRS = libltdl src backends test util $(DOC_SUBDIR)
DIST_SUBDIRS = libltdl src backends test util doc examples

## Do not put a directory here since the distributed tarball will include its
## *entire* contents, including .svn, .libs, .o and binary files.
EXTRA_DIST = BUGS CODING_STANDARDS README.osx README.w32.txt \
             Makefile.win32 win32.mk win32/postbuild_arb.bat \
	     win32/postbuild_cc.bat win32/postbuild_glsl.bat \
	     win32/postbuild_sh.bat win32/postbuild_util.bat \
	     win32/prebuild.bat win32/ShArbBackend.vcproj \
	     win32/ShCcBackend.vcproj win32/ShGlslBackend.vcproj \
	     win32/Sh.sln win32/ShUtil.vcproj win32/Sh.vcproj \
	     win32/ShGlutExample.vcproj

docs:
	cd doc ; $(MAKE) docs ; cd ..

upload-docs:
	cd doc ; $(MAKE) upload-docs ; cd ..

dist-zip: distdir
	zip -rq $(distdir).zip $(distdir)
	$(am__remove_distdir)

dist dist-all: distdir
	$(AMTAR) chof - $(distdir) | GZIP=$(GZIP_ENV) gzip -c >$(distdir).tar.gz
	zip -rq $(distdir).zip $(distdir)
	$(am__remove_distdir)

test: check
