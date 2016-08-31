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

# include <config.h>

#ifdef STDC_HEADERS
# include <stdio.h>
# include <stdlib.h>
#endif

# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <netdb.h>
# include <locale.h>

#include <jconfig.h>
#include <jwhois.h>
#include <regex.h>
#include <whois.h>
#include <http.h>
#include <cache.h>
#include <init.h>
#include <lookup.h>
#include <rwhois.h>
#include <utils.h>

#include <errno.h>
#include <progname.h>
#include <string.h>

#ifdef HAVE_ICONV
# include <iconv.h>
# include <langinfo.h>
#endif

#ifdef LIBIDN
# include <idna.h>
#endif

/* Forward declarations.  */
static int jwhois_query (whois_query_t wq, char **text);

int
main(int argc, char **argv)
{
  int ret;
  char *text, *cachestr;
  whois_query_t wq;

  set_program_name (argv[0]);
  setlocale(LC_ALL, "");
  bindtextdomain(PACKAGE, LOCALEDIR);
  textdomain(PACKAGE);

  re_syntax_options = RE_SYNTAX_EMACS;
  wq = wq_init ();

  /* Parse command line arguments and initialize the cache */
  parse_args (argc, argv);
  cache_init();
  timeout_init();

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
  cachestr = malloc (strlen (wq->query) + strlen (wq->host) + 2);
  if (!cachestr)
    {
      printf("[%s]\n", _("Error allocating memory"));
      exit (EXIT_FAILURE);
    }
  snprintf (cachestr, strlen (wq->query) + strlen (wq->host) + 2, "%s:%s",
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
  char *tmp, *tmp2, *oldquery, *curdata;
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

  if ((tmp && 0 == strcasecmp(tmp, "true")) || arguments->rwhois)
    {
      ret = rwhois_query(wq, &curdata);
    }
  else
    {
      if (tmp2 && 0 == strcasecmp(tmp2, "true"))
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
	  *text = realloc(*text, strlen (*text) + strlen (curdata) + 1);
	  if (*text == NULL)
	    exit (EXIT_FAILURE);

	  strcat(*text, curdata);
	  free(curdata);
	}
    }
  if (ret > 0)
    return jwhois_query(wq, text);
  else
    return 0;
}
