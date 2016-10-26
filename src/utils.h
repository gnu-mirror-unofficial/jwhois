/* utils.h - declarations for various functions
   Copyright (C) 2001-2002, 2015, 2016 Free Software Foundation, Inc.

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

#ifndef UTILS_H
#define UTILS_H

#include "init.h"
#include "whois.h"

char *get_whois_server_domain_path(const char *hostname);
char *get_whois_server_option(const char *hostname, const char *key);
char *create_string(const char *fmt, ...);
int split_host_from_query (whois_query_t wq);
int make_connect(const char *, int);
int add_text_to_buffer(char **, const char *);
void timeout_init (void);

/* Join STC strings in STRV array with delimiter DELIM.  Return a
   pointer to the newly allocated result.*/
extern char *strjoinv (const char *delim, int stc, const char *strv[]);

/* Print every element from ARGS to the standard output.  Return the number of
   characters printed if no error is encountered.  Return a negative value
   otherwise.  */
extern int dump_arguments (struct arguments *args);

#endif
