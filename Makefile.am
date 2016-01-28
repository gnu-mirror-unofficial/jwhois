## Process this file with automake to produce Makefile.in
##
## This file is part of jwhois
## Copyright (C) 1999-2007, 2015, 2016  Free Software Foundation, Inc.
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 3 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
##

ACLOCAL_AMFLAGS = -I m4

SUBDIRS = lib po

bin_PROGRAMS = jwhois

jwhois_SOURCES = \
  src/cache.c \
  src/dns.c \
  src/gai_strerror.c \
  src/http.c \
  src/init.c \
  src/jconfig.c \
  src/jwhois.c \
  src/lookup.c \
  src/rwhois.c \
  src/utils.c \
  src/whois.c

jwhois_CFLAGS = $(OLDCFLAGS) -DSYSCONFDIR="\"$(sysconfdir)\"" \
         -DLOCALSTATEDIR="\"$(localstatedir)\"" \
         -DLOCALEDIR="\"$(localedir)\""

jwhois_LDADD = @LIBINTL@ @LIBICONV@ @LIBS@ lib/libgnu.a

AM_CPPFLAGS = \
  -I$(top_srcdir)/include \
  -I$(top_srcdir)/lib \
  -I$(top_builddir)/lib

AM_CFLAGS = $(jwhois_CFLAGS)

info_TEXINFOS = doc/jwhois.texi
dist_man_MANS = doc/jwhois.1

# Avoid re-generating the man pages when building from tarballs.
if BUILD_FROM_GIT
doc/jwhois.1: jwhois
	$(AM_V_GEN)$(MKDIR_P) `dirname "$@"`; \
	LANGUAGE= $(HELP2MAN) --output="$@" "$(top_builddir)/$<"
endif

EXTRA_DIST = \
  example/jwhois.conf \
  include/*.h \
  jwhois.spec.in \
  jwhois.spec \
  m4/gnulib-cache.m4

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(sysconfdir)
	@if test ! -f $(DESTDIR)$(sysconfdir)/jwhois.conf; then \
		echo "$(INSTALL_DATA) $(top_srcdir)/example/jwhois.conf $(DESTDIR)$(sysconfdir)/jwhois.conf"; \
		$(INSTALL_DATA) $(top_srcdir)/example/jwhois.conf $(DESTDIR)$(sysconfdir)/jwhois.conf; \
	else \
		echo "## ";\
		echo "## Not installing jwhois.conf because you already have a previous version";\
		echo "## installed. You should manually take care of updating your jwhois.conf.";\
		echo "##";\
	fi
