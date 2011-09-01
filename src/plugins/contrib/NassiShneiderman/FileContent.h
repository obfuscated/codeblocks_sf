#ifdef __GNUG__
// #pragma interface
#endif

#ifndef __FILE_CONTENT__
#define __FILE_CONTENT__

#include <wx/stream.h>


#include <set>

class FileContentObserver
{
public:
    FileContentObserver();
    virtual ~FileContentObserver();
    virtual void Update(wxObject* hint) = 0;
};
class wxCommandProcessor;

class FileContent
{
public:

    FileContent();
    virtual ~FileContent(void);
    virtual wxCommandProcessor* CreateCommandProcessor();
    wxCommandProcessor* GetCommandProcessor();


    bool Save(const wxString &filename);
    bool Open(const wxString &filename);
    bool GetModified();
    void SetModified( bool modified );
    void Modify( bool modified );
    void NotifyObservers(wxObject* hint = 0);
    bool IsReadOnly();

    virtual wxOutputStream& SaveObject(wxOutputStream& stream) = 0;
    virtual wxInputStream& LoadObject(wxInputStream& stream) = 0;

    //bool IsReadOnly();
    virtual wxString GetWildcard() = 0;

private:
    FileContent(const FileContent &rhs);
    FileContent &operator=(const FileContent &rhs);

public:
    void AddObserver(FileContentObserver *a);
    void RemoveObserver(FileContentObserver *a);
private:
    std::set<FileContentObserver *> observers;
//    bool m_readOnly;
    bool m_modified;
    wxCommandProcessor *m_cmdProc;
    //wxString m_filename;
};

#endif //__CB_FILE_CONTENT__


