#ifndef IPC_H
#define IPC_H


#include "../include/sdk_precomp.h"

#include <wx/wx.h>

#ifndef __WIN32__
	#include <sys/types.h>
	#include <sys/ipc.h>
	#include <sys/shm.h>
	#include <fcntl.h>
#endif



static const int ipc_buf_size = 1024*64

typedef TernaryCondTypedef<platform::windows, HANDLE, int> int shm_handle_t;
typedef TernaryCondTypedef<platform::windows, HANDLE, int> int semaphore_t;



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

	bool Lock(rw_t rw);
	void Unlock(rw_t rw);
};





class IPC : public wxThread
{
	volatile bool shutdown;
	SharedMemory  shm;

public:
	IPC() : shutdown(false) {};

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