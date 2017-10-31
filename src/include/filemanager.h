/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <wx/thread.h>
#include <wx/string.h>
#include <wx/log.h>

#include "backgroundthread.h"

#undef new
#include <deque>
#include <memory>

#ifndef CB_PRECOMP
    #include <wx/file.h>
    #include <wx/filename.h>
    #include "configmanager.h"
#endif


class TiXmlDocument;
namespace TinyXML{ bool SaveDocument(const wxString&, TiXmlDocument*); }


// ***** class: LoaderBase *****
class DLLIMPORT LoaderBase : public AbstractJob
{
    wxSemaphore sem;
    bool wait;

    void WaitReady()
    {
        if (wait)
        {
            wait = false;
            sem.Wait();
        }
    };

protected:
    wxString fileName;
    char *data;
    size_t len;

    void Ready()
    {
        sem.Post();
    };

public:
    LoaderBase() : wait(true), data(nullptr), len(0) {};
    ~LoaderBase();
    void operator()() {};
    wxString FileName() const { return fileName; };

    bool Sync();
    char* GetData();
    size_t GetLength();
};

// ***** class: FileLoader *****
class DLLIMPORT FileLoader : public LoaderBase
{
public:
    FileLoader(const wxString& name) { fileName = name; };
    void operator()();
};

// ***** class: URLLoader *****
class DLLIMPORT URLLoader : public LoaderBase
{
public:
    URLLoader(const wxString& name) { fileName = name; };
    void operator()();
private:
    std::vector<char> mBuffer;
};

// ***** class: NullLoader *****
class DLLIMPORT NullLoader : public LoaderBase
{
public:
    NullLoader(const wxString& name, char* buffer, size_t size) { fileName = name; data = buffer; len = size; Ready(); };
    void operator()(){};
};

// ***** class: EditorReuser *****
class DLLIMPORT EditorReuser : public LoaderBase
{
public:
    /** Gets a UTF8 representation of the editor's content
     *  @param name the file name opened by the editor
     *  @param s the editor's content
     */
    EditorReuser(const wxString& name, const wxString& s)
    {
        fileName = name;
        len = strlen(s.mb_str(wxConvUTF8));
        data = new char[len + 1];
        strcpy(data, (const char*)s.mb_str(wxConvUTF8));
        Ready();
    }
    void operator()(){};
};

// ***** class: FileManager *****
class DLLIMPORT FileManager : public Mgr<FileManager>
{
    BackgroundThread fileLoaderThread;
    BackgroundThread uncLoaderThread;
    BackgroundThread urlLoaderThread;
public:
    FileManager();
    ~FileManager();

    /** Loads a file, once this function is called, the actually loading process is done in the
     *  worker thread(BackgroundThread).
     *  @param file the file path, it can an absolute file path, an unc file, or a url file
     *  @param reuseEditors if true, we try to use the file contents in the editor if the file is
     *  modified in the editor.
     *  @return a loader pointer holding the loader, user must delete it later, otherwise memory
     *  leak will happen.
     */
    cb_must_consume_result LoaderBase* Load(const wxString& file, bool reuseEditors = false);

    bool Save(const wxString& file, const wxString& data, wxFontEncoding encoding, bool bom);

private:
    friend bool TinyXML::SaveDocument(const wxString&, TiXmlDocument*);
    bool SaveUTF8(const wxString& file, const char* data, size_t len);

    bool WriteWxStringToFile(wxFile& f, const wxString& data, wxFontEncoding encoding, bool bom);
};

#endif
