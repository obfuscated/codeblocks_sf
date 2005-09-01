#ifndef CBTHREADPOOL_H
#define CBTHREADPOOL_H

#include <wx/event.h>
#include <wx/thread.h>
#include <wx/list.h>
#include <wx/dynarray.h>

#include "settings.h"

/// Abstract base class for workers assigned to the thread pool object
/// Derive a class from this and perform your thing in Execute()
/// Treat it like it's a wxThread...
class DLLIMPORT cbThreadPoolTask
{
	public:
        cbThreadPoolTask() : m_Abort(false) {}
        virtual ~cbThreadPoolTask(){}
		virtual int Execute() = 0;
		virtual void Abort(){ m_Abort = true; }
    protected:
        virtual bool TestDestroy(){ return m_Abort; }
        bool m_Abort;
};

/// Internal-usage struct
struct cbTaskElement
{
    cbTaskElement(cbThreadPoolTask* _task = 0, bool _autoDelete = false) : task(_task), autoDelete(_autoDelete) {}
    cbTaskElement(const cbTaskElement& rhs) : task(rhs.task), autoDelete(rhs.autoDelete) {}
    cbThreadPoolTask* task;
    bool autoDelete;
};
WX_DECLARE_LIST(cbTaskElement, cbTaskList);

// forward decl
class PrivateThread;

/// Threads array
WX_DEFINE_ARRAY(PrivateThread*, cbThreadsArray);

/// The main thread pool object.
/// If you set concurrent threads to -1, it will use the number of CPUs present ;)
class DLLIMPORT cbThreadPool
{
    public:
        cbThreadPool(wxEvtHandler* owner, int id = -1, int concurrentThreads = -1);
        virtual ~cbThreadPool();

        virtual void BatchBegin(); ///< When adding a task, it will not start until calling BatchEnd()
        virtual void BatchEnd();
        virtual bool AddTask(cbThreadPoolTask* task, bool autoDelete = true);
        virtual void AbortAllTasks();
        virtual bool Done(){ return m_Done; }
    protected:
        friend class PrivateThread;
        virtual void GetNextElement(cbTaskElement& element);
        virtual void SetConcurrentThreads(int concurrentThreads);
        virtual void ClearTaskQueue();
        virtual void AllocThreads();
        virtual void FreeThreads();
        virtual void OnThreadTaskDone(PrivateThread* thread);

        wxEvtHandler* m_pOwner;
        int m_ID;
        cbTaskList m_TaskQueue;
        cbThreadsArray m_Threads; // task threads
        int m_ConcurrentThreads;
        int m_MaxThreads;
        bool m_Done;
        bool m_Batching;
};

#endif // CBTHREADPOOL_H
