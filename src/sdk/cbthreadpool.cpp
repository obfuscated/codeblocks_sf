/*
* This file is part of Code::Blocks Studio, an open-source cross-platform IDE
* Copyright (C) 2003  Yiannis An. Mandravellos
*
* This program is distributed under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or (at your option) any later version.
*
* $Revision$
* $Id$
* $HeadURL$
*/

#include "sdk_precomp.h"

#ifndef CB_PRECOMP
    #include "sdk_events.h"
    #include "manager.h"
    #include "messagemanager.h"
    #include <wx/log.h>
#endif

#include "cbthreadpool.h"


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
          m_pTask(0),
        m_Abort(false)
        {
//            Manager::Get()->GetMessageManager()->DebugLog(_T("Thread %p created"), this);
        }
		~PrivateThread()
		{
//            Manager::Get()->GetMessageManager()->DebugLog(_T("Thread %p destroyed"), this);
		}

		bool Aborted()
		{
		    if(m_Abort)
                return true;
		    if(TestDestroy()) // Thread::Delete has been called.
		    {
                Abort(); // Update the task's status.
                return true;
		    }
		    return false;
		}

		void Abort()
		{
		    // m_pTask is accessed by more than one thread, hence the Critical Section
		    m_pPool->m_CounterCriticalSection.Enter();
		    if(m_pTask)
                m_pTask->Abort();
		    m_Abort = true;
		    m_pPool->m_CounterCriticalSection.Leave();
        }

		virtual ExitCode Entry()
        {
            // continuous loop, until we abort
            while(!Aborted())
            {
                m_pPool->m_CounterCriticalSection.Enter();
                m_pTask = 0;
                m_pPool->m_CounterCriticalSection.Leave();

                // should we abort?
                if (Aborted())
                    break;

                // wait for signal from pool
                m_pPool->m_Semaphore.Wait();

                // should we abort?
                if (Aborted())
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
                    m_pTask = elem.task;
                    m_pPool->m_CounterCriticalSection.Leave();

                    if (!Aborted())
                        m_pTask->Execute();
                    doneWork = true;

                    // decrement the "busy" counter
                    m_pPool->m_CounterCriticalSection.Enter();
                    m_pTask = 0;
                    --m_pPool->m_Counter;
                    m_pPool->m_CounterCriticalSection.Leave();
                }
                if (elem.autoDelete)
                    delete elem.task;

                // tell the pool we 're done
                if (doneWork)
                    m_pPool->OnThreadTaskDone(this);
            }
            return 0;
        }

        cbThreadPool* m_pPool;
        cbThreadPoolTask* m_pTask;
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
    m_Counter(0),
    m_Aborting(false)
{
    m_Threads.Clear();
    SetConcurrentThreads(concurrentThreads);
}

cbThreadPool::~cbThreadPool()
{
	m_Aborting = true;
    ClearTaskQueue();
	FreeThreads();
}

void cbThreadPool::SetConcurrentThreads(int concurrentThreads)
{
    if (concurrentThreads <= 0 || concurrentThreads > 16)
        concurrentThreads = -1;
    if (concurrentThreads == m_ConcurrentThreads && concurrentThreads > 0)
    {
        m_ConcurrentThreadsSchedule = 0;
        return;
    }

    // if == -1, means auto i.e. same as number of CPUs
    if (concurrentThreads == -1)
        m_ConcurrentThreadsSchedule = wxThread::GetCPUCount();
    else
        m_ConcurrentThreadsSchedule = concurrentThreads;

    // if still == -1, something's wrong; reset to 1
    if (m_ConcurrentThreadsSchedule == -1)
        m_ConcurrentThreadsSchedule = 1;

    if (Done())
    {
        m_ConcurrentThreads = m_ConcurrentThreadsSchedule;
        m_ConcurrentThreadsSchedule = 0; // no schedule

        Manager::Get()->GetMessageManager()->DebugLog(_T("Concurrent threads for pool set to %d"), m_ConcurrentThreads);

        // alloc (or dealloc) based on new thread count
        AllocThreads();
    }
    // else it will be called when all threads are done
}

// called by PrivateThread when it's done running a task
void cbThreadPool::OnThreadTaskDone(PrivateThread* thread)
{
    m_CriticalSection.Enter();

    // notify the owner that the task has ended
    CodeBlocksEvent evt(cbEVT_THREADTASK_ENDED, m_ID);
    wxPostEvent(m_pOwner, evt);

//    Manager::Get()->GetMessageManager()->DebugLog(_T("Thread %p done"), thread);
    if (m_TaskQueue.IsEmpty())
    {
        // check no running threads are busy
        m_CounterCriticalSection.Enter();
        bool reallyDone = m_Counter == 0;
        m_CounterCriticalSection.Leave();
//        Manager::Get()->GetMessageManager()->DebugLog(_T("Live threads %d"), m_Counter);

        if (reallyDone)
        {
            m_Done = true;

            // notify the owner that all tasks are done
            CodeBlocksEvent evt(cbEVT_THREADTASK_ALLDONE, m_ID);
            wxPostEvent(m_pOwner, evt);

            // change concurrent threads count now that we 're done
            if (m_ConcurrentThreadsSchedule != 0)
                SetConcurrentThreads(m_ConcurrentThreadsSchedule);
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
//    RunThreads();
}

void cbThreadPool::RunThreads()
{
    m_CriticalSection.Enter();
    size_t i;
    for (i = 0; i < m_Threads.GetCount(); ++i)
    {
        PrivateThread* thread = m_Threads[i];
        if(thread && !m_Aborting && !thread->IsRunning() && thread!=wxThread::This())
            thread->Run();
    }
    m_CriticalSection.Leave();
}

bool cbThreadPool::AddTask(cbThreadPoolTask* task, bool autoDelete)
{
    if(m_Aborting)
    {
        if(autoDelete)
            delete task;
        return false;
    }
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
//        RunThreads();
    }

    return true;
}

// called by the threads
// picks the first waiting cbTaskElement and removes it from the queue
void cbThreadPool::GetNextElement(cbTaskElement& element)
{
    m_CriticalSection.Enter();
    element.task = 0;
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
    m_Aborting = true;
    ClearTaskQueue();
	FreeThreads();
	AllocThreads();
	m_Aborting = false;
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
        thr->Create(); // Create the thread
        m_Threads.Add(thr);
        thr->Run();
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

    // actually give them CPU time to die, too
#if wxCHECK_VERSION(2,6,0)
    wxMilliSleep(21); // Wait 20 milliseconds so the threads will wake up
#else
    wxUsleep(21);
#endif

    wxLogNull logNo;
    for (i = 0; i < m_Threads.GetCount(); ++i)
    {
        unsigned int count = 0;

        PrivateThread* thread = m_Threads[i];
        if(thread == wxThread::This())
            continue;
        count = 0;
        while(thread->IsRunning())
        {
            thread->Abort();
#if wxCHECK_VERSION(2,6,0)
            wxMilliSleep(1);
#else
            wxUsleep(1);
#endif
            if(++count > 10)
            {
                if(thread->IsRunning())
                    thread->Kill();
                break;
            }
        }
    }
    WX_CLEAR_ARRAY(m_Threads);
    m_Threads.Clear();
}
