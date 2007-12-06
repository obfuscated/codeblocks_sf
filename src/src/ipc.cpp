#include "ipc.h"
#include "main.h"
#include <wx/tokenzr.h>

#ifndef __WIN32__
	#include <errno.h>
	#include <sys/types.h>
	#include <sys/ipc.h>
	#include <sys/sem.h>
#endif

void IPC::Send(const wxString& in)
{
	if(in.length() * sizeof(wxChar) > shm.Size())
		cbThrow(_T("Input exceeds shared memory size."));

	shm.Lock(SharedMemory::writer);
		memcpy(shm.BasePointer(), in.c_str(), (in.length()+1) * sizeof(wxChar));
	shm.Unlock(SharedMemory::writer);
}


void IPC::Shutdown()
{
	// Other than POSIX, Windows does not signal threads waiting for a semaphore when it is deleted,
	// therefore we have to do unlock by hand before deleting
	// IMPORTANT: This must be called from Manager::Shutdown() or from any other appropriate place
	is_shutdown = true;
	shm.Unlock(SharedMemory::writer);
};



wxThread::ExitCode IPC::Entry() /* this is the receiving end */
{
	for(;;)
	{
		if(shm.Lock(SharedMemory::reader) == 0 || is_shutdown)
			return 0;

		MainFrame* cbframe = static_cast<MainFrame*>(Manager::Get()->GetAppWindow());
		cbframe->OnDropFiles(0,0, wxStringTokenize((const wxChar*) shm.BasePointer(), _T("\n"), wxTOKEN_STRTOK));

		shm.Unlock(SharedMemory::reader);

		if(is_shutdown)
			return 0;
	}
}








#if defined (__WIN32__) /* ------------------------------------------------------------- */


SharedMemory::SharedMemory() : handle(0), sem(0), sem_w(0), shared(0), ok(false), server(false)
{
	sem_w = CreateMutex(NULL, TRUE, TEXT("CodeBmutexsIPCMr"));
	if(GetLastError() == ERROR_SUCCESS)
	{
		server = true;
	}

	sem = CreateSemaphore(0, 0, 1, TEXT("CodeBmutexsIPCw"));

	handle = CreateFileMapping(INVALID_HANDLE_VALUE, 0, PAGE_READWRITE, 0, ipc_buf_size, TEXT("CodeBmutexsIPC"));

	if (handle == 0 || (shared = MapViewOfFile(handle, FILE_MAP_ALL_ACCESS, 0, 0, ipc_buf_size)) == 0)
	{
		fputs("failed creating shared memory", stderr);
		return;
	}

	ok = true;
}

SharedMemory::~SharedMemory()
{
	UnmapViewOfFile(shared);
	CloseHandle(handle);
	CloseHandle(sem_w);
	CloseHandle(sem);
}

bool SharedMemory::Lock(rw_t rw)
{
	if(rw == reader)
	{
		return WaitForSingleObject(sem, INFINITE) == WAIT_OBJECT_0
		    && WaitForSingleObject(sem_w, INFINITE) == WAIT_OBJECT_0;
	}

	if(rw == writer)
	{
		return WaitForSingleObject(sem_w, INFINITE) == WAIT_OBJECT_0;
	}

	return false;
}

void SharedMemory::Unlock(rw_t rw)
{
	if(rw == reader)
	{
		ReleaseMutex(sem_w);
	}

	if(rw == writer)
	{
		ReleaseSemaphore(sem, 1, 0);
		ReleaseMutex(sem_w);
		Sleep(0);
	}
}


#else                   /* ------------------------------------------------------------- */


SharedMemory::SharedMemory() : handle(0), sem(0), shared(0), ok(false), server(false)
{
	char file[256];
	key_t key;

	/* we attempt to use executable for ftok() for an application-unique ID */
	if(readlink("/proc/self/exe", file, sizeof(file)) < 0)       /* Linux   */
	{
		if(readlink("/proc/self/file", file, sizeof(file)) < 0)  /* BSD     */
		{
			strcpy(file, "/tmp/fuckyou");                        /* no proc */
			close(open(file, O_CREAT, O_RDONLY|O_WRONLY));
		}
	}

	key = ftok(file, 'a');
	sem  = semget(key, 2, IPC_CREAT | 0666);

	if(sem == -1)
	{
		fputs("failed creating semaphore", stderr);
		return;
	}

	key = ftok(file, 'b');
	handle = shmget(key, ipc_buf_size, 0666 | IPC_CREAT | IPC_EXCL);

	if(handle == -1)
	{
		if(errno == EEXIST) /* IPC_EXCL ---> server already running */
		{
			handle = shmget(key, ipc_buf_size, 0644 | IPC_CREAT);
			if(handle == -1)
			{
				fputs("failed creating shared memory", stderr);
				return;
			}
			ok = true;
			server = false;
		}
		else
		{
			fputs("failed creating shared memory", stderr);
			return;
		}
	}
	else
	{
		ok = true;
		server = true;

		unsigned short int v[2] = {0, 1};
		semctl(sem, 0, SETALL, v);
	}

	shared = shmat(handle, 0, 0);
	ok = (shared > 0) ? ok : false;
}


SharedMemory::~SharedMemory()
{
	shmdt(shared);
	if(server)
	{
		shmctl(handle, IPC_RMID, 0);
		semctl(sem, 0, IPC_RMID );    /* this will wake up the thread blocking in semop() */
	}
}

bool SharedMemory::Lock(rw_t rw)
{
	if(rw == reader)
	{
		sembuf op[1];

		op[0].sem_num = reader;
        op[0].sem_op  = -1;
        op[0].sem_flg = 0;

		op[1].sem_num = writer;
        op[1].sem_op  = -1;
        op[1].sem_flg = 0;

		return semop(sem, op, 2) == 0;    /* if semaphore is deleted, EIDRM or EINVAL will be returned */
	}

	if(rw == writer)
	{
		sembuf op[1];
		op[0].sem_num = writer;
        op[0].sem_op  = -1;
        op[0].sem_flg = 0;

		return semop(sem, op, 1) == 0;
	}

	return false;
}

void SharedMemory::Unlock(rw_t rw)
{
	if(rw == writer)
	{
		sembuf op[2];
		op[0].sem_num = reader;
        op[0].sem_op  = 1;
        op[0].sem_flg = 0;

		op[1].sem_num = writer;
        op[1].sem_op  = 1;
        op[1].sem_flg = 0;

		semop(sem, op, 2);
	}

	if(rw == reader)
	{
		sembuf op[1];
		op[0].sem_num = writer;
        op[0].sem_op  = 1;
        op[0].sem_flg = 0;

		semop(sem, op, 1);
	}
}

#endif                  /* ------------------------------------------------------------- */






