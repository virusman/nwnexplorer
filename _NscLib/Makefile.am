
AM_CXXFLAGS = -fno-default-inline

noinst_LIBRARIES = libnsc.a

libnsc_a_SOURCES = \
	NscCodeGenerator.cpp \
	NscCompiler.cpp \
	NscContext.cpp \
	NscDecompiler.cpp \
	NscParserRoutines.cpp \
	NscPStackEntry.cpp \
	Nsc.h \
	NscCodeGenerator.h \
	NscContext.h \
	NscPStackEntry.h \
	NscSymbolTable.h

nodist_libnsc_a_SOURCES = \
	NscParser.cpp \
	NscParser.h

BUILT_SOURCES = \
	NscParser.cpp \
	NscParser.h

libnsc_a_LIBADD = @LIBOBJS@

CLEANFILES = \
	NscParser.cpp \
	NscParser.h \
	NscParser.output

EXTRA_DIST = \
	_NscLib.vcproj \
	NscParser.y

MAINTAINERCLEANFILES = \
	Makefile.in

NscParser.cpp NscParser.h: NscParser.y
	$(MSTA) -c++ -v -l -d $<

