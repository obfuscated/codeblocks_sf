#include "sdk_precomp.h"
#include "cbthreadpool.h"
#include "sdk_events.h"
#include "manager.h"
#include "messagemanager.h"
#include <wx/log.h>

#include <wx/listimpl.cpp>
WX_DEFINE_LIST(cbTaskList);

/// Base thread class
class PrivateThread : public wxThread
{
	public:
        enum State
        {
            Idle,
            Busy
        };
		PrivateThread(cbThreadPool* pool)
        : wxThread(wxTHREAD_JOINABLE),
          m_pPool(pool),
        m_Abort(false)
        {
        }
		~PrivateThread(){}

		void Abort(bool abort = true){ m_Abort = abort; }

		virtual ExitCode Entry()
        {
            // continuous loop, until we abort
            while (1)
            {
                if (TestDestroy())
                    break;

                // wait for signal from pool
                m_pPool->m_Semaphore.Wait();

                // should we abort?
                if (m_Abort)
                    break;

                // this is our main iteration:
                // if we have a task assigned, launch it
                // else wait again for signal...
                bool doneWork = false;
                cbTaskElement elem;
                m_pPool->GetNextElement(elem);
                if (elem.task)
                {
                    // increment the "busy" counter
                    m_pPool->m_CounterCriticalSection.Enter();
                    ++m_pPool->m_Counter;
                    m_pPool->m_CounterCriticalSection.Leave();

                    elem.task->Execute();
                    doneWork = true;

                    // decrement the "busy" counter
                    m_pPool->m_CounterCriticalSection.Enter();
                    --m_pPool->m_Counter;
                    m_pPool->m_CounterCriticalSection.Leave();
                }
                if (elem.autoDelete)
                    delete elem.task;

                if (doneWork)
                {
                    // tell the pool we 're done
                    m_pPool->OnThreadTaskDone(this);
                }
            }
            return 0;
        }

        cbThreadPool* m_pPool;
		bool m_Abort;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

cbThreadPool::cbThreadPool(wxEvtHandler* owner, int id, int concurrentThreads)
    : m_pOwner(owner),
    m_ID(id),
    m_Done(true),
    m_Batching(false),
    m_Counter(0)
{
    m_Threads.Clear();
    SetConcurrentThreads(concurrentThreads);
}

cbThreadPool::~cbThreadPool()
{
	FreeThreads();
}

void cbThreadPool::SetConcurrentThreads(int concurrentThreads)
{
    // if == -1, means auto i.e. same as number of CPUs
    if (concurrentThreads == -1)
        m_ConcurrentThreads = wxThread::GetCPUCount();
    else
        m_ConcurrentThreads = concurrentThreads;

    // if still == -1, something's wrong; reset to 1
    if (m_ConcurrentThreads == -1)
        m_ConcurrentThreads = 1;

	LOGSTREAM << _T("Concurrent threads for pool set to ") << m_ConcurrentThreads << _T('\n');

    // alloc (or dealloc) based on new thread count
    AllocThreads();
}

// called by PrivateThread when it's done running a task
void cbThreadPool::OnThreadTaskDone(PrivateThread* thread)
{
    m_CriticalSection.Enter();

    // notify the owner that the task has ended
    CodeBlocksEvent evt(cbEVT_THREADTASK_ENDED, m_ID);
    wxPostEvent(m_pOwner, evt);

    if (m_TaskQueue.IsEmpty())
    {
        // check no running threads are busy
        m_CounterCriticalSection.Enter();
        bool reallyDone = m_Counter == 0;
        m_CounterCriticalSection.Leave();

        if (reallyDone)
        {
            m_Done = true;

            // notify the owner that all tasks are done
            CodeBlocksEvent evt(cbEVT_THREADTASK_ALLDONE, m_ID);
            wxPostEvent(m_pOwner, evt);
        }
    }
    m_CriticalSection.Leave();

    // make sure any waiting threads "wake-up"
    m_Semaphore.Post();
}

void cbThreadPool::BatchBegin()
{
    m_Batching = true;
}

void cbThreadPool::BatchEnd()
{
    m_Batching = false;
    // launch the thread (if there's room in the pool)
    m_Semaphore.Post();
}

bool cbThreadPool::AddTask(cbThreadPoolTask* task, bool autoDelete)
{
    // add task to the pool
    cbTaskElement* elem = new cbTaskElement(task, autoDelete);

    m_CriticalSection.Enter();
    m_TaskQueue.Append(elem);
    m_Done = false;
    m_CriticalSection.Leave();

    if (!m_Batching)
    {
        // launch the thread (if there's room in the pool)
        m_Semaphore.Post();
    }

    return true;
}

// called by the threads
// picks the first waiting cbTaskElement and removes it from the queue
void cbThreadPool::GetNextElement(cbTaskElement& element)
{
    m_CriticalSection.Enter();

    cbTaskList::Node* node = m_TaskQueue.GetFirst();
    if (node)
    {
        cbTaskElement* elem = node->GetData();
        if (elem)
            element = *elem;
        m_TaskQueue.DeleteNode(node);
    }

    m_CriticalSection.Leave();
}

void cbThreadPool::AbortAllTasks()
{
    ClearTaskQueue();
	FreeThreads();
	AllocThreads();
}

void cbThreadPool::ClearTaskQueue()
{
    // delete all pending tasks set to autoDelete
    m_CriticalSection.Enter();
    for (cbTaskList::Node* node = m_TaskQueue.GetFirst(); node; node = node->GetNext())
    {
        cbTaskElement* elem = node->GetData();
        if (elem->autoDelete)
            delete elem->task;
    }
    m_TaskQueue.Clear();
    m_CriticalSection.Leave();
}

void cbThreadPool::AllocThreads()
{
    FreeThreads();

    for (int i = 0; i < m_ConcurrentThreads; ++i)
    {
        PrivateThread* thr = new PrivateThread(this);
        thr->Create(); // start the thread; it 'll wait for our signal ;)
        thr->Run(); // start the thread; it 'll wait for our signal ;)
        m_Threads.Add(thr);
    }
}

void cbThreadPool::FreeThreads()
{
    // delete allocated threads
    unsigned int i;
    for (i = 0; i < m_Threads.GetCount(); ++i)
    {
        PrivateThread* thread = m_Threads[i];
        thread->Abort();  // set m_Abort on *every* thread first
    }

    for (i = 0; i < m_Threads.GetCount(); ++i)
        m_Semaphore.Post(); // now it does not matter which thread wakes up

    m_Semaphore.Post();   // let's do one extra, does not harm
    // actually give them CPU time to die, too
#if wxCHECK_VERSION(2,6,0)
    wxMilliSleep(1);
#else
    wxUSleep(1);
#endif

    wxLogNull logNo;
    for (i = 0; i < m_Threads.GetCount(); ++i)
    {
        unsigned int count = 0;

        PrivateThread* thread = m_Threads[i];
        while(thread->IsRunning())
        {
            m_Semaphore.Post();
#if wxCHECK_VERSION(2,6,0)
            wxMilliSleep(1);
#else
            wxUSleep(1);
#endif
            if(++count > 10)
                break;
        }

        if(count > 10)    // a bit brute, but if it did not wake up until now
            thread->Kill(); // then it will never
    }
    m_Threads.Clear();
}
