#include "devpakinstaller.h"
#include <bzlib.h>
#include "mytar.h"
#include "cbiniparser.h"
#include <wx/filename.h>
#include <wx/msgdlg.h>

DevPakInstaller::DevPakInstaller()
    : m_pDlg(0)
{
	//ctor
}

DevPakInstaller::~DevPakInstaller()
{
	//dtor
    EndProgressDialog();
}

void DevPakInstaller::CreateProgressDialog(int max)
{
    EndProgressDialog();
    m_pDlg = new wxProgressDialog("Progress", "", max, 0, wxPD_APP_MODAL);
    m_pDlg->SetSize(480, m_pDlg->GetSize().y);
    m_pDlg->Centre();
}

void DevPakInstaller::EndProgressDialog()
{
	if (m_pDlg)
        m_pDlg->Destroy();
    m_pDlg = 0;
}

void DevPakInstaller::UpdateProgress(int val, const wxString& newtext)
{
    if (m_pDlg)
    {
        m_pDlg->Update(val, newtext);
        wxYield();
    }
}

bool DevPakInstaller::Install(const wxString& name, const wxString& filename, const wxString& dir, wxArrayString* files)
{
    CreateProgressDialog(4);
    m_Status.Clear();

    // Step 1: decompress
    UpdateProgress(0, "Decompressing " + filename);
    wxYield();
    wxString m_Status;
    wxString tmpfile = wxFileName::CreateTempFileName("cb");
    if (!Decompress(filename, tmpfile))
    {
        m_Status += " [Decompression failed]";
        wxRemoveFile(tmpfile);
        EndProgressDialog();
        return false;
    }

    // Step 2: un-tar .DevPackage file
    UpdateProgress(1, "Unpacking control-file");
    wxYield();
    TAR* t = new TAR(tmpfile.c_str());
    TAR::Record* r = t->FindFile("*.DevPackage");
    wxString controlFile = r->name;
    wxString status2;
    if (!r || !t->ExtractFile(r, dir, status2))
    {
        m_Status << " [Control file unpacking failed] - ";
        m_Status << status2;
        delete t;
        wxRemoveFile(tmpfile);
        EndProgressDialog();
        return false;
    }

    // Step 3: un-tar
    UpdateProgress(2, "Unpacking all files");
    wxYield();
    if (!Untar(controlFile, tmpfile, dir, files))
    {
        m_Status += " [Unpacking failed]";
        delete t;
        wxRemoveFile(tmpfile);
        RemoveControlFile(dir + "/" + controlFile);
        EndProgressDialog();
        return false;
    }

    UpdateProgress(3, "Done");
    delete t;
    wxRemoveFile(tmpfile);
    RemoveControlFile(dir + "/" + controlFile);
    EndProgressDialog();
    return true;
}

bool DevPakInstaller::Uninstall(const wxString& entry)
{
    m_Status.Clear();
    IniParser* p = new IniParser;
    p->ParseFile(entry);
    
    int idx = p->FindGroupByName("Files", false);
    if (idx == -1)
    {
        m_Status << "No [Files] section in " << entry << '\n';
        return false;
    }
    wxArrayString pathlist;
    CreateProgressDialog(p->GetKeysCount(idx));
    for (int i = 0; i < p->GetKeysCount(idx); ++i)
    {
        wxString file = p->GetKeyName(idx, i);
        UpdateProgress(i, "Removing: " + file);
        if (!wxRemoveFile(file))
            m_Status << "Can't remove " << file << '\n';
        else
        {
            wxString path = wxPathOnly(file);
            while (path.Last() == '/' || path.Last() == '\\')
                path.RemoveLast();
            if (pathlist.Index(path) == wxNOT_FOUND)
                pathlist.Add(path);
        }
    }
    EndProgressDialog();
    // remove dirs
    CreateProgressDialog(pathlist.GetCount());
    for (unsigned int i = 0; i < pathlist.GetCount(); ++i)
    {
        wxString path = pathlist[i];
        size_t pos = path.Find('/', true);
        while (pos != wxNOT_FOUND)
        {
            wxRmdir(path);
            path.Remove(pos, path.Length() - pos);
            UpdateProgress(i, "Removing directory: " + path);
            pos = path.Find('/', true);
        }
    }
    EndProgressDialog();
    delete p;
    if (!wxRemoveFile(entry))
        m_Status << "Can't remove " << entry << '\n';

    return true; //m_Status.IsEmpty();
}

void DevPakInstaller::RemoveControlFile(const wxString& filename)
{
    if (filename.IsEmpty())
        return;
    wxRemoveFile(filename);
    wxRmdir(wxFileName(filename).GetPath()); // deletes it if non-empty
}

bool DevPakInstaller::Decompress(const wxString& filename, const wxString& tmpfile)
{    
    // open file
    FILE* f = fopen(filename.c_str(), "rb");
    if (!f)
    {
        m_Status = "Error opening input file!";
        return false;
    }
    
    // open BZIP2 stream
    int bzerror;
    BZFILE* bz = BZ2_bzReadOpen(&bzerror, f, 0, 0, 0L, 0);
    if (!bz || bzerror != BZ_OK)
    {
        m_Status = "Can't read compressed stream!";
        fclose(f);
        return false;
    }

    // open output file
    FILE* fo = fopen(tmpfile.c_str(), "wb");
    if (!fo)
    {
        m_Status = "Error opening output file!";
        fclose(f);
        return false;
    }

    // read stream writing to uncompressed file
    char buffer[2048];
    while (bzerror != BZ_STREAM_END)
    {
        BZ2_bzRead(&bzerror, bz, buffer, 2048);
        if (bzerror != BZ_OK && bzerror != BZ_STREAM_END)
        {
            m_Status = "Error reading from stream!";
            BZ2_bzReadClose(&bzerror, bz);
            fclose(fo);
            fclose(f);
            return false;
        }
        fwrite(buffer, 2048, 1, fo);
    }

    BZ2_bzReadClose(&bzerror, bz);


    fclose(fo);
    fclose(f);
    return true;
}

bool DevPakInstaller::Untar(const wxString& controlFile, const wxString& filename, const wxString& dirname, wxArrayString* files)
{
    TAR t(filename.c_str());
    wxString tmpControlFile;

    if (!controlFile.IsEmpty())
    {
        IniParser ini;
        ini.ParseFile(dirname + "/" + controlFile);
        int grp = ini.FindGroupByName("Files");
        for (int i = 0; grp != -1 && i < ini.GetKeysCount(grp); ++i)
        {
            t.AddReplacer(ini.GetKeyName(grp, i), ini.GetKeyValue(grp, i));
        }

        // add replacers for all paths above controlFile
        // i.e. in Allegro.DevPak:
        // Test/Allegro.DevPackage (everything is below Test/),
        // add replacer for Test/
        int pos = controlFile.Last('/');
        if (pos != wxNOT_FOUND)
        {
            wxString replacer = controlFile.SubString(0, pos);
            tmpControlFile = controlFile;
            tmpControlFile.Replace(replacer, "");
            t.AddReplacer(replacer, "");
        }

        // add replacers (with user's permission) for DLLs
        #ifdef __WXMSW__
        TAR::Record* r = t.FindFile("*.dll");
        if (!r)
            r = t.FindFile("*.DLL");
        if (r)
        {
            if (wxMessageBox("This package contains some DLLs.\nDo you want to install them system-wide (YES recommended)?", "Confirmation", wxICON_QUESTION | wxYES_NO) == wxYES)
            {
                wxFileName fname(wxGetOSDirectory() + "/system32/");
                fname.MakeRelativeTo(dirname);
                t.AddReplacer("dll/", fname.GetFullPath());
            }
            wxYield();
        }
        #endif
    }

    bool ret = t.ExtractAll(dirname.c_str(), m_Status, files);
    if (!tmpControlFile.IsEmpty())
        RemoveControlFile(dirname + "/" + tmpControlFile);
    return ret;
}
