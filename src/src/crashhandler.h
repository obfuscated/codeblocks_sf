#ifndef CRASH_HANDLER
#define CRASH_HANDLER

#if (__WXMSW__)

class CrashHandler
{
//    typedef PVOID  (*AddHandler_t)(ULONG, PVECTORED_EXCEPTION_HANDLER);
//    typedef ULONG  (*RemoveHandler_t)(PVOID);
    typedef void *  (*AddHandler_t)(unsigned long, PVECTORED_EXCEPTION_HANDLER);
    typedef unsigned long  (*RemoveHandler_t)(void *);

    void * handler;
public:
    CrashHandler();
    ~CrashHandler();
};

#else

class CrashHandler
{
public:
    CrashHandler(){};
    ~CrashHandler(){};
};

#endif

#endif
