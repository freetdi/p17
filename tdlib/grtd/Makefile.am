## Process this file with automake to produce Makefile.in
#
# Makefile.am
#
# Automake input file.
#
# Copyright (C) 2016 Felix Salfelder
#
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this package; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
# Boston, MA 02110-1301, USA.
#

if USE_GALA
GALA_NOINST_APPS = \
	grtd-profile \
	tw-exact-profile
GALA_APPS = \
	tw-exact \
	grtd
endif

BOOST_LIBS = -lboost_thread -lboost_system -lpthread

PROFILE_GEN = -fprofile-generate
PROFILE_FLAGS = -fprofile-use=.
PROFILE_GRAPH = HoffmanGraph.gr

COMMON_CXXFLAGS = -Os -march=native

grtddir=$(pkglibexecdir)
grtd_PROGRAMS = ${GALA_APPS}
check_PROGRAMS = ${GALA_NOINST_APPS}

grtd_profile_SOURCES = grtd.cc
grtd_profile_CXXFLAGS = $(AM_CXXFLAGS) $(PROFILE_GEN) $(COMMON_CXXFLAGS)
grtd_profile_CPPFLAGS = $(AM_CPPFLAGS) -I. -DUSE_GALA
grtd_profile_LDADD = $(BOOST_LIBS)

grtd_SOURCES = grtd.cc
grtd_CXXFLAGS = $(AM_CXXFLAGS) $(PROFILE_FLAGS) $(COMMON_CXXFLAGS)
grtd_CPPFLAGS = $(AM_CPPFLAGS) -I. -DUSE_GALA
grtd_LDADD = $(BOOST_LIBS)

tw_exact_profile_SOURCES = tw-exact.cc
tw_exact_profile_CXXFLAGS = $(AM_CXXFLAGS) $(PROFILE_GEN) $(COMMON_CXXFLAGS)
tw_exact_profile_CPPFLAGS = $(AM_CPPFLAGS) -I.

tw_exact_SOURCES = tw-exact.cc
tw_exact_CXXFLAGS = $(AM_CXXFLAGS) $(PROFILE_FLAGS) $(COMMON_CXXFLAGS)
tw_exact_CPPFLAGS = $(AM_CPPFLAGS) -I.


# there's some way to clean this up. i forgot.
TWEX_AMNAME = tw_exact-tw-exact
TWEXP_AMNAME = tw_exact_profile-tw-exact
GRTD_AMNAME = grtd-grtd
GRTDP_AMNAME = grtd_profile-grtd

# trial and error?!
$(TWEX_AMNAME).o: $(TWEXP_AMNAME).gcda
$(TWEX_AMNAME).Tpo: $(TWEXP_AMNAME).gcda
$(GRTD_AMNAME).o: $(GRTDP_AMNAME).gcda
$(GRTD_AMNAME).Tpo: $(GRTDP_AMNAME).gcda

CLEANFILES = \
   $(TWEXP_AMNAME).gcda \
	$(GRTDP_AMNAME).gcda \
	HoffmanGraph.gr.td*

$(TWEXP_AMNAME).gcda: $(PROFILE_GRAPH) tw-exact-profile
	./tw-exact-profile < $< > $(PROFILE_GRAPH).td
	-td-validate $< $(PROFILE_GRAPH).td

TIME=1

$(GRTDP_AMNAME).gcda: $(PROFILE_GRAPH) grtd-profile
	rm -f $@
	@for i in 1 2 8 32 64; do \
		echo "./grtd-profile -m $$i < $< > $(PROFILE_GRAPH).td$$i & p=\$$!; sleep $(TIME); kill -TERM \$$p;"; \
		./grtd-profile -m $$i < $< > $(PROFILE_GRAPH).td$$i & p=$$!; sleep $(TIME); kill -TERM $$p; \
      wait $$p; \
		td-validate $< $(PROFILE_GRAPH).td$$i || : ; \
	done

BUILT_SOURCES = tdlib

tdlib:
	-$(LN_S) $(top_srcdir)/src $@

#here?
EXTRA_DIST = grparse.h dumptd.h

# TESTS = $(check_PROGRAMS)

# AM_DEFAULT_SOURCE_EXT=.cc

# AM_CPPFLAGS = -I. -DTRACE_UNTESTED
AM_CPPFLAGS = $(LOCAL_CPPFLAGS)
# AM_CXXFLAGS = -O0 -g
