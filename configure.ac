dnl Process this file with autoconf to produce a configure script.
# Copyright (C) 1999-2005, 2015, 2016  Free Software Foundation, Inc.
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

AC_INIT([GNU JWhois],
        m4_esyscmd([build-aux/git-version-gen .tarball-version]),
        [bug-jwhois@gnu.org])
AC_CONFIG_SRCDIR([src/jconfig.c])
AC_CONFIG_HEADER([src/config.h])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([gnu subdir-objects])
AM_SILENT_RULES([yes]) # Enable silent rules by default.

AH_TEMPLATE([CACHEFILE], [Default filename for the cache functionality])
AH_TEMPLATE([NOCACHE], [Define if you want to disable cache functionality])
AH_TEMPLATE([HAVE_DBM_OPEN], [Define if you have the dbm_open function. ])
AH_TEMPLATE([HAVE_GDBM_OPEN], [Define if you have the gdbm_open function. ])

AC_ARG_ENABLE([default-host],
  [AS_HELP_STRING([--enable-default-host=HOST],
     [default whois host (default whois.internic.net)])],
  [enable_default_host="$enableval"],
  [enable_default_host="whois.internic.net"])
AC_DEFINE_UNQUOTED([DEFAULT_HOST], ["$enable_default_host"],
  [Default Whois server.])

AC_ARG_ENABLE([whois-servers],
  [AS_HELP_STRING([--enable-whois-servers=DOMAIN],
     [default whois-servers.net domain (default whois-servers.net)])],
  [enable_whois_servers="$enableval"],
  [enable_whois_servers="whois-servers.net"])
AC_DEFINE_UNQUOTED([WHOIS_SERVERS], ["$enable_whois_servers"],
  [Default whois-servers.net domain for the whois-servers support])

AC_ARG_ENABLE([cache-expire],
  [AS_HELP_STRING([--enable-cache-expire=HOURS],
     [default expire time in hours for cached objects (default 168 hours)])],
  [enable_cache_expire="$enableval"],
  [enable_cache_expire="168"])
AC_DEFINE_UNQUOTED([CACHE_EXPIRE], ["$enable_cache_expire"],
  [Default expire time for the cache functionality])

AC_ARG_WITH([cache],
  [AS_HELP_STRING([--without-cache],
     [disable cache manager (see README for more info)])],
  [AS_IF([test x$withval = xno],
     [AC_DEFINE([NOCACHE], [1])
      cache=no],
     [cache=yes])],
  [cache=yes])
AC_SUBST([cache])

AC_PROG_INSTALL
AC_PROG_CC
gl_EARLY

AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.19.2])

AM_ICONV

AC_C_BIGENDIAN

# For gnulib
gl_INIT

AC_ARG_ENABLE([gcc-warnings],
  [AS_HELP_STRING([--enable-gcc-warnings],
     [turn on many GCC warnings (for developers; best with GNU make)])],
  [AS_CASE([$enableval],
     [yes|no], [],
     [AC_MSG_ERROR([bad value $enableval for gcc-warnings option])])
   gl_gcc_warnings=$enableval],
  [gl_gcc_warnings=no])
if test "$enable_gcc_warnings" = yes; then
  warn_common='-Wall -Wno-sign-compare -Wcast-align -Wdocumentation
  -Wformat -Wpointer-arith -Wwrite-strings -Wbad-function-cast -Wshadow
  -Wstrict-prototypes'
  for i in $warn_common;
  do
    gl_WARN_ADD([$i], [WARN_CFLAGS])
  done
  gl_WARN_ADD([-Werror], [WERROR_CFLAGS])
fi

AC_CHECK_FUNC(connect,,
  AC_CHECK_LIB(socket, connect))
AC_CHECK_FUNC(socket,,
  AC_CHECK_LIB(socket, socket)
  AC_CHECK_LIB(inet, socket))
AC_CHECK_FUNCS(getipnodebyname)

AC_CHECK_LIB(inet6, main,
  AC_CHECK_LIB(resolv, main))

AC_CHECK_FUNCS(memcpy strtol)
AC_CHECK_FUNCS(strcasecmp strncasecmp getopt_long)
AC_HEADER_STDC([])
AC_CHECK_HEADERS([sys/fcntl.h malloc.h stdint.h inttypes.h idna.h])
AC_HEADER_TIME


dnl check for inet_pton
AC_CHECK_FUNCS(inet_pton)
dnl Some systems have it, but not IPv6
if test "$ac_cv_func_inet_pton" = "yes" ; then
AC_MSG_CHECKING(if inet_pton supports IPv6)
AC_TRY_RUN(
  [
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
int main()
  {
    struct in6_addr addr6;
    if (inet_pton(AF_INET6, "::1", &addr6) < 1)
      exit (EXIT_FAILURE);
    else
      exit (EXIT_SUCCESS);
  }
  ], [
       AC_MSG_RESULT(yes)
       AC_DEFINE([HAVE_INET_PTON_IPV6], [1],
         [Define to 1 if inet_pton supports IPv6.])
     ], AC_MSG_RESULT(no),AC_MSG_RESULT(no))
fi

AC_ARG_WITH(libidn, AC_HELP_STRING([--with-libidn=[DIR]],
                                     [Support IDN (needs GNU Libidn)]),
       libidn=$withval, libidn=yes)
 if test "$libidn" != "no"; then
   if test "$libidn" != "yes"; then
     LDFLAGS="${LDFLAGS} -L$libidn/lib"
     CPPFLAGS="${CPPFLAGS} -I$libidn/include"
   fi
   AC_CHECK_HEADER(idna.h,
     AC_CHECK_LIB(idn, stringprep_check_version,
       [libidn=yes LIBS="${LIBS} -lidn"], libidn=no),
     libidn=no)
 fi
 if test "$libidn" != "no" ; then
  AC_DEFINE([LIBIDN], [1],
    [Define to 1 if you want IDN support.])
 else
  AC_MSG_WARN([Libidn not found])
fi
AC_MSG_CHECKING([if Libidn should be used])
AC_MSG_RESULT($libidn)


found=no
if test x$cache = xyes; then
  AC_CHECK_FUNC(dbm_open,
    [AC_DEFINE([HAVE_DBM_OPEN])
    found=yes])
  if test x$found = xno; then
    AC_CHECK_LIB(gdbm, gdbm_open,
      [AC_DEFINE([HAVE_GDBM_OPEN])
      LIBS="-lgdbm $LIBS"
      found=yes])
  fi
  if test x$found = xno; then
    AC_CHECK_LIB(ndbm, dbm_open,
      [AC_DEFINE([HAVE_DBM_OPEN])
      LIBS="-lndbm $LIBS"
      found=yes])
  fi
  if test x$found = xno; then
    AC_MSG_WARN("You don\'t have any dbm libraries installed -- disabling cache functions")
    AC_DEFINE([NOCACHE],1)
    cache=no
  else
    AC_CHECK_HEADERS(gdbm.h ndbm.h dbm.h db1/ndbm.h)
  fi
fi

if test "$ac_cv_lib_intl_bindtextdomain" = "yes" \
   || test "$ac_cv_lib_intl_gettext" = "yes"; then
  LIBS="-lintl $LIBS"
  AC_CHECK_HEADERS(libintl.h)
fi

## Are we building from git checked-out sources, or a tarball ?  This allows
## specifying some rules used only when bootstrapping.
AM_CONDITIONAL([BUILD_FROM_GIT], [test -d "$srcdir/.git"])

## GNU help2man creates man pages from --help output.
AM_MISSING_PROG([HELP2MAN], [help2man])

AX_CODE_COVERAGE

AC_CONFIG_FILES([Makefile
                 jwhois.spec
                 lib/Makefile
                 po/Makefile.in])
AC_OUTPUT
