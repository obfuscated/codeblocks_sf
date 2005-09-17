#include "wxsnotebook.h"

#include <wx/splitter.h>
#include "../properties/wxsenumproperty.h"
#include "../wxswidgetfactory.h"

WXS_ST_BEGIN(wxsNotebookStyles)
    WXS_ST_CATEGORY("wxNotebook")
    WXS_ST(wxNB_DEFAULT)
    WXS_ST(wxNB_LEFT)
    WXS_ST(wxNB_RIGHT)
    WXS_ST(wxNB_TOP)
    WXS_ST(wxNB_BOTTOM)
    WXS_ST(wxNB_FIXEDWIDTH)
    WXS_ST(wxNB_MULTILINE)
    WXS_ST(wxNB_NOPAGETHEME)
WXS_ST_END(wxsNotebookStyles)

WXS_EV_BEGIN(wxsNotebookEvents)
    WXS_EVI(EVT_NOTEBOOK_PAGE_CHANGED,wxNotebookEvent,PageChanged)
    WXS_EVI(EVT_NOTEBOOK_PAGE_CHANGING,wxNotebookEvent,PageChanging)
WXS_EV_END(wxsNotebookEvents)

class wxsNotebookPreview: public wxNotebook
{
	public:
        wxsNotebookPreview(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, wxsNotebook* NB):
            wxNotebook(parent,id,pos,size,style),
            Notebook(NB)
        {}
        
    private:

        wxsNotebook* Notebook;
        void OnChanged(wxNotebookEvent& event)
        {
        	wxsWidget* NewSelection = Notebook->GetChild(event.GetSelection());
        	if ( NewSelection != Notebook->CurrentSelection )
        	{
        		Notebook->CurrentSelection = NewSelection;
                Notebook->PropertiesUpdated(false,false);
        	}
        }
        
        DECLARE_EVENT_TABLE()
};

wxsNotebook::wxsNotebook(wxsWidgetManager* Man,wxsWindowRes* Res):
    wxsContainer(Man,Res,true,0,propWidget),
    CurrentSelection(0)
{
}

wxsNotebook::~wxsNotebook()
{
}

int wxsNotebook::AddChild(wxsWidget* NewWidget,int InsertBeforeThis)
{
	if ( NewWidget->GetInfo().Sizer )
	{
		wxMessageBox(_("Can not add sizer into Notebook.\nAdd panels first"));
		return -1;
	}
	
	if ( NewWidget->GetInfo().Spacer )
	{
		wxMessageBox(_("Spacer can be added to sizers only"));
		return -1;
	}
	
	return wxsContainer::AddChild(NewWidget,InsertBeforeThis);
}

wxWindow* wxsNotebook::MyCreatePreview(wxWindow* Parent)
{
	return new wxNotebook(Parent,-1,GetPosition(),GetSize(),GetStyle());
}

void wxsNotebook::MyFinalUpdatePreview(wxWindow* Preview)
{
	wxNotebook* Wnd = (wxNotebook*)Preview;
	for ( int i=0; i<GetChildCount(); ++i )
	{
		wxsWidget* Child = GetChild(i);
		wxsNotebookExtraParams* Params = GetExtraParams(i);
		Wnd->AddPage(Child->GetPreview(),Params->Label,Child==CurrentSelection);
	}
}

wxString wxsNotebook::GetProducingCode(wxsCodeParams& Params)
{
	const CodeDefines& CD = GetCodeDefines();
	return wxString::Format(
        _T("%s = new wxNotebook(%s,%s,%s,%s,%s);"),
        BaseParams.VarName.c_str(),
        Params.ParentName.c_str(),
        BaseParams.IdName.c_str(),
        CD.Pos.c_str(),
        CD.Size.c_str(),
        CD.Style.c_str());
}

wxString wxsNotebook::GetFinalizingCode(wxsCodeParams& Params)
{
	wxString Code;
	for ( int i=0; i<GetChildCount(); ++i )
	{
		wxsWidget* Child = GetChild(i);
		wxsNotebookExtraParams* Params = GetExtraParams(i);
		Code += wxString::Format(
            _T("%s->AddPage(%s,%s,%s);\n"),
                BaseParams.VarName.c_str(),
                Child->GetBaseParams().VarName.c_str(),
                GetWxString(Params->Label).c_str(),
                Params->Selected ? _T("true") : _T("false"));
	}
	return Code;
}

wxString wxsNotebook::GetDeclarationCode(wxsCodeParams& Params)
{
	return wxString::Format(_T("wxNotebook* %s;"),BaseParams.VarName.c_str());
}

bool wxsNotebook::XmlLoadChild(TiXmlElement* Element)
{
	if ( strcmp(Element->Value(),"object") ) return true;
	
	bool Ret = true;
	TiXmlElement* RealObject = Element;
	
    const char* Class = Element->Attribute("class");
    if ( Class && !strcmp(Class,"notebookpage") )
    {
        RealObject = Element->FirstChildElement("object");
    }
	
	if ( !RealObject ) return false;
	
    const char* Name = RealObject->Attribute("class");
    
    if ( !Name || !*Name ) return false;
    
    wxsWidget* Child = wxsWidgetFactory::Get()->Generate(wxString(Name,wxConvUTF8),GetResource());
    if ( !Child ) return false;
    
    if ( !Child->XmlLoad(RealObject) ) Ret = false;
    int Index = AddChild(Child);
    if ( Index < 0 ) 
    {
        delete Child;
        return false;
    }
    
    wxsNotebookExtraParams* Params = GetExtraParams(Index);
    TiXmlElement* Store = XmlElem();
    XmlAssignElement(Element);
    Params->Label = XmlGetVariable(_T("label"));
    Params->Selected = XmlGetInteger(_T("selected"),0) != 0;
    XmlAssignElement(Store);
    
    if (Index == 0 || Params->Selected) CurrentSelection = Child;
    
    return Ret;
}

bool wxsNotebook::XmlSaveChild(int ChildIndex,TiXmlElement* AddHere)
{
	bool Ret = true;
	TiXmlElement* NotebookPage = AddHere->InsertEndChild(TiXmlElement("object"))->ToElement();
	NotebookPage->SetAttribute("class","notebookpage");
	
    wxsNotebookExtraParams* Params = GetExtraParams(ChildIndex);
    TiXmlElement* Store = XmlElem();
    XmlAssignElement(NotebookPage);
    XmlSetVariable(_T("label"),Params->Label);
    if ( Params->Selected ) XmlSetInteger(_T("selected"),1);
    XmlAssignElement(Store);
    
    return wxsWidget::XmlSaveChild(ChildIndex,NotebookPage) && Ret;
}

void wxsNotebook::AddChildProperties(int ChildIndex)
{
	wxsWidget* Widget = GetChild(ChildIndex);
	wxsNotebookExtraParams* Params = GetExtraParams(ChildIndex);
	if ( !Widget || !Params ) return;
	
    Widget->GetPropertiesObj().AddProperty(_("Notebook page:"),Params->Label,0);
    Widget->GetPropertiesObj().AddProperty(_(" Page selected:"),Params->Selected,1);
}

void wxsNotebook::PreviewMouseEvent(wxMouseEvent& event)
{
// FIXME (SpOoN#1#): Couldn't get it working :/
	if ( GetPreview() ) GetPreview()->ProcessEvent(event);
}

void wxsNotebook::EnsurePreviewVisible(wxsWidget* Child)
{
	CurrentSelection = NULL;
	for ( int i=0; i<GetChildCount(); ++i )
	{
		if ( GetChild(i) == Child )
		{
			CurrentSelection = Child;
			if ( GetPreview() )
			{
				((wxNotebook*)GetPreview())->SetSelection(i);
			}
		}
	}
	
	wxsWidget::EnsurePreviewVisible(Child);
}
