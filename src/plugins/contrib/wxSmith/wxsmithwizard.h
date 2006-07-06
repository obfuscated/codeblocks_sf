#ifndef WXSMITHWIZARD_H
#define WXSMITHWIZARD_H

#include <cbplugin.h>
#include <compiletargetbase.h>
#include <wx/bitmap.h>

class wxSmithWizard : public cbWizardPlugin
{
    public:
        wxSmithWizard();

        virtual ~wxSmithWizard();

        virtual int GetCount() const
        { return 1; }

        virtual wxString GetTitle(int index) const
        { return _("wxSmith project"); }

        virtual wxString GetDescription(int index) const
        { return _("Creates project using wxSmith RAD editor (library required - wxWidgets 2.6)"); }

        virtual wxString GetCategory(int index) const
        { return _("GUI"); }

        virtual const wxBitmap& GetBitmap(int index) const
        { return Bitmap; }

        virtual CompileTargetBase* Launch(int index, wxString* pFilename = 0);

        virtual wxString GetScriptFilename(int index) const
        { return wxEmptyString; }

        virtual TemplateOutputType GetOutputType(int index) const
        { return totProject; }
    private:
        wxBitmap Bitmap;
};

#endif // WXSMITHWIZZARD_H
