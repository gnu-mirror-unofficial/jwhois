/* jwhois - extended Whois client
   Copyright (C) 1999,2001-2002, 2007, 2015, 2016 Free Software Foundation, Inc.

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

#ifdef HAVE_ICONV
# include <iconv.h>
# include <langinfo.h>
#endif

#ifdef LIBIDN
# include <idna.h>
#endif

#include <argp.h>
#include <argp-version-etc.h>
#include <errno.h>
#include <locale.h>
#include <progname.h>
#include <netdb.h>
#include <netinet/in.h>
#include <regex.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "cache.h"
#include "http.h"
#include "init.h"
#include "jconfig.h"
#include "lookup.h"
#include "rwhois.h"
#include "utils.h"
#include "whois.h"

/* Forward declarations.  */
static int jwhois_query (whois_query_t wq, char **text);
static error_t parse_opt (int key, char *arg, struct argp_state *state);

/* Keys for options without short-options.  */
enum
{ OPT_DISPLAY = CHAR_MAX + 1, OPT_LIMIT };

/* Static variables for argp. */
static struct argp_option options[] = {
  {"verbose", 'v', 0, 0,
   N_("verbose debug output")},
  {"config", 'c', N_("FILE"), 0,
   N_("use FILE as configuration file")},
  {"host", 'h', N_("HOST"), 0,
   N_("explicitly query HOST")},
  {"no-redirect", 'n', 0, 0,
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

static struct argp argp = {
  .options = options,
  .parser = parse_opt,
  .args_doc = N_("QUERY"),
  .doc = N_("Request information about QUERY.")
};

static const char *authors[] = { "the JWhois authors", NULL };

const char *argp_program_bug_address = PACKAGE_BUGREPORT;

int
main (int argc, char *argv[])
{
  int ret;
  char *text;
  whois_query_t wq;

  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  re_syntax_options = RE_SYNTAX_EMACS;
  wq = wq_init ();

  /* Parse command line arguments and initialize the cache */
  argp_version_setup (program_name, authors);
  argp_parse (&argp, argc, argv, 0, NULL, NULL);
  cache_init ();
  timeout_init ();

#ifdef LIBIDN
  char *idn;
  int rc = idna_to_ascii_lz (arguments->query_string, &idn, 0);
  if (rc != IDNA_SUCCESS)
    {
      printf ("[IDN encoding of '%s' failed with error code %d]\n",
              arguments->query_string, rc);
      exit (EXIT_FAILURE);
    }
  wq_set_query (wq, idn);
  free (idn);
#else
  wq_set_query (wq, arguments->query_string);
#endif
  free (arguments->query_string);

  if (arguments->ghost)
    {
      if (arguments->verbose > 1)
	printf ("[Calling %s:%d directly]\n",
		arguments->ghost, arguments->gport);

      wq->host = arguments->ghost;
      wq->port = arguments->gport;
    }
  else if (split_host_from_query (wq))
    {
      if (arguments->verbose > 1)
	printf("[Calling %s directly]\n", wq->host);
    }
  else
    {
      ret = lookup_host(wq, NULL);
      if (ret < 0)
	{
	  printf ("[%s]\n", _("Fatal error searching for host to query"));
	  exit (EXIT_FAILURE);
	}
    }

  text = NULL;

#ifndef NOCACHE
  char *cachestr = xmalloc (strlen (wq->query) + strlen (wq->host) + 2);
  snprintf(cachestr, strlen (wq->query) + strlen (wq->host) + 2, "%s:%s",
           wq->host, wq->query);

  if (!arguments->forcelookup && arguments->cache)
    {
      if (arguments->verbose > 1)
        printf ("[Looking up entry in cache]\n");

      ret = cache_read (cachestr, &text);
      if (ret < 0)
        {
          printf ("[%s]\n", _("Error reading cache"));
          exit (EXIT_FAILURE);
        }
      else if (ret > 0)
        {
          printf ("[%s]\n%s", _("Cached"), text);
          exit (EXIT_SUCCESS);
        }
    }
#endif

  jwhois_query (wq, &text);
  wq_free (wq);

#ifndef NOCACHE
  if (arguments->cache)
    {
      if (arguments->verbose > 1)
        printf ("[Storing in cache]\n");

      ret = cache_store (cachestr, text);
      if (ret < 0)
        printf ("[%s]\n", _("Error writing to cache"));
    }
#endif

  printf("%s", text);
  exit (EXIT_SUCCESS);
}

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
      /* XXX: Opening Config file should be handled outside of this file.  */
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
          in = fopen(SYSCONFDIR "/jwhois.conf", "r");
          if (!in && arguments->verbose)
            printf ("[%s: %s]\n",
                    SYSCONFDIR "/jwhois.conf", _("Unable to open"));
          else
            arguments->config = xstrdup (SYSCONFDIR "/jwhois.conf");
        }
      if (in)
        {
          jconfig_parse_file(in);
          fclose(in);
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


/*
 * Attempt to convert string if result encoding is specified in the config
 * file.
 * */
static char *
convert_charset (whois_query_t wq, char *curdata)
{
#ifdef HAVE_ICONV
  const char *charset;

  charset = get_whois_server_option(wq->host, "answer-charset");
  if (charset != NULL)
    {
      iconv_t cd;

      cd = iconv_open(nl_langinfo(CODESET), charset);
      if (cd != (iconv_t)-1)
	{
	  char *buf, *src;
	  size_t src_left, dest_size, dest_pos, res;

	  src = curdata;
	  src_left = strlen(src);
	  dest_size = src_left;
	  buf = malloc(dest_size);
	  if (buf == NULL)
	    goto error;
	  dest_pos = 0;

	  res = 0;
	  while (src_left != 0)
	    {
	      char *dest;
	      size_t dest_left;
	      
	      dest = buf + dest_pos;
	      dest_left = dest_size - dest_pos;
	      res = iconv(cd, &src, &src_left, &dest, &dest_left);
	      if (res == (size_t)-1 && errno != E2BIG)
		goto error;
	      dest_pos = dest - buf;
	      dest_size *= 2;
	      buf = realloc(buf, dest_size);
	      if (buf == NULL)
		goto error;
	    }
	  buf[dest_pos] = 0;
	  
	  iconv_close(cd);
	  free (curdata);
	  return buf;
	  
	error:
	  free(buf);
	  iconv_close(cd);
	}
    }
#endif
  (void)wq;
  return curdata;
}

/*
 *  This is the routine that actually performs a query. It selects
 *  the method to use for the host and then calls the correct routine
 *  to make the query. If the return value of the subroutine is above
 *  0, it found a redirect to another server, so jwhois_query() promptly
 *  follows it there. A return value of -1 is always a fatal error.
 */
static int
jwhois_query (whois_query_t wq, char **text)
{
  char *tmp, *tmp2, *oldquery = NULL, *curdata;
  int ret;

  if (!arguments->display_redirections)
    *text = NULL;
  
  if (!arguments->raw_query)
    {
      oldquery = wq->query;
      wq->query = (char *)lookup_query_format(wq);
    }

  tmp = (char *)get_whois_server_option(wq->host, "rwhois");
  tmp2 = (char *)get_whois_server_option(wq->host, "http");
  curdata = NULL;

  if ((tmp && STRCASEEQ (tmp, "true")) || arguments->rwhois)
    {
      ret = rwhois_query(wq, &curdata);
    }
  else
    {
      if (tmp2 && STRCASEEQ (tmp2, "true"))
	ret = http_query(wq, &curdata);
      else
	ret = whois_query(wq, &curdata);
      
    }

  if (!arguments->raw_query)
    {
      free(wq->query);
      wq->query = oldquery;
    }

  if (ret < 0)
    exit (EXIT_FAILURE);
    
  if (curdata != NULL)
    {
      curdata = convert_charset(wq, curdata);
      if (*text == NULL)
	*text = curdata;
      else
	{
	  *text = xrealloc (*text, strlen (*text) + strlen (curdata) + 1);
	  strcat(*text, curdata);
	  free(curdata);
	}
    }
  if (ret > 0)
    return jwhois_query(wq, text);
  else
    return 0;
}
