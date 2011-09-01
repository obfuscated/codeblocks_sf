
#include "FileContent.h"
#include <wx/wfstream.h>
#include <wx/msgdlg.h>

#include <wx/cmdproc.h>


FileContentObserver::FileContentObserver(){}
FileContentObserver::~FileContentObserver(){}


FileContent::FileContent():
    observers(),
    m_modified(false),
    m_cmdProc(0)
{
    if ( m_cmdProc ) delete m_cmdProc;
}
FileContent::~FileContent(void)
{
}

bool FileContent::Save(const wxString &filename)
{
    wxFileOutputStream store(filename);
    SaveObject(store);

    if ( store.GetLastError() != wxSTREAM_NO_ERROR )
        return false;

    SetModified(false);
    return true;
}
bool FileContent::Open(const wxString &filename)
{
    wxFileInputStream store(filename);
    LoadObject(store);

    if ( store.GetLastError() != wxSTREAM_NO_ERROR )
        return false;

    SetModified(false);
    NotifyObservers(0);
    return true;
}
bool FileContent::GetModified(){return m_modified;}
void FileContent::SetModified( bool modified ) {m_modified = modified;}
void FileContent::Modify( bool modified ){SetModified(modified);}
bool FileContent::IsReadOnly(){ return false; }
void FileContent::NotifyObservers(wxObject* hint)
{
    std::set<FileContentObserver *>::iterator it;
    for (it =  observers.begin(); it !=  observers.end(); it++)
        (*it)->Update(hint);//FileContentObserver *
}
void FileContent::AddObserver(FileContentObserver *a)
{
    observers.insert(a);
}
void FileContent::RemoveObserver(FileContentObserver *a)
{
    observers.erase(a);
}
wxCommandProcessor *FileContent::CreateCommandProcessor()
{
    if ( m_cmdProc ) delete m_cmdProc;
    m_cmdProc = new wxCommandProcessor();
    return m_cmdProc;
}
wxCommandProcessor* FileContent::GetCommandProcessor()
{
    if ( !m_cmdProc ) m_cmdProc = CreateCommandProcessor();
    return m_cmdProc;
}
