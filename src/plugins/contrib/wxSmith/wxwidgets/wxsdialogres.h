#ifndef WXSDIALOGRES_H
#define WXSDIALOGRES_H

#include "wxsitemres.h"

/** \brief Class responsible for managing dialog resource */
class wxsDialogRes: public wxsItemRes
{
    public:
        wxsDialogRes(wxsProject* Owner): wxsItemRes(Owner,ResType,true) {}
        wxsDialogRes(const wxString& FileName,TiXmlElement* Object): wxsItemRes(FileName,Object,ResType) {}
    private:
        virtual wxString OnGetAppBuildingCode();
        virtual wxWindow* OnBuildExactPreview(wxWindow* Parent,wxsItemResData* Data);
        static const wxString ResType;
};

#endif
