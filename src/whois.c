/* whois.c - WHOIS queries
   Copyright (C) 2001-2002, 2007, 2015, 2016 Free Software Foundation, Inc.

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
#include "whois.h"

#include <errno.h>
#include <regex.h>
#include "init.h"
#include "jconfig.h"
#include "lookup.h"
#include "utils.h"

/* Forward declarations.  */
static int whois_read (int fd, char **ptr, const char *host);

whois_query_t
wq_init (void)
{
  whois_query_t wq = xmalloc (sizeof (struct s_whois_query));
  wq->host = NULL;
  wq->port = 0;
  wq->query = NULL;
  wq->domain = NULL;

  return wq;
}

void
wq_free (whois_query_t wq)
{
  /* PORT, DOMAIN struct members are not dynamically allocated. */
  free (wq->host);
  free (wq->query);
  free (wq);
}

char *
wq_get_query (whois_query_t wq)
{
  return xstrdup (wq->query);
}

void
wq_set_query (whois_query_t wq, char *query)
{
  free (wq->query);
  wq->query = xstrdup (query);
}

/*
 *  This function takes a filedescriptor as an argument, makes an whois
 *  query to that host:port. If successfull, it returns the result in the block
 *  of text pointed to by text.
 *
 *  Returns:   -1 Error
 *              0 Success
 */
int
whois_query (whois_query_t wq, char **text)
{
  int ret, sockfd;
  char *tmpqstring;

  printf("[%s %s]\n", _("Querying"), wq->host);

  while (1)
    {
      sockfd = make_connect(wq->host, wq->port);

      if (sockfd < 0)
	{
	  printf(_("[Unable to connect to remote host]\n"));
	  return -1;
	}

      tmpqstring = xmalloc (strlen (wq->query) + 3);
      strncpy(tmpqstring, wq->query, strlen(wq->query)+1);
      strcat(tmpqstring, "\r\n");

      write(sockfd, tmpqstring, strlen(tmpqstring));

      ret = whois_read(sockfd, text, wq->host);

      if (ret < 0)
	{
	  printf("[%s %s:%d]\n", _("Error reading data from"),
		 wq->host, wq->port);
	  exit (EXIT_FAILURE);
	}
      if (arguments->redirect)
        {
          ret = lookup_redirect(wq, *text);
          if ((ret < 0) || (ret == 0))
	    break;

	  return 1;
	  break;
        }
      else
        {
          break;
        }
    }
  return 0;
}

/*
 *  This reads input from a file descriptor and stores the contents
 *  in the indicated pointer. Returns the number of bytes stored in
 *  memory or -1 upon error.
 */
static int
whois_read (int fd, char **ptr, const char *host)
{
  unsigned int count, start_count;
  int ret;
  char data[MAXBUFSIZE];
  fd_set rfds;

  count = 0;

  add_text_to_buffer(ptr, create_string("[%s]\n", host));

  start_count = strlen(*ptr);

  do
    {
      FD_ZERO(&rfds);
      FD_SET(fd, &rfds);
      ret = select(fd + 1, &rfds, NULL, NULL, NULL);

      if (ret <= 0)
        return -1;

      ret = read(fd, data, MAXBUFSIZE-1);

      if (ret >= 0)
	{
	  count += ret;
	  *ptr = realloc(*ptr, start_count+count+1);
	  if (!*ptr)
	    return -1;
	  strncat(*ptr, data, ret);
	}
    }
  while (ret != 0);

  return count;
}
