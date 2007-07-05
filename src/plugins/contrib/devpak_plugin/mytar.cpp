#include "mytar.h"
#include <io.h>
#include <globals.h>
#include <wx/intl.h>
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(ReplacersArray);

TAR::TAR(const wxString& filename)
    : m_pFile(0),
    m_SkipBytes(0),
    m_Size(0)
{
    if (filename)
        Open(filename);
}

TAR::~TAR()
{
    Close();
}

bool TAR::Open(const wxString& filename)
{
    if (filename.IsEmpty())
        return false;
    Close();

    m_pFile = fopen(filename.mb_str(), "rb");
    if (!m_pFile)
        return false;

    fseek(m_pFile, 0, SEEK_END);
    m_Size = ftell(m_pFile);
    fseek(m_pFile, 0, SEEK_SET);

    return true;
}

void TAR::Close()
{
    if (m_pFile)
        fclose(m_pFile);
    m_pFile = 0;
    Reset();
    m_Size = 0;
}

void TAR::Reset()
{
    if (m_pFile)
        fseek(m_pFile, 0, SEEK_SET);
    m_SkipBytes = 0;
}

int TAR::OctToInt(const char* oct)
{
	int i = 0;
	if (sscanf(oct, "%o", &i) != 1)
        i = 0;
//        return 1;
    return i;
}

size_t TAR::OffsetRecords(size_t bytes)
{
    size_t i = bytes / sizeof(TAR::Header);
    if (bytes % sizeof(TAR::Header) > 0)
        ++i;
    return i;
}

bool TAR::Next(TAR::Record* rec)
{
    if (!rec)
        return false;
    rec->name.Clear();
    rec->size = 0;
    rec->pos = 0;
    if (m_SkipBytes > 0)
        fseek(m_pFile, OffsetRecords(m_SkipBytes) * sizeof(TAR::Header), SEEK_CUR);
    TAR::Header buffer;
    memset(&buffer, 0, sizeof(TAR::Header));

    // reached end of file?
    size_t pos = ftell(m_pFile);
    if (pos + sizeof(buffer) > m_Size)
        return false; // yes
    if (fread(&buffer, sizeof(buffer), 1, m_pFile) != 1)
        return false;

    rec->pos = pos;
    rec->name = cbC2U(buffer.name);
    rec->size = OctToInt(buffer.size);

#if 1
    // many DevPaks, end with a single null record...
    if (buffer.name[0] == 0)
        return false;
#else // 0
    // 2 consecutive nulls means EOT
    static bool previousWasNull = false;
    if (rec->name.IsEmpty())
    {
        rec->name.Clear();
        if (previousWasNull)
            return false; // EOT
        else
        {
            previousWasNull = true;
            m_SkipBytes = rec->size;
            return true;
        }
    }
    else
        previousWasNull = false; // reset flag
#endif // 0

    switch (buffer.typeflag)
    {
        case 0:
        case _T('0'): rec->ft = ftNormal; break;
        case _T('1'): rec->ft = ftLink; break;
        case _T('2'): rec->ft = ftSymbolicLink; break;
        case _T('3'): rec->ft = ftCharacter; break;
        case _T('4'): rec->ft = ftBlock; break;
        case _T('5'): rec->ft = ftDirectory; break;
        case _T('6'): rec->ft = ftFifo; break;
        case _T('7'): rec->ft = ftContiguous; break;
        case _T('D'): rec->ft = ftDumpDir; break;
        case _T('M'): rec->ft = ftMultiVolume; break;
        case _T('V'): rec->ft = ftVolumeHeader; break;
//        case _T('L'): rec.ft = ftLongName; break;
//        case _T('K'): rec.ft = ftLongLink; break;
        default: break;
    }

    switch (rec->ft)
    {
        case ftLink:
        case ftSymbolicLink:
        case ftDirectory:
        case ftFifo:
        case ftVolumeHeader:
            m_SkipBytes = 0;
            break;
        default:
            m_SkipBytes = rec->size;
            break;
    }
    return true;
}

bool TAR::ExtractAll(const wxString& dirname, wxString& status, wxArrayString* files)
{
    Reset();
    status.Clear();
    if (files)
        files->Clear();
    TAR::Record r;
    while (Next(&r))
    {
        wxString convertedFile;
        if (!ExtractFile(&r, dirname, status, &convertedFile))
        {
            status << _("Failed extracting") << _T(" \"") << r.name << _T("\"\n");
            return false;
        }
        if (files && !convertedFile.IsEmpty())
            files->Add(convertedFile);
    }
    return true;
}

void TAR::ClearReplacers()
{
    m_Replacers.Clear();
}

void TAR::AddReplacer(const wxString& from, const wxString& to)
{
    Replacers r;
    r.from = from;
    if (r.from.Last() != _T('/'))
        r.from << _T('/');
    r.to = to;
    r.to.Replace(_T("<app>"), _T(""));

    // avoid duplicates
    for (unsigned int i = 0; i < m_Replacers.GetCount(); ++i)
    {
        if (m_Replacers[i].from == r.from)
            return;
    }

    m_Replacers.Add(r);
}

void TAR::ReplaceThings(wxString& path)
{
    while (path.Replace(_T("\\"), _T("/")))
        ;
    for (unsigned int i = 0; i < m_Replacers.GetCount(); ++i)
        path.Replace(m_Replacers[i].from, m_Replacers[i].to);
    while (path.Replace(_T("\\"), _T("/")))
        ;
    while (path.Replace(_T("//"), _T("/")))
        ;
}

bool TAR::ExtractFile(Record* rec, const wxString& dirname, wxString& status, wxString* convertedFile)
{
    if (!rec)
        return false;

    if (convertedFile)
        convertedFile->Clear();
    wxString path;
    if (rec->name.IsEmpty())
        return true;
    if (!dirname.IsEmpty())
    {
        path << dirname << _T("/");
    }
    path << rec->name;
    ReplaceThings(path);

    switch (rec->ft)
    {
        case ftNormal:
        {
            CreateDirRecursively(path);
            status << _("Unpacking ") << path << _T('\n');
            if (convertedFile)
                *convertedFile = path;

            FILE* out = fopen(path.mb_str(), "wb");
            if (!out)
            {
                status << wxString(_("Can't open file ")) << path << _T("\n");
                return false;
            }
            if (rec->size > 0)
            {
                size_t oldpos = ftell(m_pFile);
                char* buffer = new char[rec->size];
                memset(buffer, 0, rec->size);
                if (fread(buffer, rec->size, 1, m_pFile) != 1)
                {
                    delete[] buffer;
                    fclose(out);
                    fseek(m_pFile, oldpos, SEEK_SET);
                    status << _("Failure reading file ") << path << _T("\n");
                    return false;
                }
                fwrite(buffer, rec->size, 1, out);
                delete[] buffer;
                fseek(m_pFile, oldpos, SEEK_SET);
            }
            fclose(out);
            break;
        }

        default: break;
    }

    return true;
}

TAR::Record* TAR::FindFile(const wxString& filename)
{
    if (filename.IsEmpty())
        return 0;
    Reset();
    static TAR::Record r;
    while (Next(&r))
    {
        if (r.name.CmpNoCase(filename) == 0 ||
            r.name.Matches(filename)) // support wildcards
        {
            return &r;
        }
    }
    return 0;
}
