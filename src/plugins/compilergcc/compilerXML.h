#ifndef COMPILER_XML_H
#define COMPILER_XML_H

#include <compiler.h>

#include <wx/string.h>

class CompilerXML : public Compiler
{
    public:
        CompilerXML(const wxString& name, const wxString& ID, const wxString& file);
        ~CompilerXML() override;

        AutoDetectResult AutoDetectInstallationDir() override;

    protected:
        Compiler* CreateCopy() override;

    private:

        enum SearchMode
        {
            master,
            extra,
            include,
            resource,
            lib,
            none
        };

        bool AddPath(const wxString& pth, SearchMode sm, int rmDirs = 0);

        wxString m_fileName;
};

#endif // COMPILER_XML_H
