## Process this file with automake to produce Makefile.in

SUBDIRS = _MathLib _NwnLib _NscLib _NmcLib nwnnsscomp nwnmdlcomp

AM_CXXFLAGS = -fno-default-inline

CLEANFILES = \
	config.h.in~

EXTRA_DIST = \
	README-1.2.html \
	win32_config.h \
	NWNTools.sln \
	NWNTools.suo \
	nwntools.ebuild \
	Save \
	_ExpatLib \
	nwntreasure \
	nwnexplorer

dist-hook:
	rm -rf `find $(distdir) -type d -name CVS`

MAINTAINERCLEANFILES = \
	INSTALL \
	Makefile.in \
	aclocal.m4 \
	config.guess \
	config.h.in \
	config.sub \
	configure \
	depcomp \
	install-sh \
	missing \
	mkinstalldirs

RELEASE = @RELEASE@

$(distdir).tar.bz2:
	$(MAKE) dist-bzip2

if EBUILD
$(distdir)-r$(RELEASE).ebuild.tar.bz2: nwntools.ebuild $(distdir).tar.bz2
	-rm -rf portage
	mkdir -p portage/app-games/nwntools
	cp nwntools.ebuild portage/app-games/nwntools/$(distdir)-r$(RELEASE).ebuild
	mkdir -p portage/distfiles
	cp $(distdir).tar.bz2 portage/distfiles
	PORTDIR=portage DISTDIR=portage/distfiles $(EBUILD) portage/app-games/nwntools/$(distdir)-r$(RELEASE).ebuild digest
	cd portage && $(AMTAR) chof - app-games | bzip2 -9 -c > ../$(distdir)-r$(RELEASE).ebuild.tar.bz2
	-rm -rf portage

dist-ebuild: $(distdir)-r$(RELEASE).ebuild.tar.bz2
endif

if RPMBUILD
$(distdir)-$(RELEASE).src.rpm: nwntools.spec $(distdir).tar.bz2
	-rm -f $@
	$(RPMBUILD) \
		--define="_sourcedir ." \
		--define="_srcrpmdir ." \
		-bs nwntools.spec

$(distdir)-$(RELEASE).%.rpm: nwntools.spec $(distdir).tar.bz2
	-rm -f $@
	$(RPMBUILD) \
		--define="_sourcedir ." \
		--define="_rpmdir ." \
		--define="_builddir `pwd`" \
		--target=$* \
		--clean \
		-bb nwntools.spec
	mv $*/$@ .
	-rmdir $*

dist-srpm: $(distdir)-$(RELEASE).src.rpm

dist-rpm: $(distdir)-$(RELEASE).$(host_cpu).rpm
endif

