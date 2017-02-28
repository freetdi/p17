include Makefile.settings

package = gala
includedir = ${prefix}/include
pkgincludedir = ${includedir}/${package}

HEADERS = \
	boost_assign.h \
	boost_detail.h \
	boost.h \
	cbset.h \
	degs.h \
	digraph.h \
	graph.h \
	immutable.h \
	sethack.h \
	sfinae.h \
	td.h \
	trace.h

INSTALL = install

all:

check:
	$(MAKE) -C tests

install:
	-$(INSTALL) -d ${includedir}
	-$(INSTALL) -d ${pkgincludedir}
	$(INSTALL) $(HEADERS) ${pkgincludedir}

Makefile.settings:
	@echo run configure first && false
