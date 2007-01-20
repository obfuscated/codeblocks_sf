#ifndef WXSFRAMERES_H
#define WXSFRAMERES_H

#include "wxsitemres.h"

/** \brief Class responsible for managing frame resource */
class wxsFrameRes: public wxsItemRes
{
    public:
        wxsFrameRes(wxsProject* Owner): wxsItemRes(Owner,ResType,true) {}
        wxsFrameRes(const wxString& FileName,TiXmlElement* Object): wxsItemRes(FileName,Object,ResType) {}
    private:
        virtual wxString OnGetAppBuildingCode();
        virtual wxWindow* OnBuildExactPreview(wxWindow* Parent,wxsItemResData* Data);
        static const wxString ResType;
};

#endif
