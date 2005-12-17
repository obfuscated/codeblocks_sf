#ifndef WXSMITHWIZARD_H
#define WXSMITHWIZARD_H

#include <cbplugin.h>
#include <wx/bitmap.h>

class wxSmithWizard : public cbProjectWizardPlugin
{
    public:
        wxSmithWizard();
        
        virtual ~wxSmithWizard();
        
        virtual int Configure() { return -1; }
        
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
        
        virtual int Launch(int index);
        
    private:
        wxBitmap Bitmap;
};

#endif // WXSMITHWIZZARD_H
