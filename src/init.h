/* init.h - declarations for parsing command line arguments
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

void parse_args (int argc, char *argv[]);

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
   via a function parameter.  This should be defined only in the main
   function.  */
extern struct arguments *arguments;

#endif
