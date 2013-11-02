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
  if (concurrentThreads <= 0)
  {
    concurrentThreads = wxThread::GetCPUCount();
    if (concurrentThreads == -1)
      m_concurrentThreads = 1;
  }

  if (concurrentThreads == m_concurrentThreads)
  {
    m_concurrentThreadsSchedule = 0;
    return;
  }

  wxMutexLocker lock(m_Mutex);
  _SetConcurrentThreads(concurrentThreads);
}

void cbThreadPool::_SetConcurrentThreads(int concurrentThreads)
{
  if (!m_workingThreads)
  {
    std::for_each(m_threads.begin(), m_threads.end(), std::mem_fun(&cbWorkerThread::Abort));
    Broadcast();
    m_threads.clear();

    // set a new Semaphore for the new threads
    m_semaphore = CountedPtr<wxSemaphore>(new wxSemaphore(0, concurrentThreads));

    m_concurrentThreads = concurrentThreads;
    m_concurrentThreadsSchedule = 0;

    for (std::size_t i = 0; i < static_cast<std::size_t>(m_concurrentThreads); ++i)
    {
      m_threads.push_back(new cbWorkerThread(this, m_semaphore));
      m_threads.back()->Create(m_stackSize);
      m_threads.back()->Run();
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

void cbThreadPool::WorkingThread()
{
  wxMutexLocker lock(m_Mutex);
  ++m_workingThreads;
}

bool cbThreadPool::WaitingThread()
{
  wxMutexLocker lock(m_Mutex);
  --m_workingThreads;

  if (m_workingThreads <= 0 && m_tasksQueue.empty())
  {
    // Sends cbEVT_THREADTASK_ALLDONE message only the real task is all done
    if (m_taskAdded)
    {
      // notify the owner that all tasks are done
      CodeBlocksEvent evt = CodeBlocksEvent(cbEVT_THREADTASK_ALLDONE, m_ID);
      wxPostEvent(m_pOwner, evt);
    }

    // The last active thread is now waiting and there's a pending new number of threads to assign...
    if (m_concurrentThreadsSchedule)
    {
      _SetConcurrentThreads(m_concurrentThreadsSchedule);

      return false; // the thread must abort
    }
  }
  else
    m_semaphore->Post();

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

  while (!Aborted())
  {
    if (workingThread)
    {
      workingThread = false;

      // If a call to WaitingThread returns false, we must abort
      if (!m_pPool->WaitingThread())
        break;

      m_semaphore->Wait(); // nothing to do... so just wait
    }

    if (Aborted())
      break;

    if (!workingThread)
    {
      m_pPool->WorkingThread(); // time to work!
      workingThread = true;
    }

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
      m_pTask->Execute();

      {
        wxMutexLocker lock(m_taskMutex);
        m_pTask = nullptr;
        element.Delete();
      }

      m_pPool->TaskDone(this);
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
