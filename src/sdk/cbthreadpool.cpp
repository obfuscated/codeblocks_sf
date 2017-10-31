/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "sdk_precomp.h"

#ifndef CB_PRECOMP
  #include "sdk_events.h"
  #include "manager.h"
  #include "logmanager.h"
#endif

#include "cbthreadpool.h"
#include <algorithm>
#include <functional>

cbThreadPool::~cbThreadPool()
{
  wxMutexLocker lock(m_Mutex);

  std::for_each(m_threads.begin(), m_threads.end(), std::mem_fun(&cbWorkerThread::Abort));
  Broadcast(); // make every waiting thread realise it's time to die

  std::for_each(m_tasksQueue.begin(), m_tasksQueue.end(), std::mem_fun_ref(&cbThreadedTaskElement::Delete));
}

void cbThreadPool::SetConcurrentThreads(int concurrentThreads)
{
  // m_concurrentThreads is set here, it should always be a positive integer
  if (concurrentThreads <= 0)
  {
    concurrentThreads = wxThread::GetCPUCount(); // GetCPUCount will return -1 if it failed
    if (concurrentThreads == -1)
      m_concurrentThreads = 1;                   // as a fallback, we set the value to 1
  }

  if (concurrentThreads == m_concurrentThreads)
  {
    m_concurrentThreadsSchedule = 0;
    return;
  }

  wxMutexLocker lock(m_Mutex);
  _SetConcurrentThreads(concurrentThreads);
}
// this function is already wrappered by a mutex
void cbThreadPool::_SetConcurrentThreads(int concurrentThreads)
{
  if (!m_workingThreads)// if pool is not running (no thread is running)
  {
    std::for_each(m_threads.begin(), m_threads.end(), std::mem_fun(&cbWorkerThread::Abort));
    Broadcast();
    m_threads.clear();

    // set a new Semaphore for the new threads, note the max value is the concurrentThreads
    m_semaphore = CountedPtr<wxSemaphore>(new wxSemaphore(0, concurrentThreads));

    m_concurrentThreads = concurrentThreads;
    m_concurrentThreadsSchedule = 0;

    for (std::size_t i = 0; i < static_cast<std::size_t>(m_concurrentThreads); ++i)
    {
      m_threads.push_back(new cbWorkerThread(this, m_semaphore));
      m_threads.back()->Create(m_stackSize);
      m_threads.back()->Run(); // this will run cbWorkerThread::Entry()
    }

//    Manager::Get()->GetLogManager()->DebugLog(_T("Concurrent threads for pool set to %d"), m_concurrentThreads);
  }
  else
    m_concurrentThreadsSchedule = concurrentThreads;
}

void cbThreadPool::AddTask(cbThreadedTask *task, bool autodelete)
{
  if (!task)
    return;

  wxMutexLocker lock(m_Mutex);

  m_tasksQueue.push_back(cbThreadedTaskElement(task, autodelete));
  m_taskAdded = true;

  // we are in batch mode, so no need to awake the idle thread
  // m_workingThreads < m_concurrentThreads means there are some threads in idle mode (no task assigned)
  if (!m_batching && m_workingThreads < m_concurrentThreads)
    AwakeNeeded();
}

void cbThreadPool::AbortAllTasks()
{
  wxMutexLocker lock(m_Mutex);

  std::for_each(m_threads.begin(), m_threads.end(), std::mem_fun(&cbWorkerThread::AbortTask));
  std::for_each(m_tasksQueue.begin(), m_tasksQueue.end(), std::mem_fun_ref(&cbThreadedTaskElement::Delete));
  m_tasksQueue.clear();
}

void cbThreadPool::BatchEnd()
{
  wxMutexLocker lock(m_Mutex);
  m_batching = false;

  AwakeNeeded();
}

cbThreadPool::cbThreadedTaskElement cbThreadPool::GetNextTask()
{
  wxMutexLocker lock(m_Mutex);

  if (m_tasksQueue.empty())
    return cbThreadedTaskElement();

  cbThreadedTaskElement element = m_tasksQueue.front();
  m_tasksQueue.pop_front();

  return element;
}
// a thread is leaving from idle mode, and run a task
void cbThreadPool::WorkingThread()
{
  wxMutexLocker lock(m_Mutex);
  ++m_workingThreads;
}
// this thread is finishing the task, and is going to be idle.
// if there is no task left, and the total threads is running is 0, then we have all task done
// otherwise, just put me to the idle mode by m_semaphore->Post()
bool cbThreadPool::WaitingThread()
{
  wxMutexLocker lock(m_Mutex);
  --m_workingThreads;

  if (m_workingThreads <= 0 && m_tasksQueue.empty())
  {
    if (m_taskAdded)
    {
      // notify the owner that all tasks are done
      CodeBlocksEvent evt = CodeBlocksEvent(cbEVT_THREADTASK_ALLDONE, m_ID);
      wxPostEvent(m_pOwner, evt);
      m_taskAdded = false;
    }

    // The last active thread is now waiting and there's a pending new number of threads to assign...
    if (m_concurrentThreadsSchedule)
    {
      _SetConcurrentThreads(m_concurrentThreadsSchedule);

      return false; // the thread must abort
    }
  }
  else
    m_semaphore->Post(); // return the resource back to pool

  return true;
}

void cbThreadPool::TaskDone(cb_unused cbWorkerThread* thread)
{
  // notify the owner that the task has ended
  CodeBlocksEvent evt = CodeBlocksEvent(cbEVT_THREADTASK_ENDED, m_ID);
  wxPostEvent(m_pOwner, evt);
}

/* *********************************************** */
/* ******** cbWorkerThread IMPLEMENTATION ******** */
/* *********************************************** */

cbThreadPool::cbWorkerThread::cbWorkerThread(cbThreadPool *pool, CountedPtr<wxSemaphore> &semaphore)
: m_abort(false),
  m_pPool(pool),
  m_semaphore(semaphore),
  m_pTask(nullptr)
{
  // empty
}

wxThread::ExitCode cbThreadPool::cbWorkerThread::Entry()
{
  bool workingThread = false; // keeps the state of the thread so it knows better what to do
  // a working thread  = true means it is running a task, false means it is waiting a task

  while (!Aborted())
  {
    if (workingThread)
    {
      workingThread = false;

      // normally if pool own some resource, it will release one
      // If a call to WaitingThread returns false, we must abort
      if (!m_pPool->WaitingThread())
        break;
      // if there are still some tasks in the queue, WaitingThread() function will Post the
      // semaphore, and we don't delay much here for the Wait() function.
      // if there are no tasks to do, then WaitingThread() does not Post the semaphore, then
      // we are going to Idle mode now... Until some one release the resource

      m_semaphore->Wait(); // nothing to do... so just wait until it get the resource
    }

    if (Aborted())
      break;

    if (!workingThread)
    {
      m_pPool->WorkingThread(); // time to work! thread status from idle to running
      workingThread = true;
    }

    // fetch a task from the task queue
    cbThreadPool::cbThreadedTaskElement element = m_pPool->GetNextTask();

    {
      wxMutexLocker lock(m_taskMutex);
      m_pTask = element.task;
    }

    // are we done with all tasks?
    if (!m_pTask)
      continue;

    if (!Aborted())
    {
      m_pTask->Execute(); // run task's job here

      {
        wxMutexLocker lock(m_taskMutex);
        m_pTask = nullptr;
        element.Delete();
      }

      m_pPool->TaskDone(this); // send an notification event that one task is done.
    }
  }

  if (workingThread)
    m_pPool->WaitingThread();

  return nullptr;
}

void cbThreadPool::cbWorkerThread::Abort()
{
  m_abort = true;
  AbortTask();
}

bool cbThreadPool::cbWorkerThread::Aborted() const
{
  return m_abort;
}

void cbThreadPool::cbWorkerThread::AbortTask()
{
  wxMutexLocker lock(m_taskMutex);

  if (m_pTask)
    m_pTask->Abort();
}
