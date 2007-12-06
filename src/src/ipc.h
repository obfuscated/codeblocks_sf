#ifndef IPC_H
#define IPC_H

#include <wx/thread.h>
#include "cbexception.h"

#ifndef __WIN32__
	#include <sys/types.h>
	#include <sys/ipc.h>
	#include <sys/shm.h>
	#include <fcntl.h>
#else
	#include "wx/msw/wrapwin.h"
#endif

class wxString;

static const int ipc_buf_size = 1024*64;

#ifdef __WIN32__
    typedef HANDLE shm_handle_t;
    typedef HANDLE semaphore_t;
#elif defined(__APPLE__) && defined(__MACH__)
    typedef int shm_handle_t;
    typedef mach_port_t semaphore_t;
#else
    typedef int shm_handle_t;
    typedef int semaphore_t;
#endif


class SharedMemory
{
	shm_handle_t handle;

	semaphore_t sem;
	semaphore_t sem_w;

	void* shared;
	bool ok;
	bool server;

public:

	enum rw_t{ reader, writer };

	SharedMemory();
	~SharedMemory();

	bool OK() const { return ok; };

	void* BasePointer() const { return shared; };
	size_t Size() const { return ipc_buf_size; };

	bool Server() const { return server; };
	bool Client() const { return !server; };


    /*
     * Lock(reader) locks "as reader", not "the reader semaphore", i.e. it
     *   1. locks the reader semaphore
     *   2. locks the writer mutex, so the shared memory cannot be written while we read it
     *
     * Lock(writer) locks "as writer", this is equivalent to locking "the writer mutex"
     *
     * Unlock(reader) unlocks "as reader", i.e. it actually unlocks the writer mutex
     *   which the caller is still holding, so another process can write to the shared memory area again.
     *   It does not release the reader semaphore, since it should block on it on the next iteration.
     *
     * Unlock(writer) locks "as writer", i.e. it
     *   1. unlocks the reader semaphore, waking up the Server thread
     *   2. unlocks the writer mutex, so the Server thread can acquire it and prevent other processes from writing
     */
	bool Lock(rw_t rw);
	void Unlock(rw_t rw);
};





class IPC : public wxThread
{
	volatile bool is_shutdown;
	SharedMemory  shm;

public:
	IPC() : is_shutdown(false) {};

	virtual ExitCode Entry();

	bool Server() const { return shm.Server(); };

	void Shutdown();

	void Send(const wxString& value);
};









/*
 *  expemplary code for app.cpp, specifically CodeBlocksApp::ParseCmdLine  >>>>>>>>>>>>>>>
 *
 *
IPC *ipc = new IPC; // don't delete

if(ipc->Server())
{
	ipc->Run();
}
else
{
	// parser is the wxCmdLineParser
	wxString item;
	wxString buf;

	static const unsigned int max_size = ipc_buf_size / sizeof(wxChar);

	buf.Alloc(4096);

	int count = parser.GetParamCount();

	for (int i = 0; i < count; ++i)
	{
		item = parser.GetParam(i);
		item.append(_T('\n'));

		if(buf.length() + item.length() + 1) >= max_size)
		{
			buf.append(_T('\0'));
			ipc->Send(buf);
			buf.Empty();
		}
		buf.append(item);
	}

	if(buf.length())
	{
		buf.append(_T('\0'));
		ipc->Send(buf);
	}
}
 *
 *
 *  <<<<<<<<<<<<<<< expemplary code
 */




#endif
