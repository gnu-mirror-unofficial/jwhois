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

extern int cache;
extern int forcelookup;
extern int verbose;
extern char *ghost;
extern int gport;
extern char *config;

extern char *cfname;
extern int cfexpire;
extern int redirect;
extern int display_redirections;
extern char *whoisservers;
extern int raw_query;
extern int rwhois;
extern int rwhois_limit;
extern char *rwhois_display;
extern int enable_whoisservers;
extern int connect_timeout;

/* Access from source code to variables set by "configure" or "make".  */
#include "configmake.h"

/* Internationalization.  */
# include "gettext.h"
# define _(str) gettext (str)
# define N_(str) gettext_noop (str)

#define STREQ(a, b) (strcmp (a, b) == 0)

#endif /* JWHOIS_H */
