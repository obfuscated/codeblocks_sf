#ifndef CSCOPEPROCESS_H
#define CSCOPEPROCESS_H

#include <wx/process.h>


class CscopePlugin;
class CscopeProcess : public wxProcess
{
        //DECLARE_DYNAMIC_CLASS(CscopeProcess)
        CscopePlugin* m_parent;
    public:
        CscopeProcess(CscopePlugin* parent);
        virtual ~CscopeProcess();
        bool ReadProcessOutput();
    protected:
    private:
};



#endif // CSCOPEPROCESS_H
