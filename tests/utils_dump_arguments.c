/* Test of dump_arguments function.
   Copyright (C) 2016 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include <config.h>
#include "system.h"

/* Declaration.  */
#include "utils.h"

#include <progname.h>
#include "macros.h"

int
main (void)
{
  set_program_name ("dump_arguments");
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  /* ARGUMENTS is a global variable containing the default values.  */
  int ret = dump_arguments (arguments);
  return (ret < 0) ? EXIT_FAILURE : EXIT_SUCCESS;
}
