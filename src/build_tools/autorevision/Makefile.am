noinst_PROGRAMS = auto_revision
auto_revision_SOURCES = autorevision.cpp

AM_CPPFLAGS = -I$(top_srcdir)/src/include

auto_revision_LDADD = ../../base/tinyxml/libtinyxml.la

auto_revision_CXXFLAGS=-DTIXML_USE_STL

all-local: auto_revision
	./auto_revision +int +wx +t $(top_srcdir) $(top_builddir)/src/include/autorevision.h
