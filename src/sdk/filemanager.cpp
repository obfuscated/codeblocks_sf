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

template<> FileManager* Mgr<FileManager>::instance = nullptr;
template<> bool  Mgr<FileManager>::isShutdown = false;

// ***** class: LoaderBase *****
LoaderBase::~LoaderBase()
{
    WaitReady();
    delete[] data;
}

bool LoaderBase::Sync()
{
    WaitReady();
    return data;
}

char* LoaderBase::GetData()
{
    WaitReady();
    return data;
}

size_t LoaderBase::GetLength()
{
    WaitReady();
    return len;
}

// ***** class: FileLoader *****
void FileLoader::operator()()
{
    if (!wxFile::Access(fileName, wxFile::read) || wxFileName::DirExists(fileName))
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
        data = nullptr;
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

    if (stream.get() == nullptr || stream->IsOk() == false)
    {
        Ready();
        return;
    }

    char   tmp[8192] = {};
    size_t chunk     = 0;

    while ((chunk = stream->Read(tmp, sizeof(tmp)).LastRead()))
    {
        mBuffer.insert(mBuffer.end(), tmp, tmp + chunk);
    }

#ifdef __APPLE__
    data = &mBuffer[0];
#else
    data = mBuffer.data();
#endif
    len  = mBuffer.size();
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
//  fileLoaderThread.Die();
//  uncLoaderThread.Die();
//  urlLoaderThread.Die();
}

LoaderBase* FileManager::Load(const wxString& file, bool reuseEditors)
{
    if (reuseEditors)
    {
        // if a file is opened in the editor, and the file get modified, we use the content of the
        // editor, otherwise, we still use the original file
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


namespace platform
{
#if defined ( __WIN32__ ) || defined ( _WIN64 )
    // Yes this is ugly. Feel free to come up with a better idea if you have one.
	// Using the obvious wxRenameFile (or the underlying wxRename) is no option under Windows, since
	// wxRename is simply a fuckshit wrapper around a CRT function which does not work the way
	// the wxRename author assumes (MSVCRT rename fails if the target exists, instead of overwriting).
	inline bool move(wxString const& old_name, wxString const& new_name)
	{
		// hopefully I got the unintellegible conversion stuff correct... at least it seems to work...
		return ::MoveFileEx(wxFNCONV(old_name), wxFNCONV(new_name), MOVEFILE_REPLACE_EXISTING);
	}
#else
	inline bool move(wxString const& old_name, wxString const& new_name)
	{
		return ::wxRenameFile(old_name, new_name, true);
	};
#endif
}


// FileManager::SaveUTF8 (formerly const char* overload of FileManager::Save) uses wxFile::Save without any conversions.
// It is only suitable e.g. for saving a TiXmlDocument (UTF8 data), but not for editors or projects, which contain wide char data.
// Therefore it is now a private member and has been renamed to prevent accidential use.
//
// FileManager::Save uses FileManager::WriteWxStringToFile to write data, its operation is otherwise exactly identical.
//
// 1. If file does not exist, create in exclusive mode, save, return success or failure. Exclusive mode is presumably "safer".
// 2. Otherwise (file exists), save to temporary.
// 3. Attempt to atomically replace original with temporary.
// 4. If this fails, rename temporary to document failure.
// 5. No more delayed deletes -- This requires special paths for when the application shuts down
//    and only lends to race conditions and a possible crash-on-exit.
//    If you can't trust your computer to sync data to disk properly, you already lost.

bool FileManager::SaveUTF8(const wxString& name, const char* data, size_t len)
{
    if (wxFileExists(name) == false)
    {
        return wxFile(name, wxFile::write_excl).Write(data, len) == len;
    }
	else
	{
        if (!wxFile::Access(name, wxFile::write))
            return false;

        wxString temp(name);
        temp.append(wxT(".temp"));

        wxStructStat buff;
        wxLstat( name, &buff );

        wxFile f;
        f.Create(temp, true, buff.st_mode);

		if(f.Write(data, len) == len)
		{
			f.Close();
			if(platform::move(temp, name))
			{
				return true;
			}
			else
			{
				wxString failed(name);
				failed.append(wxT(".save-failed"));
				platform::move(temp, failed);
			}
		}
		return false;
	}
}

bool FileManager::Save(const wxString& name, const wxString& data, wxFontEncoding encoding, bool bom)
{
    if (wxFileExists(name) == false)
    {
		wxFile f(name, wxFile::write_excl);
        return WriteWxStringToFile(f, data, encoding, bom);
    }
	else
	{
        if (!wxFile::Access(name, wxFile::write))
            return false;

        wxString temp(name);
        temp.append(wxT(".temp"));

        wxStructStat buff;
        wxLstat( name, &buff );

        wxFile f;
        f.Create(temp, true, buff.st_mode);

        if(WriteWxStringToFile(f, data, encoding, bom))
		{
			f.Close();
			if(platform::move(temp, name))
			{
				return true;
			}
			else
			{
				wxString failed(name);
				failed.append(wxT(".save-failed"));
				platform::move(temp, failed);
			}
		}
		return false;
	}
}


bool FileManager::WriteWxStringToFile(wxFile& f, const wxString& data, wxFontEncoding encoding, bool bom)
{
    const char* mark = nullptr;
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

        if (conv.Init(wxFONTENCODING_UNICODE, encoding) && conv.Convert(data.wx_str(), tmp))
        {
            mbBuff = tmp;
            outlen = strlen(mbBuff); // should be correct, because Convert has returned true
        }
        else
        {
            // try wxCSConv, if nothing else works
            wxCSConv csconv(encoding);
            mbBuff = csconv.cWC2MB(data.c_str(), inlen, &outlen);
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

    return f.Write(buf, size) == size;

#else

    // For ANSI builds, dump the char* to file.
    return f.Write(data.c_str(), data.Length()) == data.Length();

#endif
}
