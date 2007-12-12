#include "ipc.h"
#include "main.h"
#include <wx/tokenzr.h>

const wxString g_failed_shm(_T("Failed creating shared memory initialising IPC (error 0x00000d04)."));
const wxString g_failed_sem(_T("Failed creating semaphore/mutex initialising IPC (error 0x007f0002)."));

void IPC::Send(const wxString& in)
{
	if(in.length() * sizeof(wxChar) > shm.Size())
		cbThrow(_T("Input exceeds shared memory size (error 0x0000cde0)."));

	if(shm.Lock(SharedMemory::writer) == 0)
		{
			// If locking failed here, this means the semaphore (and hence the shared memory, and the server process) was destroyed
			// after we *just* checked that it exists (a few nanoseconds ago). This is a funny race condition
			// which should be really, really rare, but which is of course nevertheless possible.
			// We should consequently turn this process into a server, after seeing that the semaphore died, but this is really awful,
			// so... we're not doing that... for now. The worst thing to happen is that double-clicking a file does not do anything once in a million times.
			//
			// Let's just throw and see how often we see this exception in normal everyday use.
			// If it never happens, then simply ignoring the issue is a perfectly acceptable solution.
			//
			cbThrow(_T("Congrats, you managed to kill process 1 within nanoseconds after launching process 2, which is quite hard to do.\n\nPlease inform the Code::Blocks team of your achievement."));
		}

	memcpy(shm.BasePointer(), in.c_str(), (in.length()+1) * sizeof(wxChar));
	shm.Unlock(SharedMemory::writer);
}


void IPC::Shutdown()
{
	// Other than POSIX, Windows does not signal threads waiting for a semaphore when the semaphore is deleted (at least, MSDN says so),
	// therefore we have to do unlock by hand before deleting, or we may lock up a process for all times.
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

		MainFrame* cbframe = static_cast<MainFrame*>(Manager::Get()->GetAppFrame());
		if(cbframe == nullptr)
			return 0;

		cbframe->OnDropFiles(0,0, wxStringTokenize((const wxChar*) shm.BasePointer(), _T("\n"), wxTOKEN_STRTOK));

		shm.Unlock(SharedMemory::reader);

		if(is_shutdown)
			return 0;
	}
}








#if defined (__WIN32__) /* ------------------------------------------------------------- */


SharedMemory::SharedMemory() : handle(0), semid(0), shared(0), ok(false), server(false)
{
	SetLastError(0); // CreateSemaphore should already do this, but anyway...
	sem[reader] = CreateSemaphore(nullptr, 0, 1, TEXT("CdeBlsemIPCr"));
	sem[writer] = CreateSemaphore(nullptr, 1, 1, TEXT("CdeBlsemIPCw"));

	if(GetLastError() != ERROR_ALREADY_EXISTS)
	{
		server = true;
	}

	handle = CreateFileMapping(INVALID_HANDLE_VALUE, 0, PAGE_READWRITE, 0, ipc_buf_size, TEXT("CdeBlshmIPC"));

	if (handle == 0 || (shared = MapViewOfFile(handle, FILE_MAP_ALL_ACCESS, 0, 0, ipc_buf_size)) == 0)
	{
		LogManager::Get()->Panic(g_failed_shm);
		return;
	}

	ok = true;
}

SharedMemory::~SharedMemory()
{
	UnmapViewOfFile(shared);
	CloseHandle(handle);
	CloseHandle(sem[reader]);
	CloseHandle(sem[writer]);
}

bool SharedMemory::Lock(rw_t rw)
{
	if(rw == reader)
	{
		return WaitForSingleObject(sem[reader], INFINITE) == WAIT_OBJECT_0
		    && WaitForSingleObject(sem[writer], INFINITE) == WAIT_OBJECT_0;
	}
	else  // if(rw == writer)
	{
		return WaitForSingleObject(sem[writer], INFINITE) == WAIT_OBJECT_0;
	}

	return false;
}

void SharedMemory::Unlock(rw_t rw)
{
	if(rw == reader)
	{
		ReleaseSemaphore(sem[writer], 1, nullptr);
	}
	else  // if(rw == writer)
	{
		ReleaseSemaphore(sem[reader], 1, nullptr);
		ReleaseSemaphore(sem[writer], 1, nullptr);
		Sleep(0);
	}
}


#else                   /* ------------------------------------------------------------- */


SharedMemory::SharedMemory() : handle(0), semid(0), shared(0), ok(false), server(false)
{
	char file[256];
	key_t key;

	/* Shared memory and semaphore functions expect unique IDs created with ftok().
	 * Unluckily, this is how POSIX works... we need a unique yet reproducable filename,
	 * and the file must exist, too.
	 * We'll use the executable file for this, if we can figure it out via /proc.
	 * Unluckily, again, this is nowhere near standardised or even guaranteed, so
	 * we'll have to create a file in /tmp if everything fails...
	 */
	if(readlink("/proc/self/exe", file, sizeof(file)) < 0)       /* Linux style */
	{
		if(readlink("/proc/self/file", file, sizeof(file)) < 0)  /* failed, try BSD style */
		{
			strcpy(file, "/tmp/fuckyou");                        /* failed again, use some bullshit */
			close(open(file, O_CREAT, O_RDONLY|O_WRONLY));
		}
	}

	key = ftok(file, 'a');
	semid  = semget(key, 2, IPC_CREAT | 0666);

	if(semid == -1)
	{
		LogManager::Get()->Panic(g_failed_sem);
		return;
	}

	key = ftok(file, 'b');
	handle = shmget(key, ipc_buf_size, 0666 | IPC_CREAT | IPC_EXCL);

	if(handle == -1)		/* failed, because...                 */
	{
		if(errno == EEXIST) /* EEXIST ---> server already running */
		{
			handle = shmget(key, ipc_buf_size, 0666);
			if(handle == -1)
			{
				LogManager::Get()->Panic(g_failed_shm);
				return;
			}
			ok = true;
			server = false;
		}
		else				/* ...any other error ---> bad        */
		{
			LogManager::Get()->Panic(g_failed_shm);
			return;
		}
	}
	else
	{
		ok = true;
		server = true;

		unsigned short int v[2] = {0, 1};
		semctl(semid, 0, SETALL, v);
	}

	shared = shmat(handle, nullptr, 0);
	ok = (shared != (void*) -1) ? ok : false;
}


SharedMemory::~SharedMemory()
{
	shmdt(shared);
	if(server)
	{
		shmctl(handle, IPC_RMID, 0);
		semctl(semid, 0, IPC_RMID );    /* this will wake up the thread blocking in semop() */
	}
}

bool SharedMemory::Lock(rw_t rw)
{
	if(rw == reader)
	{
		sembuf op[2];

		op[0].sem_num = reader;
        op[0].sem_op  = -1;
        op[0].sem_flg = 0;

		op[1].sem_num = writer;
        op[1].sem_op  = -1;
        op[1].sem_flg = 0;

		return semop(semid, op, 2) == 0;    /* if semaphore is deleted, EIDRM or EINVAL will be returned */
	}

	if(rw == writer)
	{
		sembuf op[1];
		op[0].sem_num = writer;
        op[0].sem_op  = -1;
        op[0].sem_flg = 0;

		return semop(semid, op, 1) == 0;
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

		semop(semid, op, 2);
	}

	if(rw == reader)
	{
		sembuf op[1];
		op[0].sem_num = writer;
        op[0].sem_op  = 1;
        op[0].sem_flg = 0;

		semop(semid, op, 1);
	}
}

#endif                  /* ------------------------------------------------------------- */






