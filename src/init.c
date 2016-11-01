/* init.c - initialize global parameters
   Copyright (C) 1999-2005, 2007, 2015, 2016 Free Software Foundation, Inc.

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

#include <config.h>
#include "system.h"

/* Specification.  */
#include "init.h"

/* Default values.  */
static struct arguments _arguments = {
  .query_string = NULL,
  .cache = true,
  .forcelookup = false,
  .verbose = 0,
  .ghost = NULL,
  .gport = 0,
  .config = NULL,
  .redirect = true,
  .display_redirections = false,
  .whoisservers = NULL,
  .raw_query = false,
  .rwhois = false,
  .rwhois_display = NULL,
  .rwhois_limit = 0,
  .enable_whoisservers = true
};

struct arguments *arguments = &_arguments;
