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
EXTRA_DIST = BUGS Makefile.win32 win32.mk README.w32.txt

docs:
	cd doc ; $(MAKE) docs ; cd ..

upload-docs:
	cd doc ; $(MAKE) upload-docs ; cd ..
