/*
 * -= Copyright 2005 Tim Baker (treectrl@hotmail.com) =-
 *
 * This file is part of depslib.
 *
 * License is hereby granted to use this software and distribute it
 * freely, as long as this copyright notice is retained and modifications 
 * are clearly marked.
 *
 * ALL WARRANTIES ARE HEREBY DISCLAIMED.
 */

extern void cache_read(const char *path);
extern void cache_write(const char *path);
extern int cache_check(const char *path, time_t time, LIST **includes);
extern void cache_enter(const char *path, time_t time, LIST *includes);
extern void donecache(void);

