#ifndef WXSPANELRES_H
#define WXSPANELRES_H

#include "wxsitemres.h"

/** \brief Class responsible for managing panel resource */
class wxsPanelRes: public wxsItemRes
{
    public:
        wxsPanelRes(wxsProject* Owner): wxsItemRes(Owner,ResType,false) {}
        wxsPanelRes(const wxString& FileName,TiXmlElement* Object): wxsItemRes(FileName,Object,ResType) {}
    private:
        virtual wxString OnGetAppBuildingCode() { return wxEmptyString; }
        virtual wxWindow* OnBuildExactPreview(wxWindow* Parent,wxsItemResData* Data);
        static const wxString ResType;
};

#endif
