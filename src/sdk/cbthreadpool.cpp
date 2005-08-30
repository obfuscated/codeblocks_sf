#include "cbthreadpool.h"
#include "sdk_events.h"
#include <wx/log.h>

#include <wx/listimpl.cpp>
WX_DEFINE_LIST(cbTaskList);

static int idThreadMsg = wxNewId();
static wxSemaphore s_Semaphore;
static wxCriticalSection s_CriticalSection;

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
        : m_pPool(pool),
        m_Abort(false),
        m_State(Idle)
        {
        }
		~PrivateThread(){}

		void Abort(bool abort = true){ m_Abort = abort; }
		State GetState(){ return m_State; }

		virtual ExitCode Entry()
        {
            // continuous loop, until we abort
            while (1)
            {
                if (TestDestroy())
                    break;

                // wait for signal from pool
                s_Semaphore.Wait();

                // should we abort?
                if (m_Abort)
                    break;

                // this is our main iteration:
                // if we have a task assigned, launch it
                // else wait again for signal...
                m_State = Busy;
                cbTaskElement elem;
                m_pPool->GetNextElement(elem);
                if (elem.task)
                    elem.task->Execute();
                if (elem.autoDelete)
                    delete elem.task;
                m_State = Idle;

                // tell the pool we 're done
                CodeBlocksEvent evt(wxEVT_COMMAND_MENU_SELECTED, idThreadMsg);
                evt.SetClientData(elem.autoDelete ? 0 : elem.task);
                wxPostEvent(m_pPool, evt);
            }
            return 0;
        }

        cbThreadPool* m_pPool;
		bool m_Abort;
		State m_State;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(cbThreadPool, wxEvtHandler)
    EVT_MENU(idThreadMsg, cbThreadPool::OnThreadTaskDone)
END_EVENT_TABLE()

cbThreadPool::cbThreadPool(wxEvtHandler* owner, int id, int concurrentThreads)
    : m_pOwner(owner),
    m_ID(id),
    m_Done(true)
{
    SetConcurrentThreads(concurrentThreads);
}

cbThreadPool::~cbThreadPool()
{
    AbortAllTasks();
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
        concurrentThreads = 1;

    // alloc (or dealloc) based on new thread count
    AllocThreads();
}

void cbThreadPool::OnThreadTaskDone(wxCommandEvent& event)
{
    s_CriticalSection.Enter();

    // notify the owner that the task has ended
    CodeBlocksEvent evt(cbEVT_THREADTASK_ENDED, m_ID);
    evt.SetClientData(event.GetClientData());
    wxPostEvent(m_pOwner, evt);

    if (m_TaskQueue.IsEmpty())
    {
        // check running threads too
        bool reallyDone = true;
        for (int i = 0; i < (int)m_Threads.GetCount(); ++i)
        {
            if (m_Threads[i]->GetState() == PrivateThread::Busy)
            {
                reallyDone = false;
                break;
            }
        }

        if (reallyDone)
        {
            m_Done = true;

            // notify the owner that all tasks are done
            CodeBlocksEvent evt(cbEVT_THREADTASK_ALLDONE, m_ID);
            evt.SetClientData(0);
            wxPostEvent(m_pOwner, evt);
        }
    }
    s_CriticalSection.Leave();
}

bool cbThreadPool::AddTask(cbThreadPoolTask* task, bool autoDelete)
{
    // add task to the pool
    cbTaskElement* elem = new cbTaskElement(task, autoDelete);

    s_CriticalSection.Enter();
    m_TaskQueue.Append(elem);
    m_Done = false;
    s_CriticalSection.Leave();

    // launch the thread (if there's room in the pool)
    s_Semaphore.Post();

    return true;
}

// called by the threads
// picks the first waiting cbTaskElement and removes it from the queue
void cbThreadPool::GetNextElement(cbTaskElement& element)
{
    s_CriticalSection.Enter();

    cbTaskList::Node* node = m_TaskQueue.GetFirst();
    if (node)
    {
        cbTaskElement* elem = node->GetData();
        if (elem)
            element = *elem;
        m_TaskQueue.DeleteNode(node);
    }

    s_CriticalSection.Leave();
}

void cbThreadPool::AbortAllTasks()
{
    ClearTaskQueue();

    for (unsigned int i = 0; i < m_Threads.GetCount(); ++i)
    {
        PrivateThread* thread = m_Threads[i];
        thread->Abort();
    }
    s_Semaphore.Post();
}

void cbThreadPool::ClearTaskQueue()
{
    // delete all pending tasks set to autoDelete
    s_CriticalSection.Enter();
    for (cbTaskList::Node* node = m_TaskQueue.GetFirst(); node; node = node->GetNext())
    {
        cbTaskElement* elem = node->GetData();
        if (elem->autoDelete)
            delete elem->task;
    }
    m_TaskQueue.Clear();
    s_CriticalSection.Leave();
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
    for (unsigned int i = 0; i < m_Threads.GetCount(); ++i)
    {
        PrivateThread* thread = m_Threads[i];
        thread->Abort();
    }
    m_Threads.Clear();
}
