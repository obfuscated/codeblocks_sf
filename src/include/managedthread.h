/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef managedthread_h
#define managedthread_h

#include <wx/thread.h>
#include <wx/dynarray.h>

class ManagedThread;

WX_DEFINE_ARRAY(ManagedThread*, ManagedThreadsArray);

class ManagedThread : public wxThread
{
public:
    ManagedThread(bool* abortflag  = 0L);
    virtual ~ManagedThread();
    static unsigned long count_running();
    static unsigned long count_threads();
    static void abort_all(); // Warning! Once set, can't be reset!
    static void abort(bool* flag,bool delete_thread = true);
    bool* get_abort_location() { if(!this) return 0L;return m_pAbort; }
    void set_abort_location(bool* abortflag)
    {
        if(!this) return;
        m_pAbort=abortflag;
    }
protected:
    virtual bool TestDestroy();
    virtual void* DoRun(); // Override it for your own class. Called by Entry()
    virtual void* Entry();
    bool is_aborted();
    static ManagedThread* GetThread(size_t n);
    static void DeleteThreadFromList(ManagedThread* thread);

    static wxCriticalSection s_count_running_mutex;
    static wxCriticalSection s_list_mutex;
    static unsigned long s_running;
    bool* m_pAbort;
    static bool s_abort_all;
};

#endif
