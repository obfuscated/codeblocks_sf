/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */
#include <sdk.h>

#ifndef CB_PRECOMP
  #include <map>
  #include <wx/arrstr.h>
  #include <wx/button.h>
  #include <wx/dir.h>
  #include <wx/filename.h>
  #include <wx/regex.h>
  #include <wx/sizer.h>
  #include <globals.h> // cbC2U
#endif

#include <bzlib.h>
#include <zlib.h>

#include <stdio.h>

#include <wx/sstream.h>
#include <wx/wfstream.h>

#include "MANFrame.h"
#include "man2html.h"


namespace
{
    int butSearchID = wxNewId();
    int butZoomInID = wxNewId();
    int butZoomOutID = wxNewId();
    int textEntryID = wxNewId();
    int htmlWindowID = wxNewId();

    const wxString NoSearchDirsSet = _("<html>\n"
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

    const wxString ManPageNotFound = _("<html>\n"
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

MANFrame::~MANFrame()
{
    if (!m_tmpfile.IsEmpty())
    {
        if (wxFileName::FileExists(m_tmpfile))
        {
            wxRemoveFile(m_tmpfile);
        }
    }
}

void MANFrame::LoadPage(const wxString &file)
{
    m_htmlWindow->LoadPage(file);
}

void MANFrame::SetPage(const wxString &contents)
{
    m_htmlWindow->SetPage(contents);
}

void MANFrame::OnLinkClicked(wxHtmlLinkEvent &event)
{
    wxString link = event.GetLinkInfo().GetHref();

    if (link.StartsWith(_T("man:"), &link))
    {
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

            SearchManPage(name);
        }
    }
    else if (link.StartsWith(_T("fman:"), &link))
    {
        wxString man_page = GetManPage(link);

        if (man_page.IsEmpty())
        {
            SetPage(ManPageNotFound);
            return;
        }

        SetPage(cbC2U(man2html_buffer(cbU2C(man_page))));
    }
    else if (wxFileName(link).GetExt().Mid(0, 3).CmpNoCase(_T("htm")) == 0)
    {
    	m_htmlWindow->LoadPage(link);
    }
}

void MANFrame::OnSearch(wxCommandEvent &/*event*/)
{
    SearchManPage(m_entry->GetValue());
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
        int read_bytes = BZ2_bzRead(&bzerror, bz, buffer, 2048);
        if (bzerror != BZ_OK && bzerror != BZ_STREAM_END)
        {
            BZ2_bzReadClose(&bzerror, bz);
            fclose(fo);
            fclose(f);
            return false;
        }
        fwrite(buffer, read_bytes, 1, fo);
    }

    BZ2_bzReadClose(&bzerror, bz);


    fclose(fo);
    fclose(f);
    return true;
}

void MANFrame::SetDirs(const wxString &dirs)
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
}

void MANFrame::GetMatches(const wxString &keyword, std::vector<wxString> *files_found)
{
    if (m_dirsVect.empty() || keyword.IsEmpty())
    {
        return;
    }

    for (std::vector<wxString>::iterator i = m_dirsVect.begin(); i != m_dirsVect.end(); ++i)
    {
        wxArrayString files;

        if (keyword.Last() == _T('*'))
        {
            wxDir::GetAllFiles(*i, &files, keyword);
        }
        else
        {
            wxDir::GetAllFiles(*i, &files, keyword + _T("*"));
        }

        for (size_t j = 0; j < files.GetCount(); ++j)
        {
            files_found->push_back(files[j]);
        }
    }
}

wxString MANFrame::GetManPage(wxString filename, int depth)
{
    wxString orgFilename = filename;

    if (filename.IsEmpty() || !wxFileName::FileExists(filename) || depth == 2)
    {
        return wxString();
    }

    wxString ret;
    if (filename.EndsWith(_T(".gz")))
    {
        gzFile f = gzopen(filename.mb_str(), "rb");
        if (!f)
        {
            return wxString();
        }

        char buffer[4096];
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
            return wxString();
        }
    }
    else
    {
        if (filename.EndsWith(_T(".bz2")))
        {
            if (!m_tmpfile.IsEmpty())
            {
                if (wxFileName::FileExists(m_tmpfile))
                {
                    wxRemoveFile(m_tmpfile);
                }
            }

            m_tmpfile = wxFileName::CreateTempFileName(_T("manbz2"));

            if (!Decompress(filename, m_tmpfile))
            {
                wxRemoveFile(m_tmpfile);
                m_tmpfile.Clear();
                return wxString();
            }

            filename = m_tmpfile;
        }

        wxStringOutputStream sos;
        wxFileInputStream f(filename);

        if (!f.IsOk())
        {
            return wxString();
        }

        f.Read(sos);
        ret = sos.GetString();
    }

    // Check if we should follow the link
    if (ret.StartsWith(_T(".so "), &ret))
    {
        wxString path = ret.BeforeFirst(_T('\n'));
        wxString name;
        wxString ext;
        wxString newfilename;

        wxFileName::SplitPath(path, 0, &name, &ext, wxPATH_UNIX); // man pages "always" use /
        newfilename = name + _T(".") + ext;
        wxFileName::SplitPath(orgFilename, &path, 0, &ext);
        newfilename = path + wxFileName::GetPathSeparator() + newfilename;

        if (ext == _T("bz2") || ext == _T("gz"))
        {
            newfilename += _T(".") + ext;
        }
        else if (!wxFileName::FileExists(newfilename))
        {
            if (wxFileName::FileExists(newfilename + wxT(".bz2")))
                newfilename += wxT(".bz2");
            else if (wxFileName::FileExists(newfilename + wxT(".gz")))
                newfilename += wxT(".gz");
        }

        return GetManPage(newfilename, depth + 1);
    }

    return ret;
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

wxString MANFrame::CreateLinksPage(const std::vector<wxString> &files)
{
    wxString ret= _("<html>\n"
        "<head>\n"
        "<meta content=\"text/html; charset=ISO-8859-1\"\n"
        "http-equiv=\"content-type\">\n"
        "<title></title>\n"
        "</head>\n"
        "<body>\n"
        "<h2>Multiple entries found</h2>\n"
        "<br>\n");

    typedef std::multimap<wxString, wxString> ResultsMap;
    ResultsMap sortedResults;

    wxRegEx reMatchLocale(wxT("^(.+)/(man.+)$"));
    for (std::vector<wxString>::const_iterator i = files.begin(); i != files.end(); ++i)
    {
        wxString filename = *i;
        wxString linkname, ext, path;

        wxFileName::SplitPath(filename, &path, &linkname, &ext);

        if (ext != _T("bz2") && ext != _T("gz"))
        {
            linkname += _T(".") + ext;
        }

        // Strip the common directory from the path, so we can detect the language of the man page.
        for (std::vector<wxString>::const_iterator dir = m_dirsVect.begin(); dir != m_dirsVect.end(); ++dir)
        {
            if (path.StartsWith(*dir))
            {
                path.Remove(0, dir->length());
                if (!path.empty() && path[0] == wxFileName::GetPathSeparator())
                    path.Remove(0, 1);
                break;
            }
        }

        // Detect the language of the man page.
        if (reMatchLocale.Matches(path))
        {
            const wxString &locale = reMatchLocale.GetMatch(path, 1);
            if (!locale.empty())
                linkname += wxT(" (") + locale + wxT(")");
        }

        const wxString &aTag = _T("<a href=\"fman:") + filename + _T("\">") + linkname + _T("</a><br>");
        sortedResults.insert(ResultsMap::value_type(linkname, aTag));
    }

    for (ResultsMap::const_iterator it = sortedResults.begin(); it != sortedResults.end(); ++it)
        ret += it->second;

    ret += _T("</body>\n"
        "</html>");

    return ret;
}

bool MANFrame::SearchManPage(const wxString &keyword)
{
    if (keyword.IsEmpty())
    {
        if (m_dirsVect.empty())
        {
            SetPage(NoSearchDirsSet);
        }

        return false;
    }

    std::vector<wxString> files_found;

    GetMatches(keyword, &files_found);
    m_entry->SetValue(keyword);

    if (files_found.empty())
    {
        SetPage(ManPageNotFound);
        return false;
    }

    if (files_found.size() == 1)
    {
        wxString filename = files_found.front();
        wxString man_page = GetManPage(filename);

        if (man_page.IsEmpty())
        {
            SetPage(ManPageNotFound);
            return false;
        }

        SetPage(cbC2U(man2html_buffer(cbU2C(man_page))));
        return true;
    }

    SetPage(CreateLinksPage(files_found));

    return true;
}
