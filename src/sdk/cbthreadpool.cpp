#include "sdk_precomp.h"

#ifndef CB_PRECOMP
 #include "sdk_events.h"
 #include "manager.h"
 #include "messagemanager.h"
 #include <wx/log.h>
#endif

#include "cbthreadpool.h"
#include <algorithm>
#include <functional>

cbThreadPool::~cbThreadPool()
{
  wxMutexLocker lock(m_Mutex);

  std::for_each(m_threads.begin(), m_threads.end(), std::mem_fun(&cbWorkerThread::Abort));
  m_cond.Broadcast(); // make every waiting thread realise it's time to die

  std::for_each(m_tasksQueue.begin(), m_tasksQueue.end(), std::mem_fun_ref(&cbThreadedTaskElement::Delete));
}

void cbThreadPool::SetConcurrentThreads(int concurrentThreads)
{
  if (concurrentThreads <= 0)
  {
    concurrentThreads = wxThread::GetCPUCount();

    if (concurrentThreads == -1)
    {
      m_concurrentThreads = 1;
    }
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
    m_cond.Broadcast();
    m_threads.clear();

    m_concurrentThreads = concurrentThreads;
    m_concurrentThreadsSchedule = 0;

    for (std::size_t i = 0; i < static_cast<std::size_t>(m_concurrentThreads); ++i)
    {
      m_threads.push_back(new cbWorkerThread(this, m_cond, m_condMutex));
      m_threads.back()->Create();
      m_threads.back()->Run();
    }

    Manager::Get()->GetMessageManager()->DebugLog(_T("Concurrent threads for pool set to %d"), m_concurrentThreads);
  }
  else
  {
    m_concurrentThreadsSchedule = concurrentThreads;
  }
}

void cbThreadPool::AddTask(cbThreadedTask *task, bool autodelete)
{
  if (!task)
  {
    return;
  }

  wxMutexLocker lock(m_Mutex);

  m_tasksQueue.push_back(cbThreadedTaskElement(task, autodelete));

  if (!m_batching && m_workingThreads < m_concurrentThreads)
  {
    for (std::size_t i = 0; i < static_cast<std::size_t>(m_concurrentThreads - m_workingThreads) && i < m_tasksQueue.size(); ++i)
    {
      m_cond.Signal();
    }
  }
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

  for (std::size_t i = 0; i < static_cast<std::size_t>(m_concurrentThreads - m_workingThreads) && i < m_tasksQueue.size(); ++i)
  {
    m_cond.Signal();
  }
}

cbThreadPool::cbThreadedTaskElement cbThreadPool::GetNextTask()
{
  wxMutexLocker lock(m_Mutex);

  if (m_tasksQueue.empty())
  {
    return cbThreadedTaskElement();
  }

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

  if (!m_workingThreads)
  {
    // notify the owner that all tasks are done
    CodeBlocksEvent evt = CodeBlocksEvent(cbEVT_THREADTASK_ALLDONE, m_ID);
    wxPostEvent(m_pOwner, evt);

    // The last active thread is now waiting and there's a pending new number of threads to assign...
    if (m_concurrentThreadsSchedule)
    {
      _SetConcurrentThreads(m_concurrentThreadsSchedule);

      return false; // the thread must abort
    }
  }

  return true;
}

void cbThreadPool::TaskDone(cbWorkerThread *thread)
{
  // notify the owner that the task has ended
  CodeBlocksEvent evt = CodeBlocksEvent(cbEVT_THREADTASK_ENDED, m_ID);
  wxPostEvent(m_pOwner, evt);
}

/* *********************************************** */
/* ******** cbWorkerThread IMPLEMENTATION ******** */
/* *********************************************** */

cbThreadPool::cbWorkerThread::cbWorkerThread(cbThreadPool *pool, wxCondition &cond, wxMutex &mutex)
: m_abort(false),
  m_pPool(pool),
  m_cond(cond),
  m_mutex(mutex),
  m_pTask(0)
{
  // empty
}

wxThread::ExitCode cbThreadPool::cbWorkerThread::Entry()
{
  bool must_wait = true; // tricky way to keep the thread working
  bool workingThread = false; // keeps the state of the thread so it knows better what to do

  while (!Aborted())
  {
    if (must_wait)
    {
      wxMutexLocker lock(m_mutex);

      if (workingThread)
      {
        workingThread = false;

        // If a call to WaitingThread returns false, we must abort
        if (!m_pPool->WaitingThread())
        {
          break;
        }
      }

      m_cond.Wait(); // nothing to do... so just wait
    }

    if (Aborted())
    {
      break;
    }

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
    {
      must_wait = true;
      continue;
    }

    if (!Aborted())
    {
      m_pTask->Execute();

      {
        wxMutexLocker lock(m_taskMutex);
        m_pTask = 0;
        element.Delete();
      }

      m_pPool->TaskDone(this);
    }

    must_wait = false;
  }

  if (workingThread)
  {
    m_pPool->WaitingThread();
  }

  return 0;
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
  {
    m_pTask->Abort();
  }
}
