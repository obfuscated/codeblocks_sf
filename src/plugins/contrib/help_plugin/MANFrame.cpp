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

#ifndef CB_PRECOMP
    #include "globals.h" // cbC2U
#endif

namespace
{
    int butSearchID = wxNewId();
    int butZoomInID = wxNewId();
    int butZoomOutID = wxNewId();
    int textEntryID = wxNewId();
    int htmlWindowID = wxNewId();

    wxString NoSearchDirsSet = _("<html>\n"
        "<head>\n"
        "<meta content=\"text/html; charset=ISO-8859-1\"\n"
        "http-equiv=\"content-type\">\n"
        "<title></title>\n"
        "</head>\n"
        "<body>\n"
        "<h2>No search directories set</h2>\n"
        "<br>\n"
        "You must configure a man page entry first\n"
        "</body>\n"
        "</html>");

    wxString ManPageNotFound = _("<html>\n"
        "<head>\n"
        "<meta content=\"text/html; charset=ISO-8859-1\"\n"
        "http-equiv=\"content-type\">\n"
        "<title></title>\n"
        "</head>\n"
        "<body>\n"
        "<h2>Man page not found</h2>\n"
        "</body>\n"
        "</html>");

    // build all HTML font sizes (1..7) from the given base size
    void wxBuildFontSizes(int *sizes, int size)
    {
        // using a fixed factor (1.2, from CSS2) is a bad idea as explained at
        // http://www.w3.org/TR/CSS21/fonts.html#font-size-props but this is by far
        // simplest thing to do so still do it like this for now
        sizes[0] = int(size * 0.75); // exception to 1.2 rule, otherwise too small
        sizes[1] = int(size * 0.83);
        sizes[2] = size;
        sizes[3] = int(size * 1.2);
        sizes[4] = int(size * 1.44);
        sizes[5] = int(size * 1.73);
        sizes[6] = int(size * 2);
    }

    int wxGetDefaultHTMLFontSize()
    {
        // base the default font size on the size of the default system font but
        // also ensure that we have a font of reasonable size, otherwise small HTML
        // fonts are unreadable
        int size = wxNORMAL_FONT->GetPointSize();
        if ( size < 10 )
            size = 10;

        return size;
    }

    int font_sizes[7] = { 0 };
}

BEGIN_EVENT_TABLE(MANFrame, wxPanel)
    EVT_BUTTON(butSearchID, MANFrame::OnSearch)
    EVT_BUTTON(butZoomInID, MANFrame::OnZoomIn)
    EVT_BUTTON(butZoomOutID, MANFrame::OnZoomOut)
    EVT_TEXT_ENTER(textEntryID, MANFrame::OnSearch)
    EVT_HTML_LINK_CLICKED(htmlWindowID, MANFrame::OnLinkClicked)
END_EVENT_TABLE()

MANFrame::MANFrame(wxWindow *parent, wxWindowID id, const wxBitmap &zoomInBmp, const wxBitmap &zoomOutBmp)
: wxPanel(parent, id), m_baseFontSize(wxGetDefaultHTMLFontSize())
{
    wxStaticText *m_label = new wxStaticText(this, wxID_ANY, _("Man page: "));
    m_entry = new wxTextCtrl(this, textEntryID, wxEmptyString, wxDefaultPosition, wxSize(20, -1), wxTE_PROCESS_ENTER);
    m_search = new wxButton(this, butSearchID, _("Search"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
    m_zoomIn = new wxBitmapButton(this, butZoomInID, zoomInBmp);
    m_zoomOut = new wxBitmapButton(this, butZoomOutID, zoomOutBmp);
    m_htmlWindow = new wxHtmlWindow(this, htmlWindowID);

    wxBoxSizer *main = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *bar = new wxBoxSizer(wxHORIZONTAL);

    bar->Add(m_label, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);
    bar->Add(m_entry, 1, wxALL | wxALIGN_CENTER_VERTICAL, 2);
    bar->Add(m_search, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);
    bar->Add(m_zoomOut, 0, wxALIGN_CENTER_VERTICAL);
    bar->Add(m_zoomIn, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 2);

    main->Add(bar, 0, wxEXPAND);
    main->Add(m_htmlWindow, 1, wxEXPAND);

    SetSizer(main);
    SetAutoLayout(true);

    wxBuildFontSizes(font_sizes, m_baseFontSize);
    m_htmlWindow->SetFonts(wxEmptyString, wxEmptyString, font_sizes);
}

void MANFrame::SetPage(const wxString &contents)
{
    m_htmlWindow->SetPage(contents);
}

void MANFrame::OnLinkClicked(wxHtmlLinkEvent &event)
{
    wxString link = event.GetLinkInfo().GetHref();

    if (!link.StartsWith(_T("man:"), &link))
    {
        return;
    }

    if (link.Last() == _T(')'))
    {
        link.RemoveLast();
        wxString name = link.BeforeLast(_T('('));

        if (name.IsEmpty())
        {
            return;
        }

        wxString section = link.AfterLast(_T('('));

        if (!section.IsEmpty())
        {
            name += _T(".") + section;
        }

        SearchManPage(wxEmptyString, name);
    }
}

void MANFrame::OnSearch(wxCommandEvent &event)
{
    SearchManPage(wxEmptyString, m_entry->GetValue());
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

void MANFrame::SetBaseFontSize(int newsize)
{
    m_baseFontSize = newsize;
    wxBuildFontSizes(font_sizes, m_baseFontSize);
    m_htmlWindow->SetFonts(wxEmptyString, wxEmptyString, font_sizes);
}

void MANFrame::OnZoomIn(wxCommandEvent &)
{
    ++m_baseFontSize;

    if (m_baseFontSize > 20)
    {
        m_baseFontSize = 20;
    }

    SetBaseFontSize(m_baseFontSize);
}

void MANFrame::OnZoomOut(wxCommandEvent &)
{
    --m_baseFontSize;

    if (m_baseFontSize < 6)
    {
        m_baseFontSize = 6;
    }

    SetBaseFontSize(m_baseFontSize);
}

bool MANFrame::SearchManPage(const wxString &dirs, const wxString &keyword)
{
    wxString man_page = GetManPage(dirs, keyword);

    if (keyword.IsEmpty())
    {
        if (dirs.Length() == wxString(_T("man:")).Length())
        {
            SetPage(NoSearchDirsSet);
        }

        return false;
    }

    m_entry->SetValue(keyword);

    if (man_page.IsEmpty())
    {
        SetPage(ManPageNotFound);
        return false;
    }

    SetPage(cbC2U(man2html_buffer(man_page.mb_str())));

    return true;
}
