#include "wxswindowres.h"

#include "../wxswidgetfactory.h"
#include "../wxswindoweditor.h"
#include "../wxscodegen.h"
#include <manager.h>
#include <wx/xrc/xmlres.h>
#include <editormanager.h>

const char* EmptySource =
"\
#include \"$(Include)\"\n\
\n\
BEGIN_EVENT_TABLE($(ClassName),$(BaseClassName))\n\
//(*EventTable($(ClassName))\n\
//*)\n\
END_EVENT_TABLE()\n\
\n\
$(ClassName)::$(ClassName)(wxWidnow* parent,wxWindowID id):\
    $(BaseClassCtor)\n\
{\n\
    //(*Initialize($(ClassName))\n\
    //*)\n\
}\n\
\n\
$(ClassName)::~$(ClassName)()\n\
{\n\
}\n\
\n\
";

const char* EmptyHeader =
"\
#ifndef $(Guard)\n\
#define $(Guard)\n\
\n\
//(*Headers($(ClassName))\n\
//*)\n\
\n\
class $(ClassName): public $(BaseClassName)\n\
{\n\
    public:\n\
\n\
        $(ClassName)(wxWidnow* parent,wxWindowID id = -1);\n\
        virtual ~$(ClassName);\n\
\n\
        //(*Identifiers($(ClassName))\n\
        //*)\n\
\n\
    protected:\n\
\n\
        //(*Handlers($(ClassName))\n\
        //*)\n\
\n\
        //(*Declarations($(ClassName))\n\
        //*)\n\
\n\
    private:\n\
\n\
        DECLARE_EVENT_TABLE()\n\
};\n\
\n\
#endif\n\
";


wxsWindowRes::wxsWindowRes(
    wxsProject* Project,
    const wxString& Class, 
    const wxString& Xrc, 
    const wxString& Src,
    const wxString& Head,
    WindowResType _Type):
        wxsResource(Project),
        ClassName(Class),
        XrcFile(Xrc),
        SrcFile(Src),
        HFile(Head),
        Type(_Type)
{
    RootWidget = wxsWidgetFactory::Get()->Generate(GetWidgetClass(true));
}

wxsWindowRes::~wxsWindowRes()
{
    EditClose();
    wxsWidgetFactory::Get()->Kill(RootWidget);
    switch ( Type )
    {
    	case Dialog:
            GetProject()->DeleteDialog(dynamic_cast<wxsDialogRes*>(this));
            break;
            
        case Frame:
            GetProject()->DeleteFrame(dynamic_cast<wxsFrameRes*>(this));
            break;
            
        case Panel:
            GetProject()->DeletePanel(dynamic_cast<wxsPanelRes*>(this));
            break;
            
        default:;
    }
}

wxsEditor* wxsWindowRes::CreateEditor()
{
    wxsWindowEditor* Edit = new wxsWindowEditor(Manager::Get()->GetEditorManager()->GetNotebook(),XrcFile,this);
    Edit->BuildPreview(RootWidget);
    return Edit;
}

void wxsWindowRes::Save()
{
    TiXmlDocument* Doc = GenerateXml();
    
    if ( Doc )
    {
        wxString FullFileName = GetProject()->GetInternalFileName(XrcFile);
        Doc->SaveFile(FullFileName);
        delete Doc;
    }
}

TiXmlDocument* wxsWindowRes::GenerateXml()
{
    TiXmlDocument* NewDoc = new TiXmlDocument;
    TiXmlElement* Resource = NewDoc->InsertEndChild(TiXmlElement("resource"))->ToElement();
    TiXmlElement* XmlDialog = Resource->InsertEndChild(TiXmlElement("object"))->ToElement();
    XmlDialog->SetAttribute("class",GetWidgetClass());
    XmlDialog->SetAttribute("name",ClassName.c_str());
    if ( !RootWidget->XmlSave(XmlDialog) )
    {
        delete NewDoc;
        return NULL;
    }
    return NewDoc;
}

void wxsWindowRes::ShowPreview()
{
    Save();
    
    wxXmlResource Res(GetProject()->GetInternalFileName(XrcFile));
    Res.InitAllHandlers();
    
    switch ( Type )
    {
    	case Dialog:
    	{
            wxDialog Dlg;
            
            if ( Res.LoadDialog(&Dlg,NULL,ClassName) )
            {
                Dlg.ShowModal();
            }
            break;
    	}
            
        case Frame:
        {
            wxFrame* Frm = new wxFrame;
            if ( Res.LoadFrame(Frm,NULL,ClassName) )
            {
            	Frm->Show();
            }
            break;
        }
            
        case Panel:
        {
        	wxDialog Dlg(NULL,-1,wxString::Format(wxT("Frame preview: %s"),ClassName.c_str()));
        	wxPanel* Panel = Res.LoadPanel(&Dlg,ClassName);
        	if ( Panel )
        	{
        		Dlg.Fit();
        		Dlg.ShowModal();
        	}
        	break;
        }
        
        default:;
    }
}

const wxString& wxsWindowRes::GetResourceName()
{
    return GetClassName();
}

bool wxsWindowRes::GenerateEmptySources()
{
    // Generating file variables
    
    wxString FName = wxFileName(HFile).GetFullName();
    FName.MakeUpper();
    wxString Guard(wxT("__"));
    
    for ( int i=0; i<(int)FName.Length(); i++ )
    {
        char ch = FName.GetChar(i);
        if ( ( ch < 'A' || ch > 'Z' ) && ( ch < '0' || ch > '9' ) ) Guard.Append('_');
        else Guard.Append(ch);
    }
    
    wxFileName IncludeFN(GetProject()->GetProjectFileName(HFile));
    IncludeFN.MakeRelativeTo(
        wxFileName(GetProject()->GetProjectFileName(SrcFile)).GetPath() );
    wxString Include = IncludeFN.GetFullPath();
    

    FILE* Fl = fopen(GetProject()->GetProjectFileName(HFile),"wt");
    if ( !Fl ) return false;
    wxString Content = EmptyHeader;
    Content.Replace("$(Guard)",Guard,true);
    Content.Replace("$(ClassName)",ClassName,true);
    Content.Replace("$(BaseClassName)",GetWidgetClass());
    switch ( Type )
    {
    	case Dialog:
            Content.Replace("$(BaseClassCtor)","wxDialog(parent,id,wxT(\"\"))");
            break;
            
        case Frame:
            Content.Replace("$(BaseClassCtor)","wxFrame(parent,id,wxT(\"\"))");
            break;
            
        case Panel:
            Content.Replace("$(BaseClassCtor)","wxPanel(parent,id)");
            break;
            
        default:;
    }
    fprintf(Fl,"%s",Content.c_str());
    fclose(Fl);
    
    Fl = fopen(GetProject()->GetProjectFileName(SrcFile),"wt");
    if ( !Fl ) return false;
    Content = EmptySource;
    Content.Replace("$(Include)",Include,true);
    Content.Replace("$(ClassName)",ClassName,true);
    Content.Replace("$(BaseClassName)",GetWidgetClass());
    fprintf(Fl,"%s",Content.c_str());
    fclose(Fl);
    return true;
}

void wxsWindowRes::NotifyChange()
{
	assert ( GetProject() != NULL );
	
	#if 1
	
	int TabSize = 4;
	int GlobalTabSize = 2 * TabSize;
	
	// Generating producing code
	wxsCodeGen Gen(RootWidget,TabSize,TabSize);
	
	// Creating code header

	wxString CodeHeader = wxString::Format(wxT("//(*Initialize(%s)"),GetClassName().c_str());
	wxString Code = CodeHeader + wxT("\n");
	
	// Creating local and global declarations
	
	wxString GlobalCode;
	bool WasDeclaration = false;
	AddDeclarationsReq(RootWidget,Code,GlobalCode,TabSize,GlobalTabSize,WasDeclaration);
	if ( WasDeclaration )
	{
		Code.Append('\n');
	}
		
	// Creating window-generating code
	
	Code.Append(Gen.GetCode());
	Code.Append(' ',TabSize);
	
// TODO (SpOoN#1#): Apply title and centered properties for frame and dialog
	

	wxsCoder::Get()->AddCode(GetProject()->GetProjectFileName(SrcFile),CodeHeader,Code);
	
	// Creating global declarations
	
	CodeHeader = wxString::Format(wxT("//(*Declarations(%s)"),GetClassName().c_str());
	Code = CodeHeader + wxT("\n") + GlobalCode;
	Code.Append(' ',GlobalTabSize);
	wxsCoder::Get()->AddCode(GetProject()->GetProjectFileName(HFile),CodeHeader,Code);
	
	#endif
}

void wxsWindowRes::AddDeclarationsReq(wxsWidget* Widget,wxString& LocalCode,wxString& GlobalCode,int LocalTabSize,int GlobalTabSize,bool& WasLocal)
{
	static wxsCodeParams EmptyParams;
	
	if ( !Widget ) return;
	int Count = Widget->GetChildCount();
	for ( int i=0; i<Count; i++ )
	{
		wxsWidget* Child = Widget->GetChild(i);
		bool Member = Child->GetBaseParams().IsMember;
		wxString& Code = Member ? GlobalCode : LocalCode;
		
		Code.Append(' ',Member ? GlobalTabSize : LocalTabSize);
		Code.Append(Child->GetDeclarationCode(EmptyParams));
		Code.Append('\n');
		
		WasLocal |= !Member;
		AddDeclarationsReq(Child,LocalCode,GlobalCode,LocalTabSize,GlobalTabSize,WasLocal);
	}
}

inline const char* wxsWindowRes::GetWidgetClass(bool UseRes)
{
	switch ( Type )
	{
		case Dialog: return "wxDialog";
		case Frame: return "wxFrame";
		case Panel: return UseRes ? "wxPanelr" : "wxPanel";
	}
	
	return "";
}
