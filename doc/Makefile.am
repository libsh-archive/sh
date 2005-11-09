docs: html/index.html

html/index.html: Doxyfile $(wildcard ../src/*.[hc]pp) $(wildcard ../util/*.[hc]pp)
	doxygen Doxyfile

upload-docs: docs
	scp -P 10022 -r html www.libsh.org:/var/www/libsh.org/api/@VERSION@
	date "+%B %d, %Y" > /tmp/@VERSION@.release_date
	scp -P 10022 /tmp/@VERSION@.release_date www.libsh.org:/var/www/libsh.org/api

mostlyclean-local:
	rm -rf html

install-data-local: docs
	$(mkinstalldirs) $(DESTDIR)$(prefix)/share/doc/libsh/html
	$(INSTALL_DATA) html/*.html $(DESTDIR)$(prefix)/share/doc/libsh/html
	$(INSTALL_DATA) html/*.png $(DESTDIR)$(prefix)/share/doc/libsh/html
	$(INSTALL_DATA) html/*.css $(DESTDIR)$(prefix)/share/doc/libsh/html
	$(INSTALL_DATA) html/*.dot $(DESTDIR)$(prefix)/share/doc/libsh/html

EXTRA_DIST = Doxyfile.real
