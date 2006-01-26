AUTOMAKE_OPTIONS = 1.7 foreign

if ENABLE_DOXYGEN_DOCS
DOC_SUBDIR = doc
else
DOC_SUBDIR =
endif

## If you add a directory, don't forget to add it to DIST_SUBDIRS as well
SUBDIRS = libltdl src backends test $(DOC_SUBDIR)
DIST_SUBDIRS = libltdl src backends test doc examples

## Do not put a directory here since the distributed tarball will include its
## *entire* contents, including .svn, .libs, .o and binary files.
EXTRA_DIST = BUGS CODING_STANDARDS README.osx COPYING \
	     win32/vc8/postbuild_arb.bat \
	     win32/vc8/postbuild_cc.bat win32/vc8/postbuild_glsl.bat \
	     win32/vc8/postbuild_sh.bat win32/vc8/postbuild_util.bat \
	     win32/vc8/prebuild.bat win32/vc8/ShArbBackend.vcproj \
	     win32/vc8/ShCcBackend.vcproj win32/vc8/ShGlslBackend.vcproj \
	     win32/vc8/Sh.sln win32/vc8/ShUtil.vcproj win32/vc8/Sh.vcproj \
	     win32/vc8/ShGlutExample.vcproj \
	     win32/vc7/postbuild_arb.bat \
	     win32/vc7/postbuild_cc.bat win32/vc7/postbuild_glsl.bat \
	     win32/vc7/postbuild_sh.bat win32/vc7/postbuild_util.bat \
	     win32/vc7/prebuild.bat win32/vc7/ShArbBackend.vcproj \
	     win32/vc7/ShCcBackend.vcproj win32/vc7/ShGlslBackend.vcproj \
	     win32/vc7/Sh.sln win32/vc7/ShUtil.vcproj win32/vc7/Sh.vcproj \
	     win32/vc7/ShGlutExample.vcproj \
	     xcode/ArbBackend-Info.plist xcode/CcBackend-Info.plist \
	     xcode/Particles-Info.plist xcode/GlutExample-Info.plist \
	     xcode/Info.plist xcode/Particles-Info.plist xcode/version.plist \
	     xcode/TexturesExample-Info.plist xcode/Sh.xcode/project.pbxproj \
	     xcode/English.lproj/InfoPlist.strings xcode/Sh.icns \
	     xcode/Sh.pmsp xcode/ShUtil-Info.plist \
	     xcode/pkg_resources/License.txt \
	     xcode/pkg_resources/Welcome.txt \
	     xcode/pkg_resources/ReadMe.txt

docs:
	cd doc ; $(MAKE) docs ; cd ..

upload-docs:
	cd doc ; $(MAKE) upload-docs ; cd ..

dist dist-all: distdir
	$(AMTAR) chof - $(distdir) | GZIP=$(GZIP_ENV) gzip -c >$(distdir).tar.gz
	zip -rq $(distdir).zip $(distdir)
	$(am__remove_distdir)

regress:
	@if [ -z "$$SH_REGRESS_BACKEND" ] ; then echo "You must specify the backend to test through the SH_REGRESS_BACKEND environment variable. For example:" ;  echo "" ; echo "  SH_REGRESS_BACKEND=arb make regress"; else $(MAKE) check-recursive; fi

BACKENDS=arb cc glsl host
regressall:
	for i in $(BACKENDS) ; do SH_REGRESS_BACKEND=$$i $(MAKE) check-recursive ; done
