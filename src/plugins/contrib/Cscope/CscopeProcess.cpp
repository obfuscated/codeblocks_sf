#include "CscopeProcess.h"
#include "CscopePlugin.h"
#include <wx/txtstrm.h>

CscopeProcess::CscopeProcess(CscopePlugin* parent):
    wxProcess(parent)
{
    //ctor
    wxASSERT(parent);
    m_parent = parent;

    //turn on redirect
    Redirect();
}

CscopeProcess::~CscopeProcess()
{
    //dtor
}
bool CscopeProcess::ReadProcessOutput()
{

    //wxProcess::GetInputStream() will capture stdout!
    if (IsInputAvailable())
    {
        wxTextInputStream ts(*GetInputStream());
        wxString line = ts.ReadLine();

        if(line.Length())
        {
            m_parent->OnProcessGeneratedOutputLine(line);
        }
        return true;
    }
    return false;
}
