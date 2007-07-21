#include "sdk_precomp.h"

#ifndef CB_PRECOMP
    #include "filemanager.h"
    #include "safedelete.h"
    #include "cbeditor.h"
    #include "editormanager.h"
    #include "infowindow.h"
#endif

#include <wx/url.h>

LoaderBase::~LoaderBase()
{
    delete[] data;
};

bool LoaderBase::Sync()
{
    WaitReady();
    return data;
};

char* LoaderBase::GetData()
{
    WaitReady();
    return data;
};

size_t LoaderBase::GetLength()
{
    WaitReady();
    return len;
};

void FileLoader::operator()()
{
    if(!wxFile::Access(fileName, wxFile::read))
    {
        Ready();
        return;
    }

    wxFile file(fileName);
    len = file.Length();

    data = new char[len+1];
    data[len] = '\0';

    if(file.Read(data, len) == wxInvalidOffset)
    {
        delete[] data;
        data = 0;
        len = 0;
    }
    Ready();
}

void URLLoader::operator()()
{
    wxURL url(fileName);
    url.SetProxy(ConfigManager::GetProxy());

    if (url.GetError() != wxURL_NOERR)
    {
        Ready();
        return;
    }

    std::auto_ptr<wxInputStream> stream(url.GetInputStream());

    if (stream.get() == 0 || stream->IsOk() == false)
    {
        Ready();
        return;
    }

    char tmp[8192];
    size_t chunk = 0;

    while((chunk = stream->Read(tmp, sizeof(tmp)).LastRead()))
        buffer.Append(tmp, chunk);

    data = buffer.Data();
    len = buffer.Length();
    Ready();
}

FileManager::FileManager()
    : fileLoaderThread(false),
    uncLoaderThread(false),
    urlLoaderThread(false)//,
//  delayedDeleteThread(false)
{
}

FileManager::~FileManager()
{
//  delayedDeleteThread.Die();
//  fileLoaderThread.Die();
//  uncLoaderThread.Die();
//  urlLoaderThread.Die();
}

LoaderBase* FileManager::Load(const wxString& file, bool reuseEditors)
{
    if(reuseEditors)
    {
        EditorManager* em = Manager::Get()->GetEditorManager();
        if(em)
        {
            wxFileName fileName(file);
            for(int i = 0; i < em->GetEditorsCount(); ++i)
            {
                cbEditor* ed = em->GetBuiltinEditor(em->GetEditor(i));
                if(ed && fileName == ed->GetFilename())
                {
                    wxString s(ed->GetControl()->GetText());
                    NullLoader *nl = new NullLoader(file, (char*) s.c_str(), s.length() * sizeof(wxChar));
                    return nl;
                }
            }
        }
    }

    if(file.StartsWith(_T("http://")))
    {
        URLLoader *ul = new URLLoader(file);
        urlLoaderThread.Queue(ul);
        return ul;
    }

    FileLoader *fl = new FileLoader(file);

    if(file.length() > 2 && file[0] == _T('\\') && file[1] == _T('\\'))
    {
        // UNC files behave like "normal" files, but since we know they are served over the network,
        // we can run them independently from local filesystem files for higher concurrency
        uncLoaderThread.Queue(fl);
        return fl;
    }

    fileLoaderThread.Queue(fl);
    return fl;
}



inline bool WriteWxStringToFile(wxFile& f, const wxString& data, wxFontEncoding encoding, bool bom)
{
    char* mark = 0;
    size_t mark_length = 0;
    if (bom)
    {
        switch (encoding)
        {
        case wxFONTENCODING_UTF8:
            mark = "\xEF\xBB\xBF";
            mark_length = 3;
            break;
        case wxFONTENCODING_UTF16BE:
            mark = "\xFE\xFF";
            mark_length = 2;
            break;
        case wxFONTENCODING_UTF16LE:
            mark = "\xFF\xFE";
            mark_length = 2;
            break;
        case wxFONTENCODING_UTF32BE:
            mark = "\x00\x00\xFE\xFF";
            mark_length = 4;
            break;
        case wxFONTENCODING_UTF32LE:
            mark = "\xFF\xFE\x00\x00";
            mark_length = 4;
            break;
        case wxFONTENCODING_SYSTEM:
        default:
            break;
        }
    }

    if(f.Write(mark, mark_length) != mark_length)
        return false;

    if(data.length() == 0)
        return true;


    if( encoding == wxFONTENCODING_UTF16 || encoding == wxFONTENCODING_UTF16LE || encoding == wxFONTENCODING_UTF16BE ||
        encoding == wxFONTENCODING_UTF32 || encoding == wxFONTENCODING_UTF32LE || encoding == wxFONTENCODING_UTF32BE)
    {
        //FIXME: This code probably does not work correctly, as it does not really convert anything according to
        //       the requested encoding. The output is always whatever wxWidgets uses as native data.
        //       However, it's been like this ever since Code::Blocks started using Unicode, and nobody ever complained...
        //       Apparently, UTF-16/UTF-32 is not a big issue ---> will leave it as it is for now.
        const size_t len = data.Length() * sizeof(wxChar);
        return f.Write(data.c_str(), len) == len;
    }

    size_t size = 0;
    wxCSConv conv(encoding);
    wxCharBuffer buf = data.mb_str(conv);

    if((buf == 0) || !(size = strlen(buf)))
    {
        buf = data.mb_str(wxConvUTF8);

        if((buf == 0) || !(size = strlen(buf)))
            {
                cbMessageBox(_T(    "The file could not be saved because it contains characters "
                                    "that can neither be represented in your current code page, "
                                    "nor be converted to UTF-8.\n"
                                    "The latter should actually not be possible.\n\n"
                                    "Please check your language/encoding settings and try saving again." ),
                                    _("Failure"), wxICON_WARNING | wxOK );
                return false;
            }
        else
            {
                InfoWindow::Display(_("Encoding Changed"),
                                    _("The saved document contained characters\n"
                                      "which were illegal in the selected encoding.\n\n"
                                      "The file's encoding has been changed to UTF-8\n"
                                      "to prevent you from losing data."), 8000);
            }
    }

    return f.Write(buf, size);
};

bool FileManager::Save(const wxString& name, const char* data, size_t len)
{
    if(wxFileExists(name) == false) // why bother if we don't need to
    {
        wxFile f(name, wxFile::write);
        if(!f.IsOpened())
            return false;
        return f.Write(data, len);
    }

    if(platform::windows) // work around broken Windows readonly flag
    {
        wxFile f;
        if(!f.Open(name, wxFile::read_write))
            return false;
    }

    wxString tempName(name + _T(".cbTemp"));
    do
    {
        wxFile f(tempName, wxFile::write);
        if(!f.IsOpened())
            return false;

        if(f.Write(data, len) != len)
        {
            f.Close();
            wxRemoveFile(tempName);
            return false;
        }
    }while(false);

    return ReplaceFile(name, tempName);
}

bool FileManager::Save(const wxString& name, const wxString& data, wxFontEncoding encoding, bool bom)
{
    if(wxFileExists(name) == false) // why bother if we don't need to
    {
        wxFile f(name, wxFile::write);
        if(!f.IsOpened())
            return false;
        return WriteWxStringToFile(f, data, encoding, bom);
    }

    if(platform::windows)
    {
        wxFile f;
        if(!f.Open(name, wxFile::read_write))
            return false;
    }

    wxString tempName(name + _T(".cbTemp"));
    do
    {
        wxFile f(tempName, wxFile::write);
        if(!f.IsOpened())
            return false;

        if(WriteWxStringToFile(f, data, encoding, bom) == false)
        {
            f.Close();
            wxRemoveFile(tempName);
            return false;
        }
    }while(false);

    return ReplaceFile(name, tempName);
}

bool FileManager::ReplaceFile(const wxString& old_file, const wxString& new_file)
{
    wxString backup_file(old_file + _T(".backup"));

    // rename the old file into a backup file
    if(wxRenameFile(old_file, backup_file))
    {
        // now rename the new created (temporary) file to the "old" filename
        if(wxRenameFile(new_file, old_file))
        {
            if (Manager::IsAppShuttingDown())
            {
                // app shut down, forget delayed deletion
                wxRemoveFile(backup_file);
            }
            else
            {
                // issue a delayed deletion of the back'd up (old) file
                delayedDeleteThread.Queue(new DelayedDelete(backup_file));
            }
            return true;
        }
        else
        {
            // if final rename operation failed, restore the old file from backup
            wxRenameFile(backup_file, old_file);
        }
    }

    return false;
}
