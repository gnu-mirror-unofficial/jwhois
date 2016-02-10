/*
    This file is part of jwhois
    Copyright (C) 1999-2002, 2007, 2015, 2016  Free Software Foundation, Inc.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#ifdef STDC_HEADERS
# include <stdio.h>
# include <stdlib.h>
#endif

#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
# include <sys/socket.h>
#endif
#ifdef HAVE_NETINET_IN_H
# include <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
# include <netdb.h>
#endif

#include <jwhois.h>
#include <dns.h>
#include <string.h>

#ifdef ENABLE_NLS
# include <libintl.h>
# define _(s)  gettext(s)
#else
# define _(s)  (s)
#endif

/*
 *  This function looks up a hostname or IP number using the newer
 *  getaddrinfo() system call.
 *
 *  Returns:   !0   Error code (see gai_strerror())
 *             0    Success
 */
int
lookup_host_addrinfo(struct addrinfo **res, const char *host, int port)
{
  struct addrinfo hints;
  char ascport[10] = "whois";
  int error;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = PF_UNSPEC;

  hints.ai_socktype = SOCK_STREAM;
  if (port)
    sprintf(ascport, "%9.9d", port);

  error = getaddrinfo(host, ascport, &hints, res);

  if (error)
    {
      printf("[%s: %s]\n", host, gai_strerror(error));
      return error;
    }
  return 0;
}
