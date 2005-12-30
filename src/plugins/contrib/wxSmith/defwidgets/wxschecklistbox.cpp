#include "../wxsheaders.h"
#include "wxschecklistbox.h"

#include <wx/checklst.h>

WXS_ST_BEGIN(wxsCheckListBoxStyles)
    WXS_ST_CATEGORY("wxCheckListBox")
    WXS_ST_MASK(wxLB_HSCROLL,wxsSFWin,0,true) // Windows ONLY
    WXS_ST(wxLB_SINGLE)
    WXS_ST(wxLB_MULTIPLE)
    WXS_ST(wxLB_EXTENDED)
    WXS_ST(wxLB_ALWAYS_SB)
    WXS_ST(wxLB_NEEDED_SB)
    WXS_ST(wxLB_SORT)
WXS_ST_END(wxsCheckListBoxStyles)

WXS_EV_BEGIN(wxsCheckListBoxEvents)
    WXS_EVI(EVT_CHECKLISTBOX,wxCommandEvent,Toggled)
    WXS_EV_DEFAULTS()
WXS_EV_END(wxsCheckListBoxEvents)

wxsDWDefineBegin(wxsCheckListBoxBase,wxCheckListBox,
        WXS_THIS = new wxCheckListBox(WXS_PARENT,WXS_ID,WXS_POS,WXS_SIZE,0,0,WXS_STYLE);
        wxsDWAddStrings(arrayChoices,WXS_THIS);
    )
wxsDWDefineEnd()

wxString wxsCheckListBox::GetProducingCode(wxsCodeParams& Params)
{
    wxString Code;
    const CodeDefines& CDefs = GetCodeDefines();
    Code.Printf(_T("%s = new wxCheckListBox(%s,%s,%s,%s,0,NULL,%s);\n"),
            GetBaseProperties().VarName.c_str(),
            Params.ParentName.c_str(),
            GetBaseProperties().IdName.c_str(),
            CDefs.Pos.c_str(),
            CDefs.Size.c_str(),
            CDefs.Style.c_str());
    for ( size_t i = 0; i < arrayChoices.Count(); ++i )
    {
        Code << wxString::Format(_T("%s->Append(%s);\n"),
            GetBaseProperties().VarName.c_str(),
            wxsGetWxString(arrayChoices[i]).c_str());
    }

    for ( size_t i = 0; i < arrayChecks.Count(); ++i )
    {
        if ( arrayChecks[i] )
        {
            Code << wxString::Format(_T("%s->Check(%d,true);\n"),
                GetBaseProperties().VarName.c_str(), i);
        }
    }
    Code << CDefs.InitCode;
    return Code;
}

wxWindow* wxsCheckListBox::MyCreatePreview(wxWindow* Parent)
{
    wxCheckListBox* Wnd = new wxCheckListBox(
        Parent,-1L,GetPosition(),GetSize(),0,NULL,GetStyle());
        
    for ( size_t i = 0; i < arrayChoices.Count(); ++i )
    {
        Wnd->Append(arrayChoices[i]);
    }
        
    for ( size_t i = 0; i < arrayChecks.Count(); ++i )
    {
        if ( arrayChecks[i] )
        {
            Wnd->Check(i,true);
        }
    }
    return Wnd;
}

bool wxsCheckListBox::MyXmlLoad()
{
    TiXmlElement* Content = XmlElem()->FirstChildElement("content");
    if ( !Content ) return true;
    
    for ( TiXmlElement* Item = Content->FirstChildElement("item");
          Item;
          Item = Item->NextSiblingElement("item") )
    {
        bool Checked = _U(Item->Attribute("checked")) == _T("1");
        wxString Text;
        for ( TiXmlNode* FindText = Item->FirstChild(); FindText; FindText = FindText->NextSibling() )
        {
            TiXmlText* TextNode = FindText->ToText();
            if ( TextNode )
            {
                Text = _U(TextNode->Value());
                break;
            }
        }
        
        arrayChoices.Add(Text);
        arrayChecks.Add(Checked);
    }
    
    return true;
}

bool wxsCheckListBox::MyXmlSave()
{
    TiXmlElement* Elem = XmlElem()->InsertEndChild(TiXmlElement("content"))->ToElement();
    if ( !Elem ) return false;
    
    for ( size_t i = 0; i<arrayChoices.Count(); ++i )
    {
        TiXmlElement* Item = Elem->InsertEndChild(TiXmlElement("item"))->ToElement();
        if ( !Item ) return false;
        Item->SetAttribute("checked",arrayChecks[i]?"1":"0");
        Item->InsertEndChild(TiXmlText(arrayChoices[i].mb_str()));
    }
    
    return true;
}

void wxsCheckListBox::CreateObjectProperties()
{
    PropertiesObject.AddProperty(_("Content:"),new wxsStringListCheckProperty(&PropertiesObject,arrayChoices,arrayChecks,wxLB_SORT));
    wxsWidget::CreateObjectProperties();
}
