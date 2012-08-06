/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "sdk_precomp.h"

#ifndef CB_PRECOMP
    #include "filemanager.h"
    #include "safedelete.h"
    #include "cbeditor.h"
    #include "editormanager.h"
    #include "infowindow.h"
#endif
#include "cbstyledtextctrl.h"

#include <wx/url.h>
#include <wx/encconv.h>

template<> FileManager* Mgr<FileManager>::instance = 0;
template<> bool  Mgr<FileManager>::isShutdown = false;

// ***** class: LoaderBase *****
LoaderBase::~LoaderBase()
{
    WaitReady();
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

// ***** class: FileLoader *****
void FileLoader::operator()()
{
    if (!wxFile::Access(fileName, wxFile::read))
    {
        Ready();
        return;
    }

    wxFile file(fileName);
    len = file.Length();

    data = new char[len+4];
    char *dp = data + len;
    *dp++ = '\0';
    *dp++ = '\0';
    *dp++ = '\0';
    *dp++ = '\0';

    if (file.Read(data, len) == wxInvalidOffset)
    {
        delete[] data;
        data = 0;
        len = 0;
    }
    Ready();
}

// ***** class: URLLoader *****
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

    char tmp[8192] = {};
    size_t chunk = 0;

    while ((chunk = stream->Read(tmp, sizeof(tmp)).LastRead()))
    {
        mBuffer.insert(mBuffer.end(), tmp, tmp + chunk);
    }

    data = mBuffer.data();
    len = mBuffer.size();
    const char Zeros4[] = "\0\0\0\0";
    mBuffer.insert(mBuffer.end(), Zeros4, Zeros4 + 4);
    Ready();
}

// ***** class: FileManager *****
FileManager::FileManager()
    : fileLoaderThread(false),
    uncLoaderThread(false),
    urlLoaderThread(false)
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
    if (reuseEditors)
    {
        EditorManager* em = Manager::Get()->GetEditorManager();
        if (em)
        {
            wxFileName fileName(file);
            for (int i = 0; i < em->GetEditorsCount(); ++i)
            {
                cbEditor* ed = em->GetBuiltinEditor(em->GetEditor(i));
                if (ed && fileName == ed->GetFilename())
                {
                    if (!ed->GetModified())
                        break;
                    EditorReuser *nl = new EditorReuser(file, ed->GetControl()->GetText());
                    return nl;
                }
            }
        }
    }

    if (file.StartsWith(_T("http://")))
    {
        URLLoader *ul = new URLLoader(file);
        urlLoaderThread.Queue(ul);
        return ul;
    }

    FileLoader *fl = new FileLoader(file);

    if (file.length() > 2 && file[0] == _T('\\') && file[1] == _T('\\'))
    {
        // UNC files behave like "normal" files, but since we know they are served over the network,
        // we can run them independently from local filesystem files for higher concurrency
        uncLoaderThread.Queue(fl);
        return fl;
    }

    fileLoaderThread.Queue(fl);
    return fl;
}

bool FileManager::Save(const wxString& name, const char* data, size_t len)
{
    if (wxFileExists(name) == false) // why bother if we don't need to
    {
        wxFile f(name, wxFile::write);
        if (!f.IsOpened())
            return false;
        return f.Write(data, len);
    }

    if (platform::windows) // work around broken Windows readonly flag
    {
        wxFile f;
        if (!f.Open(name, wxFile::read_write))
            return false;
    }

    wxString tempName(name + _T(".cbTemp"));
    do
    {
        if ( !wxCopyFile(name, tempName) )
        {
            return false;
        }

        wxFile f(name, wxFile::write);
        if ( !f.IsOpened() )
            return false;

        if (f.Write(data, len) != len)
        {
            f.Close();
            // Keep the backup file as the original file has been destroyed
            //wxRemoveFile(tempName);
            return false;
        }

        f.Close();

    } while (false);

    if (Manager::IsAppShuttingDown())
    {
        // app shut down, forget delayed deletion
        wxRemoveFile(tempName);
    }
    else
    {
        // issue a delayed deletion of the back'd up (old) file
        delayedDeleteThread.Queue(new DelayedDelete(tempName));
    }

    return true;
}

bool FileManager::Save(const wxString& name, const wxString& data, wxFontEncoding encoding, bool bom)
{
    if (wxFileExists(name) == false) // why bother if we don't need to
    {
        wxFile f(name, wxFile::write);
        if (!f.IsOpened())
            return false;
        return WriteWxStringToFile(f, data, encoding, bom);
    }

    if (platform::windows)
    {
        wxFile f;
        if (!f.Open(name, wxFile::read_write))
            return false;
    }

    wxString tempName(name + _T(".cbTemp"));
    do
    {
        if (!wxCopyFile(name, tempName))
        {
            return false;
        }

        wxFile f(name, wxFile::write);
        if ( !f.IsOpened() )
            return false;

        if (WriteWxStringToFile(f, data, encoding, bom) == false)
        {
            f.Close();
            // Keep the backup file as the original file has been destroyed
            //wxRemoveFile(tempName);
            return false;
        }

        f.Close();

    } while (false);

    if (Manager::IsAppShuttingDown())
    {
        // app shut down, forget delayed deletion
        wxRemoveFile(tempName);
    }
    else
    {
        // issue a delayed deletion of the back'd up (old) file
        delayedDeleteThread.Queue(new DelayedDelete(tempName));
    }
    return true;
}

bool FileManager::ReplaceFile(const wxString& old_file, const wxString& new_file)
{
    wxString backup_file(old_file + _T(".backup"));

    // rename the old file into a backup file
    if (wxRenameFile(old_file, backup_file))
    {
        // now rename the new created (temporary) file to the "old" filename
        if (wxRenameFile(new_file, old_file))
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

bool FileManager::WriteWxStringToFile(wxFile& f, const wxString& data, wxFontEncoding encoding, bool bom)
{
    const char* mark = 0;
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

        if (f.Write(mark, mark_length) != mark_length)
            return false;
    }

    if (data.length() == 0)
        return true;

#if defined(UNICODE) || defined(_UNICODE)

    size_t inlen = data.Len(), outlen = 0;
    wxCharBuffer mbBuff;
    if ( encoding == wxFONTENCODING_UTF7 )
    {
        wxMBConvUTF7 conv;
        mbBuff = conv.cWC2MB(data.c_str(), inlen, &outlen);
    }
    else if ( encoding == wxFONTENCODING_UTF8 )
    {
        wxMBConvUTF8 conv;
        mbBuff = conv.cWC2MB(data.c_str(), inlen, &outlen);
    }
    else if ( encoding == wxFONTENCODING_UTF16BE )
    {
        wxMBConvUTF16BE conv;
        mbBuff = conv.cWC2MB(data.c_str(), inlen, &outlen);
    }
    else if ( encoding == wxFONTENCODING_UTF16LE )
    {
        wxMBConvUTF16LE conv;
        mbBuff = conv.cWC2MB(data.c_str(), inlen, &outlen);
    }
    else if ( encoding == wxFONTENCODING_UTF32BE )
    {
        wxMBConvUTF32BE conv;
        mbBuff = conv.cWC2MB(data.c_str(), inlen, &outlen);
    }
    else if ( encoding == wxFONTENCODING_UTF32LE )
    {
        wxMBConvUTF32LE conv;
        mbBuff = conv.cWC2MB(data.c_str(), inlen, &outlen);
    }
    else
    {
        // try wxEncodingConverter first, even it it only works for
        // wxFONTENCODING_ISO8859_1..15, wxFONTENCODING_CP1250..1257 and wxFONTENCODING_KOI8
        // but it's much, much faster than wxCSConv (at least on linux)
        wxEncodingConverter conv;
        // should be long enough
        char* tmp = new char[2*inlen];

        #if wxCHECK_VERSION(2, 9, 0)
        if (conv.Init(wxFONTENCODING_UNICODE, encoding) && conv.Convert(data.wx_str(), tmp))
        #else
        if (conv.Init(wxFONTENCODING_UNICODE, encoding) && conv.Convert(data.c_str(), tmp))
        #endif
        {
            mbBuff = tmp;
            outlen = strlen(mbBuff); // should be correct, because Convert has returned true
        }
        else
        {
            // try wxCSConv, if nothing else works
            wxCSConv conv(encoding);
            mbBuff = conv.cWC2MB(data.c_str(), inlen, &outlen);
        }
        delete[] tmp;
    }
     // if conversion to chosen encoding succeeded, we write the file to disk
    if (outlen > 0)
        return f.Write(mbBuff, outlen) == outlen;

    // if conversion to chosen encoding does not succeed, we try UTF-8 instead
    size_t size = 0;
    wxCSConv conv(encoding);
    wxCharBuffer buf = data.mb_str(conv);

    if (!buf || !(size = strlen(buf)))
    {
        buf = data.mb_str(wxConvUTF8);

        if (!buf || !(size = strlen(buf)))
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

#else

    // For ANSI builds, dump the char* to file.
    return f.Write(data.c_str(), data.Length()) == data.Length();

#endif
}
