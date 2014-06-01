/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef CBTHREADPOOL_H
#define CBTHREADPOOL_H

#include <wx/thread.h>
#include <wx/event.h>
#include <vector>
#include <list>

#include "cbthreadedtask.h"
#include "settings.h"
#include "prep.h"

/// A Thread Pool implementation
class DLLIMPORT cbThreadPool
{
  public:
    /** cbThreadPool ctor
      *
      * @param owner Event handler to receive cbEVT_THREADTASK_ENDED and cbEVT_THREADTASK_ALLDONE events
      * @param id Used with the events
      * @param concurrentThreads Number of threads in the pool. -1 means current CPU count
      */
    cbThreadPool(wxEvtHandler *owner, int id = -1, int concurrentThreads = -1, unsigned int stackSize = 0);

    /// cbThreadPool dtor
    ~cbThreadPool();

    /** Changes the number of threads in the pool
      *
      * @param concurrentThreads New number of threads. -1 or 0 means current CPU count
      * @note If tasks are running, it'll delay it until they're all done.
      */
    void SetConcurrentThreads(int concurrentThreads);

    /** Gets the current number of threads in the pool
      *
      * @return Number of threads in the pool
      * @note If a call to SetConcurrentThreads hasn't been applied, it'll return the
      * number of threads that will be set by it when all tasks be done.
      */
    int GetConcurrentThreads() const;

    /** return the pool ID */
    int GetId() const { return m_ID; }

    /** Adds a new task to the pool
      *
      * @param task The task to execute
      * @param autodelete If true, the task will be deleted when it finish or be aborted
      */
    void AddTask(cbThreadedTask *task, bool autodelete = true);

    /** Aborts all running and pending tasks
      *
      * @note Calls cbThreadedTask::Abort for all running tasks and just removes the pending ones.
      */
    void AbortAllTasks();

    /** Tells if the pool has finished its job
      *
      * @return true if it has nothing to do, false if it's executing tasks
      */
    bool Done() const;

    /** Begin a batch process
      *
      * @note EVIL: Call it if you want to add all tasks first and get none executed yet.
      * If you DON'T call it, tasks will be executed as you add them (in fact it's what
      * one would expect).
      */
    void BatchBegin();

    /** End a batch process
      *
      * @note EVIL: Call it when you have finished adding tasks and want them to execute.
      * BEWARE: if you call BatchBegin but DON'T call BatchEnd, the tasks WON'T execute.
      */
    void BatchEnd();

  private:

    /// Josuttis' implementation of CountedPtr
    /* class for counted reference semantics
     * - deletes the object to which it refers when the last CountedPtr
     *   that refers to it is destroyed
     */
    template <typename T>
    class CountedPtr
    {
      private:
        T *ptr;         // pointer to the value
        long *count;    // shared number of owners

      public:
        // initialize pointer with existing pointer
        // - requires that the pointer p is a return value of new
        explicit CountedPtr(T *p = 0);
        // copy pointer (one more owner)
        CountedPtr(const CountedPtr<T> &p) throw();
        // destructor (delete value if this was the last owner)
        ~CountedPtr() throw();
        // assignment (unshare old and share new value)
        CountedPtr<T> &operator = (const CountedPtr<T> &p) throw();
         // access the value to which the pointer refers
        T &operator * () const throw();
        T *operator -> () const throw();

      private:
        void dispose(); //decrease the counter, and if it get 0, destroy both counter and ptr
    };

    /** A Worker Thread class.
      *
      * These are the ones that execute the tasks.
      * You shouldn't worry about it since it's for "private" purposes of the Pool.
      */
    class cbWorkerThread : public wxThread
    {
      public:
        /** cbWorkerThread ctor
          *
          * @param pool Thread Pool this Worker Thread belongs to
          * @param semaphore Used to synchronize the Worker Threads, it is a reference to the CountedPtr
          * object
          */
        cbWorkerThread(cbThreadPool *pool, CountedPtr<wxSemaphore> &semaphore);

        /// Entry point of this thread. The magic happens here.
        ExitCode Entry();

        /// Tell the thread to abort. It will also tell the task to abort (if any)
        void Abort();

        /** Tells whether we should abort or not
          *
          * @return true if we should abort
          */
        bool Aborted() const;

        /// Aborts the running task (if any)
        void AbortTask();

      private:
        bool m_abort;
        cbThreadPool *m_pPool;
        // a counted semaphore shared with all the cbWorkerThread
        CountedPtr<wxSemaphore> m_semaphore;
        cbThreadedTask *m_pTask;
        wxMutex m_taskMutex;// to protect the member variable accessing from multiply threads
    };

    typedef std::vector<cbWorkerThread *> WorkerThreadsArray;

    /// All tasks are added to one of these. It'll also save the autodelete value
    struct cbThreadedTaskElement
    {
      cbThreadedTaskElement(cbThreadedTask *_task = nullptr, bool _autodelete = false)
      : task(_task),
        autodelete(_autodelete)
      {
        // empty
      }

      /// It'll delete the task only if it was set to
      void Delete()
      {
        if (autodelete)
        {
          delete task;
          task = nullptr; // better safe than sorry
        }
      }

      cbThreadedTask *task;
      bool autodelete;
    };

    typedef std::list<cbThreadedTaskElement> TasksQueue;

    wxEvtHandler *m_pOwner; // events notification will send to this guy
    int m_ID;           // id used to fill the ID field of the event
    bool m_batching;    // whether in batch mode of adding tasks

    // current number of concurrent threads, this is the maximum value of the m_workingThreads
    // this variable should always be positive, 0 and -1 is not allowed.
    int m_concurrentThreads;
    unsigned int m_stackSize; // stack size for every threads

    // if we cannot apply the new value of concurrent threads, keep it here, usually the time to
    // apply a scheduled value is when all the tasks is done.
    int m_concurrentThreadsSchedule;

    // the total threads(cbWorkerThread) are stored here, this contains all the threads either is
    // currently running or in idle() mode.
    WorkerThreadsArray m_threads;

    // the pending tasks (cbThreadedTaskElement), usually we have many tasks to run in the pool, but
    // we have limited number of threads to run those tasks, so tasks which don't have the change to
    // run will be put in the queue. Once a thread is finish a task, it will fetch a new task from
    // this task queue.
    TasksQueue m_tasksQueue;

    // true if any task added, reset to false if all the tasks is done
    bool m_taskAdded;

    int m_workingThreads; // how many working threads are running tasks

    mutable wxMutex m_Mutex; // we better be safe, protect the change of member variables

    // used to synchronize the Worker Threads, the counted value is that how many threads are
    // sharing this semaphore. The semaphore's initial value is the thread number we can used to
    // run the tasks.
    CountedPtr<wxSemaphore> m_semaphore;

    void _SetConcurrentThreads(int concurrentThreads); // like SetConcurrentThreads, but non-thread safe

    // awakes all threads, this is used when we are going to abort all the threads, there are two
    // cases we need to call Broadcast(), one is the destructor, the other is the user need to
    // change the concurrent thread numbers, so we abort all the threads, and re-create them again.
    void Broadcast();

    // awakes only a few threads, this usually happens when we add some tasks, and there are some
    // threads which is currently in idle mode, so we can awake these idle threads to run tasks.
    void AwakeNeeded();


  protected:
    friend class cbWorkerThread;

    /** Returns the next task to run
      *
      * @return Next task to run, or a NULL task (set in .task) if none
      */
    cbThreadedTaskElement GetNextTask();

    /// Mechanism for the threads to tell the Pool they're running, a thread is switch from the idle
    /// mode to working mode. This is triggered by semaphore released somewhere
    void WorkingThread();

    /** Mechanism for the threads to tell the Pool they're done and will go to idle, so we can assign
      * another task to this thread.
      *
      * @return true if everything is OK, false if we should abort, this usually happens we need to
      * set a scheduled m_concurrentThreads value.
      */
    bool WaitingThread();

    /** Called by a Worker Thread to inform a single task has finished, this will send a cbEVT_THREADTASK_ENDED event
      *
      * @param thread The Worker Thread
      */
    void TaskDone(cbWorkerThread *thread);
};

/* ************************************************ */
/* **************** INLINE MEMBERS **************** */
/* ************************************************ */

inline cbThreadPool::cbThreadPool(wxEvtHandler *owner, int id, int concurrentThreads, unsigned int stackSize)
: m_pOwner(owner),
  m_ID(id),
  m_batching(false),
  m_concurrentThreads(-1),
  m_stackSize(stackSize),
  m_concurrentThreadsSchedule(0),
  m_taskAdded(false),
  m_workingThreads(0),
  m_semaphore(new wxSemaphore)
{
  // m_concurrentThreads will be set to a positive integer value.
  SetConcurrentThreads(concurrentThreads);
}

inline int cbThreadPool::GetConcurrentThreads() const
{
  wxMutexLocker lock(m_Mutex);
  return m_concurrentThreadsSchedule ? m_concurrentThreadsSchedule : m_concurrentThreads;
}

inline bool cbThreadPool::Done() const
{
  wxMutexLocker lock(m_Mutex);
  return m_workingThreads == 0;
}

inline void cbThreadPool::BatchBegin()
{
  wxMutexLocker lock(m_Mutex);
  m_batching = true;
}

inline void cbThreadPool::Broadcast()
{
  // if m_concurrentThreads == -1, which means the pool is not initialized yet
  if (m_concurrentThreads == -1)
    return;
  // let the idle(pending) worker thread to execute tasks, those worker threads are waiting for semaphore
  for (std::size_t i = 0; i < static_cast<std::size_t>(m_concurrentThreads - m_workingThreads); ++i)
    m_semaphore->Post();
}

inline void cbThreadPool::AwakeNeeded()
{
  // if m_concurrentThreads == -1, which means the pool is not initialized yet
  if (m_concurrentThreads == -1)
    return;

  // the thread number to awake should be less than the idle thread number and the tasks queue's size
  std::size_t awakeThreadNumber = 0;
  awakeThreadNumber = std::min(m_tasksQueue.size(),
                               static_cast<std::size_t>(m_concurrentThreads - m_workingThreads));
  for (std::size_t i = 0; i < awakeThreadNumber; ++i)
    m_semaphore->Post();
}

/* *** Josuttis' CountedPtr *** */

template <typename T>
inline cbThreadPool::CountedPtr<T>::CountedPtr(T *p)
: ptr(p),
  count(new long(1))
{
  // empty
}

template <typename T>
inline cbThreadPool::CountedPtr<T>::CountedPtr(const CountedPtr<T> &p) throw()
: ptr(p.ptr),
  count(p.count)
{
  ++*count;
}

template <typename T>
inline cbThreadPool::CountedPtr<T>::~CountedPtr() throw()
{
  dispose();
}

template <typename T>
inline cbThreadPool::CountedPtr<T> &cbThreadPool::CountedPtr<T>::operator = (const CountedPtr<T> &p) throw()
{
  if (this != &p)
  {
    dispose();
    ptr = p.ptr;
    count = p.count;
    ++*count;
  }

  return *this;
}

template <typename T>
inline T &cbThreadPool::CountedPtr<T>::operator * () const throw()
{
  return *ptr;
}

template <typename T>
inline T *cbThreadPool::CountedPtr<T>::operator -> () const throw()
{
  return ptr;
}

template <typename T>
inline void cbThreadPool::CountedPtr<T>::dispose()
{
  if (--*count == 0)
  {
    delete count;
    delete ptr;
  }
}

#endif  //CBTHREADPOOL_H
