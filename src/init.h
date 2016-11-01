/* init.h - declarations for initializing global parameters
   Copyright (C) 2002, 2016 Free Software Foundation, Inc.

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

#ifndef INIT_H
#define INIT_H

#include <stdbool.h>

struct arguments {
  /* Contains the actual query.  */
  char *query_string;

  /* This is set if caching is enabled */
  bool cache;

  /* Set if checking for a cached copy of a document should be bypassed */
  bool forcelookup;

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
  bool redirect;

  /* Set to TRUE to display all redirects, otherwise display only final reply */
  bool display_redirections;

  /* Set to TRUE to send query in raw form to the host instead of mangling it
     through query-format */
  bool raw_query;

  /* Set to TRUE to force an rwhois query */
  bool rwhois;

  /* Set to a valid display name for rwhois queries */
  char *rwhois_display;

  /* Set to a valid limit for rwhois queries */
  int rwhois_limit;

  /* Set to FALSE to completely disable whois-servers.net service support */
  bool enable_whoisservers;

  /* Timeout value for connect calls in seconds */
  int connect_timeout;
};

/* XXX: Temporary global variable necessary until the rest of the code uses it
   via a function parameter.  This should be defined only in the main
   function.  */
extern struct arguments *arguments;

#endif
