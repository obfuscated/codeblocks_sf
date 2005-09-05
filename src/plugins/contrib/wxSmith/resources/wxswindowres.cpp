#include "wxswindowres.h"

#include "../wxswidgetfactory.h"
#include "../wxswindoweditor.h"
#include "../wxscodegen.h"
#include <manager.h>
#include <editormanager.h>

const wxChar* EmptySource =
_T("#include \"$(Include)\"\n")
_T("\n")
_T("BEGIN_EVENT_TABLE($(ClassName),$(BaseClassName))\n")
wxsBHeader("EventTable","$(ClassName)") _T("\n")
wxsBEnd() _T("\n")
_T("END_EVENT_TABLE()\n")
_T("\n")
_T("$(ClassName)::$(ClassName)(wxWindow* parent,wxWindowID id):\n")
_T("    $(BaseClassCtor)\n")
_T("{\n")
_T("    ") wxsBHeader("Initialize","$(ClassName)") _T("\n")
_T("    ") wxsBEnd() _T("\n")
_T("}\n")
_T("\n")
_T("$(ClassName)::~$(ClassName)()\n")
_T("{\n")
_T("}\n")
_T("\n");

const wxChar* EmptyHeader =
_T("#ifndef $(Guard)\n")
_T("#define $(Guard)\n")
_T("\n")
wxsBHeader("Headers","$(ClassName)") _T("\n")
_T("#include <wx/wx.h>\n")
wxsBEnd() _T("\n")
_T("\n")
_T("class $(ClassName): public $(BaseClassName)\n")
_T("{\n")
_T("    public:\n")
_T("\n")
_T("        $(ClassName)(wxWindow* parent,wxWindowID id = -1);\n")
_T("        virtual ~$(ClassName)();\n")
_T("\n")
_T("        ") wxsBHeader("Identifiers","$(ClassName)") _T("\n")
_T("        ") wxsBEnd() _T("\n")
_T("\n")
_T("    protected:\n")
_T("\n")
_T("        ") wxsBHeader("Handlers","$(ClassName)") _T("\n")
_T("        ") wxsBEnd() _T("\n")
_T("\n")
_T("        ") wxsBHeader("Declarations","$(ClassName)") _T("\n")
_T("        ") wxsBEnd() _T("\n")
_T("\n")
_T("    private:\n")
_T("\n")
_T("        DECLARE_EVENT_TABLE()\n")
_T("};\n")
_T("\n")
_T("#endif\n");


wxsWindowRes::wxsWindowRes(
    wxsProject* Project,
    int EditMode,
    const wxString& Class, 
    const wxString& Wxs, 
    const wxString& Src,
    const wxString& Head,
    const wxString& Xrc):
        wxsResource(Project,EditMode),
        ClassName(Class),
        WxsFile(Wxs),
        SrcFile(Src),
        HFile(Head),
        XrcFile(Xrc)

{
}

void wxsWindowRes::Initialize()
{
    RootWidget = wxsWidgetFactory::Get()->Generate(GetWidgetClass(true),this);
    if ( !RootWidget )
    {
    	wxMessageBox(GetWidgetClass(true));
    }
}

wxsWindowRes::~wxsWindowRes()
{
    EditClose();
    wxsWidgetFactory::Get()->Kill(RootWidget);
}

wxsEditor* wxsWindowRes::CreateEditor()
{
    wxsWindowEditor* Edit = new wxsWindowEditor(Manager::Get()->GetEditorManager()->GetNotebook(),this);
    Edit->BuildPreview(RootWidget);
    return Edit;
}

void wxsWindowRes::Save()
{
    TiXmlDocument* Doc = GenerateXml();
    
    if ( Doc )
    {
        Doc->SaveFile(WxsFile.mb_str());
        delete Doc;
    }
}

TiXmlDocument* wxsWindowRes::GenerateXml()
{
    TiXmlDocument* NewDoc = new TiXmlDocument;
    TiXmlElement* Resource = NewDoc->InsertEndChild(TiXmlElement("resource"))->ToElement();
    TiXmlElement* XmlDialog = Resource->InsertEndChild(TiXmlElement("object"))->ToElement();
    XmlDialog->SetAttribute("class",wxString(GetWidgetClass()).mb_str());
    XmlDialog->SetAttribute("name",ClassName.mb_str());
    if ( !RootWidget->XmlSave(XmlDialog) )
    {
        delete NewDoc;
        return NULL;
    }
    return NewDoc;
}

void wxsWindowRes::ShowPreview()
{
// TODO (SpOoN#1#): Save in temporary file
    Save();
    
    wxXmlResource Res(WxsFile);
    Res.InitAllHandlers();
    ShowResource(Res);
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
    wxString Guard;
    
    for ( int i=0; i<(int)FName.Length(); i++ )
    {
        wxChar ch = FName.GetChar(i);
        if ( ( ch < _T('A') || ch > _T('Z') ) &&
             ( ch < _T('0') || ch > _T('9') ) ) Guard.Append(_T('_'));
        else Guard.Append(ch);
    }
    
    wxFileName IncludeFN(GetProject()->GetProjectFileName(HFile));
    IncludeFN.MakeRelativeTo(
        wxFileName(GetProject()->GetProjectFileName(SrcFile)).GetPath() );
    wxString Include = IncludeFN.GetFullPath();
    

    FILE* Fl = fopen(GetProject()->GetProjectFileName(HFile).mb_str(),"wt");
    if ( !Fl ) return false;
    wxString Content = EmptyHeader;
    Content.Replace(_T("$(Guard)"),Guard,true);
    Content.Replace(_T("$(ClassName)"),ClassName,true);
    Content.Replace(_T("$(BaseClassName)"),GetWidgetClass(),true);
    fprintf(Fl,"%s",Content.mb_str());
    fclose(Fl);
    
    Fl = fopen(GetProject()->GetProjectFileName(SrcFile).mb_str(),"wt");
    if ( !Fl ) return false;
    Content = EmptySource;
    Content.Replace(_T("$(Include)"),Include,true);
    Content.Replace(_T("$(ClassName)"),ClassName,true);
    Content.Replace(_T("$(BaseClassName)"),GetWidgetClass(),true);
    Content.Replace(_T("$(BaseClassCtor)"),GetConstructor(),true);
    fprintf(Fl,"%s",(const char*)Content.mb_str());
    fclose(Fl);
    return true;
}

void wxsWindowRes::NotifyChange()
{
	// Nothing to be done when edit xrc file only
	if ( GetEditMode() == wxsResFile ) return;

    // Regenerating source code
    
	assert ( GetProject() != NULL );
	UpdateWidgetsVarNameId();
	
    // TODO (SpOoN#1#): find tab size in settings
	int TabSize = 4;
	int GlobalTabSize = 2 * TabSize;

//------------------------------
// Generating initializing code
//------------------------------
	
	wxString CodeHeader = wxString::Format(wxsBHeaderF("Initialize"),GetClassName().c_str());
	wxString Code = CodeHeader + _T("\n");
	
	// Creating local and global declarations
	wxString GlobalCode;
	bool WasDeclaration = false;
	AddDeclarationsReq(RootWidget,Code,GlobalCode,TabSize,GlobalTabSize,WasDeclaration);
	if ( WasDeclaration )
	{
		Code.Append(_T('\n'));
	}
		
	// Creating window-generating code

    if ( GetEditMode() == wxsResSource )
    {
        // Generating producing code
        wxsCodeGen Gen(RootWidget,TabSize,TabSize);
        Code.Append(Gen.GetCode());
        Code.Append(ResSetUpCode(TabSize));
        Code.Append(_T(' '),TabSize);
    }
    else if ( GetEditMode() == wxsResFile | wxsResSource )
    {
    	// Writing new Xrc file
        TiXmlDocument* Doc = GenerateXrc();
        if ( Doc )
        {
            Doc->SaveFile(GetProject()->GetProjectFileName(XrcFile).mb_str());
            delete Doc;
        }
        
        // No local variables - clearing the code
        Code = CodeHeader;
        Code.Append(_T('\n'));
        Code.Append(GetXrcLoadingCode(TabSize));
        Code.Append(_T('\n'));
        Code.Append(_T(' '),TabSize);
    	
    	// Loading all controls
    	GenXrcFetchingCode(Code,RootWidget,TabSize);
    }
	

	wxsCoder::Get()->AddCode(GetProject()->GetProjectFileName(SrcFile),CodeHeader,Code);
	
//---------------------------------
// Generating variable declarations
//---------------------------------
	
	CodeHeader.Printf(wxsBHeaderF("Declarations"),GetClassName().c_str());
	Code = CodeHeader + _T("\n") + GlobalCode;
	Code.Append(' ',GlobalTabSize);
	wxsCoder::Get()->AddCode(GetProject()->GetProjectFileName(HFile),CodeHeader,Code);
	
//---------------------------------
// Generating Identifiers
//---------------------------------

    CodeHeader.Printf(wxsBHeaderF("Identifiers"),GetClassName().c_str());
    Code = CodeHeader;
    Code.Append(_T('\n'));
    Code.Append(_T(' '),GlobalTabSize);
    if ( GetEditMode() == wxsResSource )
    {
        wxArrayString IdsArray;
        BuildIdsArray(RootWidget,IdsArray);
        Code.Append(_T("enum Identifiers\n"));
        Code.Append(_T(' '),GlobalTabSize);
        Code.Append(_T('{'));
        IdsArray.Sort();
        wxString Previous = _T("");
        bool First = true;
        for ( size_t i = 0; i<IdsArray.Count(); ++i )
        {
            if ( IdsArray[i] != Previous )
            {
                Previous = IdsArray[i];
                Code.Append( _T('\n') );
                Code.Append( _T(' '), GlobalTabSize + TabSize );
                Code.Append( Previous );
                if ( First )
                {
                    Code.Append( _T(" = 0x1000") );
                    First = false;
                }
                if ( i < IdsArray.Count() - 1 )
                {
                    Code.Append( _T(',') );
                }
            }
        }
        Code.Append( _T('\n') );
        Code.Append( _T(' '), GlobalTabSize );
        Code.Append( _T("};\n") );
        Code.Append( _T(' '), GlobalTabSize );
    }
    wxsCoder::Get()->AddCode(GetProject()->GetProjectFileName(HFile),CodeHeader,Code);
	
//---------------------------------
// Generating Includes
//---------------------------------

	wxArrayString HeadersArray;
	BuildHeadersArray(RootWidget,HeadersArray);
	HeadersArray.Add(_T("<wx/intl.h>"));
	if ( GetEditMode() == wxsResSource | wxsResFile )
	{
		HeadersArray.Add(_T("<wx/xrc/xmlres.h>"));
	}
	HeadersArray.Sort();
	CodeHeader.Printf(wxsBHeaderF("Headers"),GetClassName().c_str());
	Code = CodeHeader;
	wxString Previous = _T("");
	for ( size_t i = 0; i<HeadersArray.Count(); i++ )
	{
		if ( HeadersArray[i] != Previous )
        {
        	Previous = HeadersArray[i];
            Code.Append(_T("\n#include "));
            Code.Append(Previous);
        }
	}
	Code.Append(_T('\n'));
	wxsCoder::Get()->AddCode(GetProject()->GetProjectFileName(HFile),CodeHeader,Code);
	UpdateEventTable();
}

void wxsWindowRes::AddDeclarationsReq(wxsWidget* Widget,wxString& LocalCode,wxString& GlobalCode,int LocalTabSize,int GlobalTabSize,bool& WasLocal)
{
	static wxsCodeParams EmptyParams;
	
	if ( !Widget ) return;
	int Count = Widget->GetChildCount();
	for ( int i=0; i<Count; i++ )
	{
		wxsWidget* Child = Widget->GetChild(i);
		wxString Decl = Child->GetDeclarationCode(EmptyParams);
		if ( Decl.Length() )
		{
            bool Member = Child->GetBaseParams().IsMember;
            wxString& Code = Member ? GlobalCode : LocalCode;
            Code.Append(' ',Member ? GlobalTabSize : LocalTabSize);
            Code.Append(Decl);
            Code.Append('\n');
            WasLocal |= !Member;
		}
		AddDeclarationsReq(Child,LocalCode,GlobalCode,LocalTabSize,GlobalTabSize,WasLocal);
	}
}

void wxsWindowRes::UpdateWidgetsVarNameId()
{
    StrMap NamesMap;
    StrMap IdsMap;
    
    CreateSetsReq(NamesMap,IdsMap,RootWidget);
   	UpdateWidgetsVarNameIdReq(NamesMap,IdsMap,RootWidget);
}

void wxsWindowRes::UpdateWidgetsVarNameIdReq(StrMap& NamesMap, StrMap& IdsMap, wxsWidget* Widget)
{
	int Cnt = Widget->GetChildCount();
	for ( int i=0; i<Cnt; i++ )
	{
		wxsWidget* Child = Widget->GetChild(i);
		
        wxsWidgetBaseParams& Params = Child->GetBaseParams();
        
        if ( Params.VarName.Length() == 0 || Params.IdName.Length() == 0 )
        {
            wxString NameBase = Child->GetInfo().DefaultVarName;
            wxString Name;
            wxString IdBase = Child->GetInfo().DefaultVarName;
            IdBase.MakeUpper();
            wxString Id;
            int Index = 1;
            do
            {
                Name.Printf(_T("%s%d"),NameBase.c_str(),Index);
                Id.Printf(_T("ID_%s%d"),IdBase.c_str(),Index++);
            }
            while ( NamesMap.find(Name) != NamesMap.end() ||
                    IdsMap.find(Id)     != IdsMap.end() );
            
            Params.VarName = Name;
            NamesMap[Name] = Child;
            Params.IdName = Id;
            IdsMap[Id] = Child;
        }
    
		UpdateWidgetsVarNameIdReq(NamesMap,IdsMap,Child);
	}
}

void wxsWindowRes::CreateSetsReq(StrMap& NamesMap, StrMap& IdsMap, wxsWidget* Widget, wxsWidget* Without)
{
	int Cnt = Widget->GetChildCount();
	for ( int i=0; i<Cnt; i++ )
	{
		wxsWidget* Child = Widget->GetChild(i);
		
		if ( Child != Without )
		{
            if ( Child->GetBaseParams().VarName.Length() )
            {
                NamesMap[Child->GetBaseParams().VarName.c_str()] = Child;
            }
            
            if ( Child->GetBaseParams().VarName.Length() )
            {
                IdsMap[Child->GetBaseParams().VarName.c_str()] = Child;
            }
		}
		
		CreateSetsReq(NamesMap,IdsMap,Child,Without);
	}
}

bool wxsWindowRes::CheckBaseProperties(bool Correct,wxsWidget* Changed)
{
    StrMap NamesMap;
    StrMap IdsMap;
    
    if ( Changed == NULL )
    {
    	// Will check all widgets
    	return CheckBasePropertiesReq(RootWidget,Correct,NamesMap,IdsMap);
    }
    
    // Creating sets of names and ids
   	CreateSetsReq(NamesMap,IdsMap,RootWidget,Changed);
   	
   	// Checkign and correcting changed widget
   	return CorrectOneWidget(NamesMap,IdsMap,Changed,Correct);
}

bool wxsWindowRes::CheckBasePropertiesReq(wxsWidget* Widget,bool Correct,StrMap& NamesMap,StrMap& IdsMap)
{
	bool Result = true;
	int Cnt = Widget->GetChildCount();
	for ( int i=0; i<Cnt; ++i )
	{
		wxsWidget* Child = Widget->GetChild(i);
		
		if ( !CorrectOneWidget(NamesMap,IdsMap,Child,Correct) )
		{
			if ( !Correct ) return false;
			Result = false;
		}
		
		NamesMap[Child->GetBaseParams().VarName] = Child;
		IdsMap[Child->GetBaseParams().IdName] = Child;
		
		if ( ! CheckBasePropertiesReq(Child,Correct,NamesMap,IdsMap) )
		{
			if ( !Correct ) return false;
			Result = false;
		}
	}
	
	return Result;
}

bool wxsWindowRes::CorrectOneWidget(StrMap& NamesMap,StrMap& IdsMap,wxsWidget* Changed,bool Correct)
{
	bool Valid = true;

    // Validating variable name
	
    if ( Changed->GetBPType() & wxsWidget::bptVariable )
    {
    	wxString& VarName = Changed->GetBaseParams().VarName;
    	wxString Corrected;
    	VarName.Trim(true);
    	VarName.Trim(false);
    	
    	// first validating produced name
    	
    	if ( VarName.Length() == 0 )
    	{
    		if ( !Correct )
    		{
    			wxMessageBox(_("Item must have variable name"));
    			return false;
    		}

   			// Creating new unique name
    			
   			const wxString& Prefix = Changed->GetInfo().DefaultVarName;
   			for ( int i=1;; ++i )
   			{
   				Corrected.Printf(_T("%s%d"),Prefix.c_str(),i);
   				if ( NamesMap.find(Corrected) == NamesMap.end() ) break;
   			}

    		Valid = false;
    	}
    	else
    	{
    		// Validating name as C++ ideentifier
            if ( wxString(_T("ABCDEFGHIJKLMNOPQRSTUVWXYZ")
                          _T("abcdefghijklmnopqrstuvwxyz")
                          _T("_") ).Find(VarName.GetChar(0)) == -1 )
            {
            	if ( !Correct )
            	{
            		wxMessageBox(wxString::Format(_("Invalid character: '%c' in variable name"),VarName.GetChar(0)));
            		return false;
            	}
                Valid = false;
            }
            else
            {
            	Corrected.Append(VarName.GetChar(0));
            }
            
            for ( size_t i=1; i<VarName.Length(); ++i )
            {
                if ( wxString(_T("0123456789")
                              _T("ABCDEFGHIJKLMNOPQRSTUVWXYZ")
                              _T("abcdefghijklmnopqrstuvwxyz")
                              _T("_") ).Find(VarName.GetChar(i)) == -1 )
                {
                    if ( !Correct )
                    {
                        wxMessageBox(wxString::Format(_("Invalid character: '%c' in variable name"),VarName.GetChar(i)));
                        return false;
                    }
                    Valid = false;
                }
                else
                {
                    Corrected.Append(VarName.GetChar(i));
                }
            }

            // Searching for another widget with same name
            if ( NamesMap.find(Corrected) != NamesMap.end() )
            {
            	if ( !Correct )
            	{
            		wxMessageBox(wxString::Format(_("Item with variable name '%s' already exists"),Corrected.c_str()));
            		return false;
            	}
            	
            	// Generating new unique name

                const wxString& Prefix = Changed->GetInfo().DefaultVarName;
                for ( int i=1;; ++i )
                {
                    Corrected.Printf(_T("%s%d"),Prefix.c_str(),i);
                    if ( NamesMap.find(Corrected) == NamesMap.end() ) break;
                }

            	Valid = false;
            }
    	}
    
        if ( Correct )
        {
        	VarName = Corrected;
        }
    }
    
    if ( Changed->GetBPType() & wxsWidget::bptId )
    {
    	wxString& IdName = Changed->GetBaseParams().IdName;
    	wxString Corrected;
    	IdName.Trim(true);
    	IdName.Trim(false);
    	
    	// first validating produced name
    	
    	if ( IdName.Length() == 0 )
    	{
    		if ( !Correct )
    		{
    			wxMessageBox(_("Item must have identifier"));
    			return false;
    		}

   			// Creating new unique name
    			
   			wxString Prefix = Changed->GetInfo().DefaultVarName;
   			Prefix.UpperCase();
   			for ( int i=1;; ++i )
   			{
   				Corrected.Printf(_T("ID_%s%d"),Prefix.c_str(),i);
   				if ( IdsMap.find(Corrected) == IdsMap.end() ) break;
   			}

    		Valid = false;
    	}
    	else
    	{
    		// Validating name as C++ ideentifier
            if ( wxString(_T("ABCDEFGHIJKLMNOPQRSTUVWXYZ")
                          _T("abcdefghijklmnopqrstuvwxyz")
                          _T("_") ).Find(IdName.GetChar(0)) == -1 )
            {
            	if ( !Correct )
            	{
            		wxMessageBox(wxString::Format(_("Invalid character: '%c' in variable name"),IdName.GetChar(0)));
            		return false;
            	}
                Valid = false;
            }
            else
            {
            	Corrected.Append(IdName.GetChar(0));
            }
            
            for ( size_t i=1; i<IdName.Length(); ++i )
            {
                if ( wxString(_T("0123456789")
                              _T("ABCDEFGHIJKLMNOPQRSTUVWXYZ")
                              _T("abcdefghijklmnopqrstuvwxyz")
                              _T("_") ).Find(IdName.GetChar(i)) == -1 )
                {
                    if ( !Correct )
                    {
                        wxMessageBox(wxString::Format(_("Invalid character: '%c' in variable name"),IdName.GetChar(i)));
                        return false;
                    }
                    Valid = false;
                }
                else
                {
                    Corrected.Append(IdName.GetChar(i));
                }
            }

            // Searching for another widget with same name
            
            if ( IdsMap.find(Corrected) != IdsMap.end() && Corrected != _T("ID_COMMON") )
            {
            	if ( !Correct )
            	{
            		wxMessageBox(wxString::Format(_("Item with identifier '%s' already exists"),Corrected.c_str()));
            		return false;
            	}
            	
            	// Generating new unique name

                wxString Prefix = Changed->GetInfo().DefaultVarName;
                Prefix.UpperCase();
                for ( int i=1;; ++i )
                {
                    Corrected.Printf(_T("ID_%s%d"),Prefix.c_str(),i);
                    if ( IdsMap.find(Corrected) == IdsMap.end() ) break;
                }

            	Valid = false;
            }
    	}
    	
    	if ( Correct )
    	{
    		IdName = Corrected;
    	}
    }
    
	return Valid;
}

void wxsWindowRes::BuildIdsArray(wxsWidget* Widget,wxArrayString& Array)
{
	int Cnt = Widget->GetChildCount();
	for ( int i=0; i<Cnt; i++ )
	{
		wxsWidget* Child = Widget->GetChild(i);
		if ( Child->GetBPType() & wxsWidget::bptId )
		{
			Array.Add(Child->GetBaseParams().IdName);
		}
		BuildIdsArray(Child,Array);
	}
}

void wxsWindowRes::BuildHeadersArray(wxsWidget* Widget,wxArrayString& Array)
{
	Array.Add(Widget->GetInfo().HeaderFile);
	if ( Widget->GetInfo().ExtHeaderFile.Length() )
	{
        Array.Add(Widget->GetInfo().ExtHeaderFile);
	}
	int Cnt = Widget->GetChildCount();
	for ( int i=0; i<Cnt; i++ )
	{
		wxsWidget* Child = Widget->GetChild(i);
		BuildHeadersArray(Child,Array);
	}
}

void wxsWindowRes::UpdateEventTable()
{
	int TabSize = 4;
	wxString CodeHeader;
	CodeHeader.Printf(wxsBHeaderF("EventTable"),ClassName.c_str());
	wxString Code = CodeHeader;
	Code.Append(_T('\n'));
	CollectEventTableEnteries(Code,RootWidget,TabSize);
	wxsCoder::Get()->AddCode(GetProject()->GetProjectFileName(GetSourceFile()),CodeHeader,Code);
}

void wxsWindowRes::CollectEventTableEnteries(wxString& Code,wxsWidget* Widget,int TabSize)
{
	int Cnt = Widget->GetChildCount();
	for ( int i=0; i<Cnt; i++ )
	{
		wxsWidget* Child = Widget->GetChild(i);
		Code += Child->GetEvents()->GetArrayEnteries(TabSize);
		CollectEventTableEnteries(Code,Child,TabSize);
	}
}

void wxsWindowRes::GenXrcFetchingCode(wxString& Code,wxsWidget* Widget,int TabSize)
{
	int Cnt = Widget->GetChildCount();
	for ( int i=0; i<Cnt; i++ )
	{
		wxsWidget* Child = Widget->GetChild(i);
		if ( Child->GetBaseParams().IsMember )
		{
			Code.Append(Child->GetBaseParams().VarName);
			Code.Append(_T(" = XRCCTRL(*this,\""));
			Code.Append(Child->GetBaseParams().IdName);
			Code.Append(_T("\","));
			Code.Append(Child->GetInfo().Name);
			Code.Append(_T(");\n"));
			Code.Append(_T(' '),TabSize);
		}
		GenXrcFetchingCode(Code,Child,TabSize);
	}
}

TiXmlDocument* wxsWindowRes::GenerateXrc()
{
	int EMStore = GetEditMode();
	SetEditMode(wxsResFile);
	TiXmlDocument* Generated = GenerateXml();
	SetEditMode(EMStore);
	return Generated;
}
