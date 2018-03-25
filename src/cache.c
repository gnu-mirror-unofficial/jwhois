/* cache.c - remember previous queries
   Copyright (C) 1999-2002, 2007, 2015, 2016 Free Software Foundation, Inc.

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
#include "cache.h"

#ifdef HAVE_SYS_FCNTL_H
# include <sys/fcntl.h>
#endif
#if TIME_WITH_SYS_TIME
# include <time.h>
#endif
#if !defined NOCACHE && defined HAVE_GDBM_OPEN
# ifdef HAVE_GDBM_H
#  include <gdbm.h>
# endif
#else
# if !defined NOCACHE && defined HAVE_DBM_OPEN
#  ifdef HAVE_NDBM_H
#   include <ndbm.h>
#  else
#   ifdef HAVE_DBM_H
#    include <dbm.h>
#   else
#    ifdef HAVE_DB1_NDBM_H
#     include <db1/ndbm.h>
#    endif
#   endif
#  endif
# endif
#endif

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include "init.h"
#include "jconfig.h"

#define DBM_MODE           S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP

#if !defined NOCACHE && defined HAVE_GDBM_OPEN
# define dbm_open(a,b,c)    gdbm_open(a, 0, b, c, 0)
# define DBM_COPTIONS       GDBM_WRCREAT
# define DBM_WOPTIONS       GDBM_WRITER
# define DBM_ROPTIONS       GDBM_READER
# define DBM_IOPTIONS       GDBM_REPLACE
# define dbm_store(a,b,c,d) gdbm_store(a,b,c,d)
# define dbm_close(a)       gdbm_close(a)
# define dbm_fetch(a,b)     gdbm_fetch(a,b)
#else
# if !defined NOCACHE && defined HAVE_DBM_OPEN
# define DBM_COPTIONS       O_RDWR|O_CREAT
# define DBM_WOPTIONS       O_RDWR
# define DBM_ROPTIONS       O_RDONLY
# define DBM_IOPTIONS       DBM_REPLACE
# endif
#endif

/*
 *  This function initialises the cache database and possibly converts it
 *  to a newer format if such exists. Returns -1 on error. 0 on success.
 */
int
cache_init(void)
{
#ifndef NOCACHE
  datum dbkey = {xstrdup ("#jwhois#cacheversion#1"), 22};
  datum dbstore = {xstrdup ("1"), 1};
#ifdef HAVE_GDBM_OPEN
  GDBM_FILE dbf;
#else
  DBM *dbf;
#endif

  if (!arguments->cache)
    return 0;

  jconfig_set();
  struct jconfig *j = jconfig_getone("jwhois", "cachefile");
  arguments->cfname = j ? j->value : xstrdup (LOCALSTATEDIR "/jwhois.db");

  if (arguments->verbose > 1)
    printf ("[Cache: Cache file name = \"%s\"]\n", arguments->cfname);

  jconfig_set();
  j = jconfig_getone("jwhois", "cacheexpire");

  char *buf;
  const char *ret = j ? j->value : CACHE_EXPIRE;
  arguments->cfexpire = strtol (ret, &buf, 10);
  if (*buf != '\0')
    {
      if (arguments->verbose)
        printf("[Cache: %s: %s]\n", _("Invalid expire time"), ret);
      arguments->cfexpire = 168;
    }

  if (arguments->verbose > 1)
    printf("[Cache: Expire time = %d]\n", arguments->cfexpire);

  umask(0);
  dbf = dbm_open (arguments->cfname, DBM_COPTIONS, DBM_MODE);
  if (!dbf)
    {
      if (arguments->verbose)
	printf ("[Cache: %s %s]\n", _("Unable to open"), arguments->cfname);
      arguments->cache = 0;
      return -1;
    }

  int iret = dbm_store(dbf, dbkey, dbstore, DBM_IOPTIONS);
  if (iret < 0)
    {
      if (arguments->verbose)
	printf("[Cache: %s]\n", _("Unable to store data in cache\n"));
      arguments->cache = 0;
    }
  dbm_close(dbf);
#endif
  return 0;
}

/*
 *  This stores the passed text in the database with the key `key'.
 *  Returns 0 on success and -1 on failure.
 */
int
cache_store(char *key, const char *text)
{
#ifndef NOCACHE
  datum dbkey;
  datum dbstore;
  int ret;
#ifdef HAVE_GDBM_OPEN
  GDBM_FILE dbf;
#else
  DBM *dbf;
#endif
  time_t *timeptr;
  char *ptr;

  if (arguments->cache)
    {
      dbkey.dptr = key;
      dbkey.dsize = strlen(key);

      ptr = malloc(strlen(text)+sizeof(time_t)+1);
      if (!ptr)
	return -1;
      memcpy(ptr+sizeof(time_t), text, strlen(text)+1);
      
      timeptr = (time_t *)ptr;
      *timeptr = time(NULL);
      
      dbstore.dptr = ptr;
      dbstore.dsize = strlen(text)+sizeof(time_t)+1;
      
      dbf = dbm_open(arguments->cfname, DBM_WOPTIONS, DBM_MODE);
      if (!dbf)
	return -1;
      else
	{
	  ret = dbm_store(dbf, dbkey, dbstore, DBM_IOPTIONS);
	  if (ret < 0)
	    return -1;
	  dbm_close(dbf);
	}
    }
#endif
  return 0;
}

/*
 *  Given a key, this function retrieves the text from the database
 *  and checks the expire time on it. If it is still valid data, it
 *  returns the number of bytes in text, else 0 or -1 on error.
 */
int
cache_read(char *key, char **text)
{
#ifndef NOCACHE
  datum dbkey;
  datum dbstore;
#ifdef HAVE_GDBM_OPEN
  GDBM_FILE dbf;
#else
  DBM *dbf;
#endif
#endif
  if (!arguments->cache)
    return 0;

#ifndef NOCACHE
  dbkey.dptr = key;
  dbkey.dsize = strlen(key);

  dbf = dbm_open (arguments->cfname, DBM_ROPTIONS, DBM_MODE);
  if (!dbf)
    return -1;
  dbstore = dbm_fetch(dbf, dbkey);
  if ((dbstore.dptr == NULL))
    {
      dbm_close(dbf);
      return 0;
    }

  time_t time_c;
  /* Ensure suitable alignment.  */
  memcpy (&time_c, dbstore.dptr, sizeof (time_c));
  if (((time(NULL) - time_c) / (60 * 60)) > arguments->cfexpire)
    {
      dbm_close(dbf);
      return 0;
    }
  *text = malloc(dbstore.dsize);
  if (!*text)
    return -1;
  memcpy(*text, (char *)(dbstore.dptr)+sizeof(time_t), dbstore.dsize-sizeof(time_t));
  dbm_close(dbf);

  return (dbstore.dsize-sizeof(time_t));
#else
  return 0;
#endif /* !NOCACHE */
}
