AUTOMAKE_OPTIONS = 1.7 foreign

if ENABLE_DOXYGEN_DOCS
DOC_SUBDIR = doc
else
DOC_SUBDIR =
endif

SUBDIRS = libltdl src backends test util $(DOC_SUBDIR)

EXTRA_DIST = BUGS Makefile.win32 win32.mk README.w32.txt

docs:
	cd doc ; $(MAKE) docs ; cd ..

upload-docs:
	cd doc ; $(MAKE) upload-docs ; cd ..
