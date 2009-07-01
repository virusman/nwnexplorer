
bin_PROGRAMS = wxnwntreasure

wxnwntreasure_SOURCES = \
	EncItemPropDlg.cpp \
	EncItemPropDlg.h \
	PlcItemPropDlg.cpp \
	PlcItemPropDlg.h \
	ProItemPropDlg.cpp \
	ProItemPropDlg.h \
	ItemPropDlg.cpp \
	ItemPropDlg.h \
	TablePropDlg.cpp \
	TablePropDlg.h \
	wxNwnTreasure.cpp \
	wxNwnTreasure.h

AM_CPPFLAGS = $(WX_CPPFLAGS)
AM_CXXFLAGS = $(WX_CXXFLAGS_ONLY)
AM_LDFLAGS = $(WX_LIBS)

EXTRA_DIST = \
	after.xpm \
	down.xpm \
	encounter.xpm \
	exit.xpm \
	fileopen.xpm \
	gold.xpm \
	helpicon.xpm \
	new.xpm \
	none.xpm \
	prefs.xpm \
	prop.xpm \
	remove.xpm \
	save.xpm \
	save_as.xpm \
	saveas.xpm \
	table.xpm \
	tableprop.xpm \
	up.xpm \
	encounter.xpm \
	profile.xpm \
	placeable.xpm \
	nwntreasure.xpm \
	noref.xpm \
	global.xpm \
	item.xpm

MAINTAINERCLEANFILES = \
	Makefile.in

#rules added by sumpfork (pgorniak@mac.com) to make mac os app and dmg
#somewhat messy - sorry
PROGRAM = $(bin_PROGRAMS)
BUNDLE = $(PROGRAM).app/Contents
RESCOMP = /Developer/Tools/Rez
IMAGE = $(PWD)/$(PROGRAM).image
AWK = awk
DU = du
HDIUTIL = hdiutil

versioned-dmg: $(PROGRAM).dmg
	mv -f $(PROGRAM).dmg $(PROGRAM)-$(VERSION).dmg

$(PROGRAM).dmg: bundle
	@$(INSTALL) -d $(IMAGE)
	@$(LIBTOOL) --mode=install cp $(PWD)/README $(IMAGE)/README
	@$(LIBTOOL) --mode=install cp $(PWD)/COPYING $(IMAGE)/COPYING
	@$(LIBTOOL) --mode=install cp -r $(PWD)/$(PROGRAM).app $(IMAGE)/
	$(HDIUTIL) create -sectors `$(DU) -s $(IMAGE) | $(AWK) '{print 2.5*$$1}'` -volname $(PROGRAM) -fs HFS -ov $(PROGRAM)
	$(HDIUTIL) attach $@ 2>&1 | grep $(PROGRAM) | cut -d ' ' -f1 > mountloc.tmp
	@$(LIBTOOL) --mode=install cp -r $(IMAGE)/* /Volumes/$(PROGRAM)
	$(HDIUTIL) detach `cat mountloc.tmp`
	$(RM) mountloc.tmp
	$(HDIUTIL) resize $@ -size min
	mv -f $@ $(PROGRAM).tmp.dmg
	$(HDIUTIL) convert $(PROGRAM).tmp.dmg -format UDZO -imagekey zlib-level=2 -o $@
	$(RM) $(PROGRAM).tmp.dmg

#
# Targets to build a Mac OS X application bundle
#
bundle: res $(BUNDLE)/Info.plist $(BUNDLE)/PkgInfo $(BUNDLE)/Resources/$(PROGRAM).rsrc $(BUNDLE)/Resources/$(PROGRAM).icns

res: $(BUNDLE)/MacOS/$(PROGRAM)
	$(RESCOMP) -t APPL Carbon.r $(PROGRAM).r -o $(BUNDLE)/MacOS/$(PROGRAM)

$(BUNDLE)/Info.plist: Info.plist.in
	@$(INSTALL) -d `dirname $@`
	sed -e "s/IDENTIFIER/`echo $(program_dir) | sed 's,/,.,g'`/" \
	    -e "s/EXECUTABLE/$(PROGRAM)/" \
	    -e "s/VERSION/$(PACKAGE_VERSION)/" $< > $@

$(BUNDLE)/PkgInfo:
	@$(INSTALL) -d `dirname $@`
	echo -n "APPL????" > $@

$(BUNDLE)/MacOS/$(PROGRAM): $(PROGRAM)
	@$(INSTALL) -d `dirname $@`
	@$(LIBTOOL) --mode=install cp $< $(PWD)/$@

$(BUNDLE)/Resources/$(PROGRAM).rsrc: $(PROGRAM).rsrc
	@$(INSTALL) -d `dirname $@`
	cp $< $@

$(BUNDLE)/Resources/$(PROGRAM).icns: $(PROGRAM).icns
	@$(INSTALL) -d `dirname $@`
	cp $< $@



