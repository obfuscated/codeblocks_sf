#ifndef CBTHREADPOOL_EXTRAS_H
#define CBTHREADPOOL_EXTRAS_H

/// Josuttis' implementation of CountedPtr
template <typename T>
class CountedPtr
{
  private:
    T *ptr;
    long *count;

  public:
    explicit CountedPtr(T *p = 0);
    CountedPtr(const CountedPtr<T> &p) throw();
    ~CountedPtr() throw();
    CountedPtr<T> &operator = (const CountedPtr<T> &p) throw();
    T &operator * () const throw();
    T *operator -> () const throw();

  private:
    void dispose();
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
      * @param semaphore Used to synchronise the Worker Threads
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
    CountedPtr<wxSemaphore> m_semaphore;
    cbThreadedTask *m_pTask;
    wxMutex m_taskMutex;
};

#endif
