#include "MANFrame.h"
#include "man2html.h"

#include <wx/sizer.h>
#include <wx/dir.h>
#include <wx/sstream.h>
#include <wx/wfstream.h>
#include <wx/filename.h>
#include <stdio.h>
#include <bzlib.h>
#include <zlib.h>

namespace
{
    int butSearchID = wxNewId();
    int textEntryID = wxNewId();
}

BEGIN_EVENT_TABLE(MANFrame, wxFrame)
    EVT_BUTTON(butSearchID, MANFrame::OnSearch)
    EVT_TEXT_ENTER(textEntryID, MANFrame::OnSearch)
    EVT_CLOSE(MANFrame::OnClose)
END_EVENT_TABLE()

MANFrame::MANFrame(wxWindow *parent, wxWindowID id, const wxString &title)
: wxFrame(parent, id, title)
{
    wxStaticText *m_label = new wxStaticText(this, wxID_ANY, _("Man page: "));
    m_entry = new wxTextCtrl(this, textEntryID, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    m_search = new wxButton(this, butSearchID, _("Search"));
    m_htmlWindow = new wxHtmlWindow(this);

    wxBoxSizer *main = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *bar = new wxBoxSizer(wxHORIZONTAL);

    bar->Add(m_label, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    bar->Add(m_entry, 1, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    bar->Add(m_search, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

    main->Add(bar, 0, wxEXPAND);
    main->Add(m_htmlWindow, 1, wxEXPAND);

    SetSizer(main);
    SetAutoLayout(true);
    SetSize(512, 384);
    CentreOnScreen();
}

void MANFrame::SetPage(const wxString &contents)
{
    m_htmlWindow->SetPage(contents);
}

void MANFrame::OnSearch(wxCommandEvent &event)
{
    SearchManPage(wxEmptyString, m_entry->GetValue());
}

void MANFrame::OnClose(wxCloseEvent &event)
{
    Show(false);
    event.Veto(true);
}

bool MANFrame::Decompress(const wxString& filename, const wxString& tmpfile)
{
    // open file
    FILE* f = fopen(filename.mb_str(), "rb");
    if (!f)
    {
        return false;
    }

    // open BZIP2 stream
    int bzerror;
    BZFILE* bz = BZ2_bzReadOpen(&bzerror, f, 0, 0, 0L, 0);
    if (!bz || bzerror != BZ_OK)
    {
        fclose(f);
        return false;
    }

    // open output file
    FILE* fo = fopen(tmpfile.mb_str(), "wb");
    if (!fo)
    {
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

wxString MANFrame::GetManPage(const wxString &dirs, const wxString &keyword)
{
    if (!dirs.IsEmpty())
    {
        m_dirsVect.clear();
        size_t start_pos = 4; // len("man:")

        while (true)
        {
            size_t next_semi = dirs.find(_T(';'), start_pos);

            if ((int)next_semi == wxNOT_FOUND)
            {
                next_semi = dirs.Length();
            }

            m_dirsVect.push_back(dirs.SubString(start_pos, next_semi - 1));

            if (next_semi == dirs.Length())
            {
                break;
            }

            start_pos = next_semi + 1;
        }
    }

    if (m_dirsVect.empty() || keyword.IsEmpty())
    {
        return wxString();
    }

    for (std::vector<wxString>::iterator i = m_dirsVect.begin(); i != m_dirsVect.end(); ++i)
    {
        wxString filename = wxDir::FindFirst(*i, keyword + _T(".*"));

        if (filename.IsEmpty())
        {
            continue;
        }

        if (filename.EndsWith(_T(".bz2")))
        {
            wxString tmpfile = wxFileName::CreateTempFileName(_T("manbz2"));

            if (!Decompress(filename, tmpfile))
            {
                wxRemoveFile(tmpfile);
                continue;
            }

            filename = tmpfile; // tmpfile isn't removed
        }
        else if (filename.EndsWith(_T(".gz")))
        {
            gzFile f = gzopen(filename.mb_str(), "rb");

            if (!f)
            {
                continue;
            }

            char buffer[4096];
            wxString ret;
            int read_bytes = -1;

            while (true)
            {
                read_bytes = gzread(f, buffer, sizeof(buffer));

                if (read_bytes <= 0) // -1 = error, 0 = eof
                {
                    break;
                }

                ret += wxString(buffer, wxConvLocal, read_bytes);
            }

            gzclose(f);

            if (read_bytes == -1)
            {
                continue;
            }

            return ret;
        }

        wxStringOutputStream sos;
        wxFileInputStream f(filename);

        if (!f.IsOk())
        {
            continue;
        }

        f.Read(sos);

        return sos.GetString();
    }

    return wxString();
}

bool MANFrame::SearchManPage(const wxString &dirs, const wxString &keyword)
{
    wxString man_page = GetManPage(dirs, keyword);

    if (!keyword.IsEmpty())
    {
        m_entry->SetValue(keyword);
    }

    if (man_page.IsEmpty())
    {
        if (!keyword.IsEmpty())
        {
            SetTitle(_("Man page not found: ") + keyword);
        }

        return false;
    }

    SetPage(cbC2U(man2html_buffer(man_page.mb_str())));
    SetTitle(keyword);

    return true;
}
