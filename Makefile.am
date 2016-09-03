## Process this file with automake to produce Makefile.in
# Copyright (C) 1999-2007, 2015, 2016  Free Software Foundation, Inc.
#
# This file is part of GNU JWhois.
#
# GNU JWhois is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# GNU JWhois is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU JWhois.  If not, see <http://www.gnu.org/licenses/>.

ACLOCAL_AMFLAGS = -I m4

SUBDIRS = lib po

bin_PROGRAMS = jwhois
noinst_LIBRARIES = libjwhois.a

AM_CPPFLAGS = \
  -I$(top_srcdir)/src \
  -I$(top_srcdir)/lib \
  -I$(top_builddir)/lib

AM_CFLAGS = $(WARN_CFLAGS)

LDADD = libjwhois.a lib/libgnu.a

jwhois_CFLAGS = $(AM_CFLAGS) $(WERROR_CFLAGS)
jwhois_LDADD = $(LDADD) $(LIBINTL) $(LIBICONV) $(LIBS)
jwhois_SOURCES = \
  src/jwhois.c \
  src/jwhois.h

libjwhois_a_CFLAGS = $(AM_CFLAGS) $(WERROR_CFLAGS)
libjwhois_a_SOURCES = \
  src/cache.c \
  src/cache.h \
  src/http.c \
  src/http.h \
  src/init.c \
  src/init.h \
  src/jconfig.c \
  src/jconfig.h \
  src/lookup.c \
  src/lookup.h \
  src/rwhois.c \
  src/rwhois.h \
  src/utils.c \
  src/utils.h \
  src/whois.c \
  src/whois.h

sysconf_DATA = example/jwhois.conf

info_TEXINFOS = doc/jwhois.texi
doc_jwhois_TEXINFOS = doc/fdl-1.3.texi
dist_man_MANS = doc/jwhois.1

# Avoid re-generating the man pages when building from tarballs.
if BUILD_FROM_GIT
doc/jwhois.1: jwhois
	$(AM_V_GEN)$(MKDIR_P) `dirname "$@"`; \
	LANGUAGE= $(HELP2MAN) --output="$@" "$(top_builddir)/$<"
endif

TEST_EXTENSIONS = .sh
SH_LOG_COMPILER = $(SHELL)
AM_TESTS_ENVIRONMENT = \
  confdir='$(abs_top_srcdir)/example' \
  PATH="$(abs_top_builddir)$(PATH_SEPARATOR)$$PATH"

TESTS = \
  tests/config.sh \
  $(check_PROGRAMS)

check_PROGRAMS = \
  tests/utils_strjoinv

noinst_HEADERS = tests/macros.h

EXTRA_DIST = \
  .version \
  ChangeLog.old \
  example/jwhois.conf \
  jwhois.spec.in \
  jwhois.spec \
  m4/gnulib-cache.m4 \
  tests/init.sh \
  $(TESTS)

BUILT_SOURCES = .version
.version:
	$(AM_V_GEN)echo $(VERSION) > $@-t && mv $@-t $@

dist-hook: gen-ChangeLog
	$(AM_V_GEN)echo $(VERSION) > $(distdir)/.tarball-version

gen_start_date = 2015-11-29
.PHONY: gen-ChangeLog
gen-ChangeLog:
	$(AM_V_GEN)if test -d $(srcdir)/.git; then			\
	  log_fix="$(srcdir)/build-aux/git-log-fix";			\
	  test -e "$$log_fix"						\
	    && amend_git_log="--amend=$$log_fix"			\
	    || amend_git_log=;						\
	  $(top_srcdir)/build-aux/gitlog-to-changelog			\
	    $$amend_git_log --since=$(gen_start_date) > $(distdir)/cl-t && \
	    { rm -f $(distdir)/ChangeLog &&				\
	      mv $(distdir)/cl-t $(distdir)/ChangeLog; }		\
	fi
