#ifndef WXSSTDDIALOGBUTTONSIZER_H
#define WXSSTDDIALOGBUTTONSIZER_H

#include "../wxsitem.h"

class wxsStdDialogButtonSizer: public wxsItem
{
    public:

        wxsStdDialogButtonSizer(wxsItemResData* Data);

    private:

        virtual long OnGetPropertiesFlags();
        virtual void OnEnumItemProperties(long Flags);
        virtual wxObject* OnBuildPreview(wxWindow* Parent,long PreviewFlags);
        virtual void OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language);
        virtual void OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language);
        virtual bool OnXmlRead(TiXmlElement* Element,bool IsXRC,bool IsExtra);
        virtual bool OnXmlWrite(TiXmlElement* Element,bool IsXRC,bool IsExtra);
        virtual void OnAddExtraProperties(wxsPropertyGridManager* Grid);
        virtual void OnExtraPropertyChanged(wxsPropertyGridManager* Grid,wxPGId Id);

        static const int NumButtons = 8;

        bool     m_Use    [NumButtons];
        wxPGId   m_UseId  [NumButtons];
        wxString m_Label  [NumButtons];
        wxPGId   m_LabelId[NumButtons];

};

#endif
