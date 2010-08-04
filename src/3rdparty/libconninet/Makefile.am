SUBDIRS = src tests

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = conninet.pc

MAINTAINERCLEANFILES = \
        Makefile.in config.h.in configure \
        install-sh ltmain.sh missing mkinstalldirs \
        config.log config.status config.guess config.sub \
        build-stamp compile depcomp acinclude.m4 aclocal.m4 \
	debian/files \
	debian/libconninet0.postinst.debhelper \
	debian/libconninet0.postrm.debhelper \
	debian/libconninet0.substvars

EXTRA_DIST = \
	debian/changelog \
	debian/compat \
	debian/control \
	debian/copyright \
	debian/libconninet0-dev.dirs \
	debian/libconninet0-dev.files \
	debian/libconninet0.dirs \
	debian/libconninet0.files \
	debian/rules

# get rid of debian build dirs
maintainer-clean-local:
	-rm -rf debian/build debian/tmp \
		debian/libconninet0 \
		debian/libconninet0-dev \
		debian/libconninet0-dbg

if DOXYGEN_DOCS_ENABLED
  DOXYGEN_LINE = $(DOXYGEN) doxygen.cfg
else 
  DOXYGEN_LINE = 
endif

.PHONY : doc

doc: 
	$(DOXYGEN_LINE)

deb: dist
	-mkdir $(top_builddir)/debian-build
	cd $(top_builddir)/debian-build && tar zxf ../$(top_builddir)/$(PACKAGE)-$(VERSION).tar.gz
	cd $(top_builddir)/debian-build/$(PACKAGE)-$(VERSION) && dpkg-buildpackage -rfakeroot -sa -D
	-rm -rf $(top_builddir)/debian-build/$(PACKAGE)-$(VERSION)
