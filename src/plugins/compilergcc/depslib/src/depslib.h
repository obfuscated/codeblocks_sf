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

#ifndef DEPSLIB_H
#define DEPSLIB_H

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/* This structure is returned by depsGetStats() */
struct depsStats
{
	long scanned; /* the number of files scanned for #include */
	long cache_used; /* This many files were not scanned for #include because
					   up-to-date entries in the cache were found */
	long cache_updated; /* This many files had entries in the cache updated */
};

typedef struct _depsRef *depsRef;

/*
 * -- depsStart --
 * Call this once before the build begins. There must be a matching call
 * to depsDone(). depsStart() may be called multiple times as long as
 * depsDone() is called before the next call to depsStart().
 */
extern void depsStart(void);

/*
 * -- depsSetCWD --
 * Call this to specify the current working directory against which the 'path'
 * arguments to depsAddSearchDir() and depsScanForHeaders() are considered
 * relative to. depslib uses 'cwd' to convert relative paths to absolute paths.
 *
 * arg cwd (in) -> the absolute pathname of a directory.
 */
extern void depsSetCWD(const char *cwd);

/*
 * -- depsCacheRead() --
 * Call this to read a header cache file.
 *
 * arg path (in) -> the path of a header cache file
 */
extern void depsCacheRead(const char *path);

/*
 * -- depsCacheWrite() --
 * Call this to write a header cache file.
 *
 * arg path (in) -> the path of a header cache file
 */
extern void depsCacheWrite(const char *path);

/*
 * -- depsSearchStart --
 * Call this after depsStart() to initialize the list of search dirs.
 */
extern void depsSearchStart(void);

/*
 * -- depsAddSearchDir --
 * Call this repeatedly after depsSearchStart() to append a directory to
 * the list of directories to look for header files in.
 *
 * arg path (in) -> the relative (or absolute) directory path
 */
extern void depsAddSearchDir(const char *path);

/*
 * -- depsScanForHeaders --
 * Call this repeatedly after depsSearchStart() to recursively scan a file for
 * #include statements. For #include <foo.h> only the directories provided by
 * previous calls to depsAddSearchDir() will be searched. For #include "foo.h"
 * the directory of the file argument to this routine will be searched first
 * followed by the current list of search directories.
 *
 * arg path (in) -> the relative (or absolute) file path
 * result -> an opaque pointer to information about the given file, or NULL
 *   if the file was not found
 */
extern depsRef depsScanForHeaders(const char *path);

/*
 * -- depsGetNewest --
 * Call this to get the absolute path and file modification time of the file
 * that has the most-recent file modification time. This could be the same
 * file passed to depsScanForHeaders() or one of its descendent header files.
 * 
 * arg ref (in) -> the result of depsScanForHeaders()
 * arg time (out) -> the file modification time of the most-recent file
 * result -> the absolute path of the most-recent file. On Windows this will
 *   be all lower-case.
 */
extern const char *depsGetNewest(depsRef ref, time_t *time);

/*
 * -- depsTimeStamp --
 * Return the timestamp for a file.
 *
 * arg path (in) -> the relative (or absolute) file path
 * arg time (out) -> the file modification time or zero if not found
 */
extern void depsTimeStamp(const char *path, time_t *time);

/*
 * -- depsGetStats --
 * Returns statistics about the operation of depslib.
 *
 * arg stats (out) -> pointer to stats info
 */
extern void depsGetStats(struct depsStats *stats);

/*
 * -- depsDone --
 * Call this after depsStart() to free all the memory allocated by depslib.
 * After calling this routine, depsStart() may be called again.
 */
extern void depsDone(void);

#define DEPS_ERROR_ALREADY_INIT 1
#define DEPS_ERROR_NOT_INIT 2
#define DEPS_ERROR_NOT_SEARCH 3

/*
 * -- depsError --
 * Return error code from last call.
 */
extern int depsError(void);

#ifdef __cplusplus
}
#endif

#endif /* DEPSLIB_H */
