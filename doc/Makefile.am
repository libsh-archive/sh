docs: html/index.html

html/index.html: Doxyfile $(wildcard ../src/*.[hc]pp) $(wildcard ../util/*.[hc]pp)
	doxygen Doxyfile

upload-docs: docs
	scp -r html shell.sourceforge.net:/home/groups/l/li/libsh/htdocs/api

mostlyclean-local:
	rm -rf html

install-data-local: docs
	$(mkinstalldirs) $(DESTDIR)$(prefix)/share/doc/libsh/html
	$(INSTALL_DATA) html/*.html $(DESTDIR)$(prefix)/share/doc/libsh/html
	$(INSTALL_DATA) html/*.png $(DESTDIR)$(prefix)/share/doc/libsh/html
	$(INSTALL_DATA) html/*.css $(DESTDIR)$(prefix)/share/doc/libsh/html
	$(INSTALL_DATA) html/*.dot $(DESTDIR)$(prefix)/share/doc/libsh/html
