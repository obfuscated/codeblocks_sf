/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef CRASH_HANDLER
#define CRASH_HANDLER

#if (__WXMSW__)

#include <winnt.h>

class CrashHandler
{
//    typedef PVOID  (*AddHandler_t)(ULONG, PVECTORED_EXCEPTION_HANDLER);
//    typedef ULONG  (*RemoveHandler_t)(PVOID);
    typedef void *  (*AddHandler_t)(unsigned long, PVECTORED_EXCEPTION_HANDLER);
    typedef unsigned long  (*RemoveHandler_t)(void *);

    void * handler;
public:
    CrashHandler(bool bDisabled);
    ~CrashHandler();
};

#else

class CrashHandler
{
public:
    CrashHandler(bool bDisabled){};
    ~CrashHandler(){};
};

#endif

#endif
