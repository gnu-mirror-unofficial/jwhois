/* init.c - parse command line arguments
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

#include <argp.h>
#include <argp-version-etc.h>
#include <progname.h>
#include "jconfig.h"
#include "utils.h"

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

const char *argp_program_bug_address = PACKAGE_BUGREPORT;

static const char *authors[] = { "the JWhois authors", NULL };

static char args_doc[] = N_("QUERY");

static char doc[] = N_("Request information about QUERY.");

/* Keys for options without short-options.  */
enum
{ OPT_DISPLAY = CHAR_MAX + 1, OPT_LIMIT };

static struct argp_option options[] = {
  {"verbose", 'v', 0, 0,
   N_("verbose debug output")},
  {"config", 'c', N_("FILE"), 0,
   N_("use FILE as configuration file")},
  {"host", 'h', N_("HOST"), 0,
   N_("explicitly query HOST")},
  {"no-redirect", 'c', 0, 0,
   N_("disable content redirection")},
  {"no-whoisservers", 's', 0, 0,
   N_("disable whois-servers.net service support")},
  {"raw", 'a', 0, 0,
   N_("disable reformatting of the query")},
  {"display-redirections", 'i', 0, 0,
   N_("display all redirects instead of hiding them")},
  {"port", 'p', N_("PORT"), 0,
   N_("use port number PORT (in conjunction with HOST)")},
  {"rwhois", 'r', 0, 0,
   N_("force an rwhois query to be made")},
  {"rwhois-display", OPT_DISPLAY, N_("DISPLAY"), 0,
   N_("sets the display option in rwhois queries")},
  {"rwhois-limit", OPT_LIMIT, N_("LIMIT"), 0,
   N_("sets the maximum number of matches to return")},
#ifndef NOCACHE
  {"force-lookup", 'f', 0, 0,
   N_("force lookup even if the entry is cached")},
  {"disable-cache", 'd', 0, 0,
   N_("disable cache functions")},
#endif
  {0, 0, 0, 0, 0}
};

/* Parse a single option.  */
static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  char *ret;
  FILE *in;

  switch (key)
    {
    case 'v':
      arguments->verbose += 1;
      break;
    case 'f':
      arguments->forcelookup = 1;
      break;
    case 'd':
      arguments->cache = 0;
      break;
    case 'n':
      arguments->redirect = 0;
      break;
    case 'a':
      arguments->raw_query = 1;
      break;
    case 'i':
      arguments->display_redirections = 1;
      break;
    case 's':
      arguments->enable_whoisservers = 0;
      break;
    case 'r':
      arguments->rwhois = 1;
      break;
    case 'c':
      arguments->config = arg;
      break;
    case 'h':
      arguments->ghost = arg;
      break;
    case OPT_DISPLAY:
      arguments->rwhois_display = arg;
      break;
    case OPT_LIMIT:
      arguments->rwhois_limit = strtol (arg, &ret, 10);
      if (*ret != '\0')
        printf ("[%s (%s)]\n", _("Invalid limit"), arg);
      break;
    case 'p':
      arguments->gport = strtol (arg, &ret, 10);
      if (*ret != '\0')
        printf ("[%s: %s]\n", _("Invalid port number"), arg);
      break;
    case ARGP_KEY_NO_ARGS:
      argp_usage (state);
      break;
    case ARGP_KEY_ARGS:
      arguments->query_string =
        strjoinv (" ", state->argc - state->next,
                  /* Fix 'incompatible-pointer-types' warning.  */
                  (const char **) state->argv + state->next);
      break;
    case ARGP_KEY_FINI:
      if (arguments->config)
        {
          in = fopen (arguments->config, "r");
          if (!in)
            {
              printf ("[%s: %s]\n", arguments->config, _("Unable to open"));
              exit (EXIT_FAILURE);
            }
        }
      else
        {
          in = fopen (SYSCONFDIR "/jwhois.conf", "r");
          if (!in && arguments->verbose)
            printf ("[%s: %s]\n",
                    SYSCONFDIR "/jwhois.conf", _("Unable to open"));
          else
            arguments->config = xstrdup (SYSCONFDIR "/jwhois.conf");
        }
      if (in)
        {
          jconfig_parse_file (in);
          fclose (in);
        }
      if (arguments->verbose > 1)
        dump_arguments (arguments);
      break;
    case ARGP_KEY_ARG:
    default:
      return ARGP_ERR_UNKNOWN;
    }
  return 0;
}

/* argp parser.  */
static struct argp argp = { options, parse_opt, args_doc, doc };

void
parse_args (int argc, char *argv[])
{
  /* Parse command line arguments.  */
  argp_version_setup (program_name, authors);
  argp_parse (&argp, argc, argv, 0, NULL, NULL);
}
