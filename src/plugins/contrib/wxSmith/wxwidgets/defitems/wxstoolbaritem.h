#ifndef WXSTOOLBARITEM_H
#define WXSTOOLBARITEM_H

#include "../wxstool.h"

/** \brief Items inside toolbar (f.ex button) which are not wxContainer) */
class wxsToolBarItem : public wxsTool
{
    public:

        wxsToolBarItem(wxsItemResData* Data,bool IsSeparator);

    private:

        virtual void OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language);
        virtual void OnEnumToolProperties(long Flags);
        virtual void OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language) {}
        virtual bool OnIsPointer() { return true; }
        virtual bool OnCanAddToResource(wxsItemResData* Data,bool ShowMessage) { return false; }
        virtual bool OnCodefExtension(wxsCodingLang Language,wxString& Result,const wxChar* &FmtChar,va_list ap);
        virtual bool OnXmlWrite(TiXmlElement* Element,bool IsXRC,bool IsExtra);
        virtual bool OnXmlRead(TiXmlElement* Element,bool IsXRC,bool IsExtra);
        virtual bool OnCanAddToParent(wxsParent* Parent,bool ShowMessage);
        virtual void OnBuildDeclarationCode(wxString& Code,wxsCodingLang Language);
        virtual wxString OnGetTreeLabel(int& Image);

        enum Type
        {
            Separator,
            Normal,
            Radio,
            Check
        };

        Type          m_Type;
        wxString      m_Label;
        wxsBitmapData m_Bitmap;
        wxsBitmapData m_Bitmap2;
        wxString      m_ToolTip;
        wxString      m_HelpText;

        friend class wxsToolBarEditor;
};

#endif
