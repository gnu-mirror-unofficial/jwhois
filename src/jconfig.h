/* jconfig.h - declarations for configuration file management
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

#ifndef JCONFIG_H
#define JCONFIG_H

struct jconfig {
	char	*domain;
	char	*key;
	char	*value;
	int	line;
	struct jconfig	*next;
};

void jconfig_set(void);
struct jconfig *jconfig_next(const char *);
struct jconfig *jconfig_next_all(const char *);
void jconfig_end(void);
struct jconfig *jconfig_getone(const char *, const char *);

int jconfig_add(const char *, const char *, const char *, int);
void jconfig_free(void);
void jconfig_parse_file(FILE *);

#endif
