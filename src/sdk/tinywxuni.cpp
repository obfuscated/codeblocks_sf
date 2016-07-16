#include "sdk_precomp.h"

#ifndef CB_PRECOMP
    #include <wx/file.h>
    #include <wx/string.h>
    #include "filemanager.h"
    #include "manager.h"
#endif

#include "tinywxuni.h"
#include <tinyxml.h>

bool TinyXML::LoadDocument(const wxString& filename, TiXmlDocument *doc)
{

    if (!doc || !wxFile::Access(filename, wxFile::read))
        return false;

    wxFile file(filename);
    size_t len = file.Length();

    if (!len)
		return false;

    char *input = new char[len+1];
    input[len] = '\0';
    file.Read(input, len);

    doc->Parse(input);
    delete[] input;
    return true;
}

TiXmlDocument* TinyXML::LoadDocument(const wxString& filename)
{
    TiXmlDocument* doc = new TiXmlDocument();

    if (TinyXML::LoadDocument(filename, doc))
        return doc;

    delete doc;
    return 0;
}

bool TinyXML::SaveDocument(const wxString& filename, TiXmlDocument* doc)
{
    if (!doc)
        return false;

    TiXmlPrinter printer;
    printer.SetIndent("\t");
    doc->Accept(&printer);

    return Manager::Get()->GetFileManager()->SaveUTF8(filename, printer.CStr(), printer.Size());
}

