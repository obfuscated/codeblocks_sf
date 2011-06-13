/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef BACKGROUNDTHREAD_H
#define BACKGROUNDTHREAD_H

#include "safedelete.h"

#undef new
#include <deque>
#include <list>
#include <algorithm>

#include <wx/timer.h> // wxMilliSleep
#include "wx/thread.h"
#include "manager.h"
#include "blockallocated.h"

/*
* BackgroundThread is a lightweight single background worker thread implementation for situations in which
* you simply want to do one or several things in another thread, and using a thread pool is overkill.
* Also, several BackgroundThreads can be used in situations where a thread pool is unsuitable by design. For example,
* you can use two BackgroundThreads to asynchronously read a list of files from disk one at at time and download another list
* of files from the internet without hogging either the hard disk or the network layer with more than one concurrent access.
*
* BackgroundThread can be configured to own the job objects (will delete them after running) or not. It can also own
* the semaphore and queue, or use a shared context.
*
* BackgroundThreadPool is a low overhead thread pool implementation around BackgroundThread.
*/


class AbstractJob
{
public:
    AbstractJob(){};
    virtual ~AbstractJob(){};
    virtual void operator()() = 0;
};



class JobQueue : public std::deque<AbstractJob*>
{
    wxCriticalSection c;

public:
    void Push(AbstractJob *j)
    {
        wxCriticalSectionLocker l(c);
        push_back(j);
    };
    AbstractJob* Pop()
    {
        wxCriticalSectionLocker l(c);
        AbstractJob* j = front();
        pop_front();
        return j;
    };
};



class BackgroundThread : public wxThread
{
    JobQueue *queue;
    wxSemaphore *semaphore;
    bool die;
    const bool ownsQueue;
    const bool ownsSemaphore;
    const bool ownsJobs;

public:
    BackgroundThread(JobQueue *q, wxSemaphore *s, const bool owns_jobs = true)
    : queue(q), semaphore(s), die(false), ownsQueue(false), ownsSemaphore(false), ownsJobs(owns_jobs)
    {
        Create();
        Run();
    };

    BackgroundThread(wxSemaphore *s, const bool owns_jobs = true)
    : queue(new JobQueue), semaphore(s), die(false), ownsQueue(true), ownsSemaphore(false), ownsJobs(owns_jobs)
    {
        Create();
        Run();
    };

    BackgroundThread(JobQueue *q, const bool owns_jobs = true)
    : queue(q), semaphore(new wxSemaphore), die(false), ownsQueue(false), ownsSemaphore(true), ownsJobs(owns_jobs)
    {
        Create();
        Run();
    };

    BackgroundThread(const bool owns_jobs = true)
    : queue(new JobQueue), semaphore(new wxSemaphore), die(false), ownsQueue(true), ownsSemaphore(true), ownsJobs(owns_jobs)
    {
        Create();
        Run();
    };

    ~BackgroundThread()
    {
        if(ownsSemaphore)
            ::Delete(semaphore);
        if(ownsQueue)
            ::Delete(queue);
    };


    void Queue(AbstractJob* j)
    {
        queue->Push(j);
        semaphore->Post();
    };

    void Die()
    {
        die = true;
        semaphore->Post();
        wxMilliSleep(0);
    };


    void MarkDying() // Need this for threadpool. Die() alone does not work in shared context (for obvious reason).
    {
        die = true;
    };

    /*
    * This function is inherently unsafe!
    * Also, if used on a thread belonging to a pool, it will not do what you think it does!
    * Do not use this function unless you are sure you really know what you are doing.
    */
    void MurderDeathKill()
    {
        Die();
        wxMilliSleep(0);
        wxMilliSleep(0);

        if(this && IsRunning())
            Kill();
    };

    ExitCode Entry()
    {
        AbstractJob* job;
        for(;;)
        {
            semaphore->Wait();
            if(die)
                break;

            job = queue->Pop();
            if ( job )
                (*job)();

            if(ownsJobs)
                delete job;
        }
        return 0;
    };
};





struct Agony { inline void operator()(BackgroundThread* t){t->MarkDying();}; };
struct Death { inline void operator()(BackgroundThread* t){t->Die();}; };

class BackgroundThreadPool
{
    typedef std::list<BackgroundThread*> ThreadList;

    JobQueue queue;
    wxSemaphore semaphore;
    ThreadList threadList;

public:
    BackgroundThreadPool(size_t num_threads = 4)
    {
        for(unsigned int i = 0; i < num_threads; ++i)
            AddThread(new BackgroundThread(&queue, &semaphore));
    };

    ~BackgroundThreadPool()
    {
        for_each(threadList.begin(), threadList.end(), Agony());
        for_each(threadList.begin(), threadList.end(), Death());
        Delete(queue);
        wxMilliSleep(0);
    };

    void AddThread(BackgroundThread * t)
    {
        threadList.push_back(t);
    };

    void Queue(AbstractJob* j)
    {
        queue.Push(j);
        semaphore.Post();
    };
};



#endif

