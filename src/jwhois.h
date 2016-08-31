/* jwhois.h - declarations for common macros and program parameters
   Copyright (C) 1999, 2001-2002, 2016 Free Software Foundation, Inc.

   This file is part of GNU JWhois.

   GNU JWhois is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   GNU JWhois is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GNU JWhois.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef JWHOIS_H
#define JWHOIS_H

#ifndef HAVE_MEMCPY
# define memcpy(d, s, n) bcopy ((s), (d), (n))
#endif

#ifndef MAXBUFSIZE
# define MAXBUFSIZE 1024
#endif

struct arguments {
  /* Contains the actual query.  */
  char *query_string;

  /* This is set if caching is enabled */
  int cache;

  /* Set if checking for a cached copy of a document should be bypassed */
  int forcelookup;

  /* Verbose debugging output */
  int verbose;

  /* Host specified on the command line */
  char *ghost;

  /* Port specified on the command line */
  int gport;

  /* Name of the current configuration file */
  char *config;

  /* Name of the cache database */
  char *cfname;

  /* Default whois-servers.net domain */
  char *whoisservers;

  /* Default expire time for cached objects */
  int cfexpire;

  /* Whether or not to use lookup_redirect() on whois server output */
  int redirect;

  /* Set to 1 to display all redirects, otherwise display only final reply */
  int display_redirections;

  /* Set to 1 to send query in raw form to the host instead of mangling it
     through query-format */
  int raw_query;

  /* Set to 1 to force an rwhois query */
  int rwhois;

  /* Set to a valid display name for rwhois queries */
  char *rwhois_display;

  /* Set to a valid limit for rwhois queries */
  int rwhois_limit;

  /* Set to 0 to completely disable whois-servers.net service support */
  int enable_whoisservers;

  /* Timeout value for connect calls in seconds */
  int connect_timeout;
};

/* XXX: Temporary global variable necessary until the rest of the code uses it
   via a function parameter.  This should only be defined only in the main
   function.  */
extern struct arguments *arguments;

/* Access from source code to variables set by "configure" or "make".  */
#include "configmake.h"

/* Internationalization.  */
# include "gettext.h"
# define _(str) gettext (str)
# define N_(str) gettext_noop (str)

#define STREQ(a, b) (strcmp (a, b) == 0)

#endif /* JWHOIS_H */
