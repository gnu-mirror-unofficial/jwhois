/* system.h - system-dependent declarations; include this first.
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

#ifndef JWHOIS_SYSTEM_H
#define JWHOIS_SYSTEM_H

#ifdef STDC_HEADERS
# include <stdio.h>
# include <stdlib.h>
# include <stdarg.h>
#endif

/* Use POSIX headers.  If they are not available, we use the substitute
   provided by gnulib.  */
#include <string.h>
#include <unistd.h>

#ifndef HAVE_MEMCPY
# define memcpy(d, s, n) bcopy ((s), (d), (n))
#endif

#ifndef MAXBUFSIZE
# define MAXBUFSIZE 1024
#endif

/* Access from source code to variables set by "configure" or "make".  */
#include "configmake.h"

/* Convenient functions that use 'malloc' with out-of-memory checking.  */
#include "xalloc.h"

/* Internationalization.  */
# include "gettext.h"
# define _(str) gettext (str)
# define N_(str) gettext_noop (str)

/* Convenient macros for string comparaison to increase readibility.  */
#define STREQ(s1, s2) (strcmp (s1, s2) == 0)
#define STRCASEEQ(s1, s2) (strcasecmp (s1, s2) == 0)
#define STRNCASEEQ(s1, s2, n) (strncasecmp (s1, s2, n) == 0)

#endif /* JWHOIS_SYSTEM_H */
