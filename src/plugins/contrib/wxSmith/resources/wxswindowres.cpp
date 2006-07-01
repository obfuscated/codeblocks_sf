#include "../wxsheaders.h"
#include "wxswindowres.h"

#include "../wxswidgetfactory.h"
#include "../wxswindoweditor.h"
#include "../wxscodegen.h"
#include "../wxsmith.h"
#include "../wxswinundobuffer.h"
#include "../wxspredefinedids.h"
#include "../wxsextresmanager.h"
#include "../wxscustomwidgetxmlhandler.h"
#include <manager.h>
#include <editormanager.h>
#include <wxFlatNotebook.h>

const wxChar* EmptySource =
_T("#include \"$(Include)\"\n")
_T("\n")
_T("BEGIN_EVENT_TABLE($(ClassName),$(BaseClassName))\n")
_T("\t") wxsBHeader("EventTable","$(ClassName)") _T("\n")
_T("\t") wxsBEnd() _T("\n")
_T("END_EVENT_TABLE()\n")
_T("\n")
_T("$(ClassName)::$(ClassName)(wxWindow* parent,wxWindowID id)\n")
_T("{\n")
_T("\t") wxsBHeader("Initialize","$(ClassName)") _T("\n")
_T("\t") wxsBEnd() _T("\n")
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
_T("#include <wx/wxprec.h>\n")
_T("\n")
_T("#ifdef __BORLANDC__\n")
_T("    #pragma hdrstop\n")
_T("#endif\n")
_T("\n")
wxsBHeader("Headers","$(ClassName)") _T("\n")
wxsBEnd() _T("\n")
_T("\n")
_T("class $(ClassName): public $(BaseClassName)\n")
_T("{\n")
_T("\tpublic:\n")
_T("\n")
_T("\t\t$(ClassName)(wxWindow* parent,wxWindowID id = -1);\n")
_T("\t\tvirtual ~$(ClassName)();\n")
_T("\n")
_T("\t\t") wxsBHeader("Identifiers","$(ClassName)") _T("\n")
_T("\t\t") wxsBEnd() _T("\n")
_T("\n")
_T("\tprotected:\n")
_T("\n")
_T("\t\t") wxsBHeader("Handlers","$(ClassName)") _T("\n")
_T("\t\t") wxsBEnd() _T("\n")
_T("\n")
_T("\t\t") wxsBHeader("Declarations","$(ClassName)") _T("\n")
_T("\t\t") wxsBEnd() _T("\n")
_T("\n")
_T("\tprivate:\n")
_T("\n")
_T("\t\tDECLARE_EVENT_TABLE()\n")
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
        Preview(NULL),
        ClassName(Class),
        WxsFile(Wxs),
        SrcFile(Src),
        HFile(Head),
        XrcFile(Xrc),
        RootWidget(NULL),
        Modified(false)
{
}

wxsWindowRes::wxsWindowRes(const wxString& Class,const wxString& FileName):
    wxsResource(NULL,wxsREMFile),
    Preview(NULL),
    ClassName(Class),
    WxsFile(FileName),
    SrcFile(_T("")),
    HFile(_T("")),
    XrcFile(FileName),
    RootWidget(NULL),
    Modified(false)
{
}

void wxsWindowRes::Initialize()
{
	Clear();
}

wxsWindowRes::~wxsWindowRes()
{
    EditClose();
    wxsFACTORY()->Kill(RootWidget);
}

wxsEditor* wxsWindowRes::CreateEditor()
{
    Load();
	RootWidget->BuildTree(wxsTREE(),GetTreeItemId());
    wxsWindowEditor* Edit = new wxsWindowEditor(Manager::Get()->GetEditorManager()->GetNotebook(),this);
    return Edit;
}

void wxsWindowRes::Clear()
{
    wxsBlockSelectEvents();
	if ( RootWidget )
	{
        wxsFACTORY()->Kill(RootWidget);
		RootWidget = NULL;
	}

    RootWidget = wxsGEN(GetWidgetClass(true),this);
    if ( !RootWidget )
    {
    	wxMessageBox(_("Internal error in plugin: wxSmith.\nCode::Blocks may crash !!!\nPlease, save all Your files, close Code::Blocks and reinstall/remove wxSmith plugin"));
    }
    wxsBlockSelectEvents(false);
}

bool wxsWindowRes::Load()
{
    if ( GetEditor() )
    {
        DBGLOG(_("wxSmith ERROR !!! Can not load resource when editor is opened"));
        return false;
    }

	Clear();

    TiXmlDocument Doc(cbU2C(WxsFile));
    TiXmlElement* Resource;

    if ( !  Doc.LoadFile() ||
         ! (Resource = Doc.FirstChildElement("resource")) )
    {
        Manager::Get()->GetMessageManager()->Log(_("Couldn't load resource data"));
        return false;
    }

    /* Finding dialog object */

    TiXmlElement* XmlWindow = Resource->FirstChildElement("object");
    while ( XmlWindow )
    {
    	wxString TypeName = GetWidgetClass(false);
        if ( !strcmp(XmlWindow->Attribute("class"),cbU2C(TypeName)) &&
             !strcmp(XmlWindow->Attribute("name"),cbU2C(ClassName)) )
        {
            break;
        }

        XmlWindow = XmlWindow->NextSiblingElement("object");
    }

    if ( !XmlWindow ) return false;

    if ( !GetRootWidget()->XmlLoad(XmlWindow) )
    {
        Manager::Get()->GetMessageManager()->Log(_("Couldn't load xrc data, some resources may be damaged"));
        return false;
    }

    // Clearing modified flag

    Modified = false;

    return true;
}

void wxsWindowRes::Save()
{
//    if ( !GetEditor() )
//    {
//        DBGLOG(_("wxSmith ERROR !!! Resource can be saved only when editor is opened"));
//    }
//
    TiXmlDocument* Doc = GenerateXml();

    if ( Doc )
    {
        Doc->SaveFile(cbU2C(WxsFile));
        delete Doc;
    }

    if ( GetEditor() ) GetEditor()->SetModified(false);
}

TiXmlDocument* wxsWindowRes::GenerateXml()
{
    TiXmlDocument* NewDoc = new TiXmlDocument;
    TiXmlElement* Resource = NewDoc->InsertEndChild(TiXmlElement("resource"))->ToElement();
    TiXmlElement* XmlDialog = Resource->InsertEndChild(TiXmlElement("object"))->ToElement();
    XmlDialog->SetAttribute("class",cbU2C(GetWidgetClass()));
    XmlDialog->SetAttribute("name",cbU2C(ClassName));
    if ( !RootWidget->XmlSave(XmlDialog) )
    {
        delete NewDoc;
        return NULL;
    }
    return NewDoc;
}

void wxsWindowRes::ShowPreview()
{
    if ( Preview ) return;
// TODO (SpOoN#1#): Save in temporary file
    Save();

    wxXmlResource Res(WxsFile);
    Res.InitAllHandlers();
    Res.AddHandler(new wxsCustomWidgetXmlHandler());
    ShowResource(Res);
}

void wxsWindowRes::HidePreview()
{
    if ( !Preview ) return;
    delete Preview;
    Preview = NULL;
}

bool wxsWindowRes::IsPreview()
{
    return Preview != NULL;
}

const wxString& wxsWindowRes::GetResourceName()
{
    return GetClassName();
}

bool wxsWindowRes::GenerateEmptySources(bool Header,bool Source)
{
    if ( !GetProject() ) return false;

    // Generating file variables

    if ( Header )
    {
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

        FILE* Fl = fopen(cbU2C(GetProject()->GetProjectFileName(HFile)),"wt");
        if ( !Fl ) return false;
        wxString Content = EmptyHeader;
        Content.Replace(_T("$(Guard)"),Guard,true);
        Content.Replace(_T("$(ClassName)"),ClassName,true);
        Content.Replace(_T("$(BaseClassName)"),GetWidgetClass(),true);
        fprintf(Fl,"%s",(const char*)cbU2C(Content));
        fclose(Fl);
    }

    if ( Source )
    {
        wxFileName IncludeFN(GetProject()->GetProjectFileName(HFile));
        IncludeFN.MakeRelativeTo(
            wxFileName(GetProject()->GetProjectFileName(SrcFile)).GetPath() );
        wxString Include = IncludeFN.GetFullPath();

        FILE* Fl = fopen(cbU2C(GetProject()->GetProjectFileName(SrcFile)),"wt");
        if ( !Fl ) return false;
        wxString Content = EmptySource;
        Content.Replace(_T("$(Include)"),Include,true);
        Content.Replace(_T("$(ClassName)"),ClassName,true);
        Content.Replace(_T("$(BaseClassName)"),GetWidgetClass(),true);
        fprintf(Fl,"%s",(const char*)cbU2C(Content));
        fclose(Fl);
    }
    return true;
}

void wxsWindowRes::NotifyChange()
{
    // Regenerating source code
	UpdateWidgetsVarNameId();
	RebuildCode();

    // Applying modified state
    if ( GetEditor() )
    {
    	// Must process inside editor (updating titile)
    	GetEditor()->SetModified();
    }
    else
    {
        SetModified();
    }

    // Storing change inside undo buffer

    if ( GetEditor() )
    {
    	((wxsWindowEditor*)GetEditor())->GetUndoBuff()->StoreChange();
    }
}

void wxsWindowRes::RebuildCode()
{
    if ( !GetProject() ) return;

//------------------------------
// Generating initializing code
//------------------------------

	wxString CodeHeader = wxString::Format(wxsBHeaderF("Initialize"),GetClassName().c_str());
	wxString Code = CodeHeader + _T("\n");

	// Creating local and global declarations
	wxString GlobalCode;
	bool WasDeclaration = false;
	AddDeclarationsReq(RootWidget,Code,GlobalCode,WasDeclaration);
	if ( WasDeclaration )
	{
		Code.Append(_T('\n'));
	}

	// Creating window-generating code

    if ( GetEditMode() == wxsREMSource )
    {
        // Generating producing code
        wxsCodeGen Gen(RootWidget,false);
        Code.Append(Gen.GetCode());
    }
    else if ( GetEditMode() == wxsREMMixed )
    {
    	// Writing new Xrc file
        TiXmlDocument* Doc = GenerateXrc();
        if ( Doc )
        {
            Doc->SaveFile(cbU2C(GetProject()->GetProjectFileName(XrcFile)));
            delete Doc;
        }

        // No local variables - clearing the code
        Code = CodeHeader;
        Code.Append(_T('\n'));
        Code.Append(GetXrcLoadingCode());
        Code.Append(_T('\n'));

    	// Loading all controls
    	GenXrcFetchingCode(Code,RootWidget);
    }

	wxsCoder::Get()->AddCode(GetProject()->GetProjectFileName(SrcFile),CodeHeader,Code);

//---------------------------------
// Generating variable declarations
//---------------------------------

	CodeHeader.Printf(wxsBHeaderF("Declarations"),GetClassName().c_str());
	Code = CodeHeader + _T("\n") + GlobalCode;
	wxsCoder::Get()->AddCode(GetProject()->GetProjectFileName(HFile),CodeHeader,Code);

//---------------------------------
// Generating Identifiers
//---------------------------------

    CodeHeader.Printf(wxsBHeaderF("Identifiers"),GetClassName().c_str());
    Code = CodeHeader;
    Code.Append(_T('\n'));
    if ( GetEditMode() == wxsREMSource )
    {
        wxArrayString IdsArray;
        BuildIdsArray(RootWidget,IdsArray);
        if ( IdsArray.Count() )
        {
            Code.Append(_T("enum Identifiers\n{"));
            IdsArray.Sort();
            wxString Previous = _T("");
            bool First = true;
            for ( size_t i = 0; i<IdsArray.Count(); ++i )
            {
                if ( IdsArray[i] != Previous )
                {
                    Previous = IdsArray[i];
                    Code.Append( _T("\n\t") );
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
            Code.Append( _T("\n};\n") );
        }
    }
    wxsCoder::Get()->AddCode(GetProject()->GetProjectFileName(HFile),CodeHeader,Code);

//---------------------------------
// Generating Includes
//---------------------------------

	wxArrayString HeadersArray;
	BuildHeadersArray(RootWidget,HeadersArray);
	HeadersArray.Add(_T("<wx/intl.h>"));
	if ( GetEditMode() == wxsREMMixed )
	{
		HeadersArray.Add(_T("<wx/xrc/xmlres.h>"));
	}
	HeadersArray.Add(_T("<wx/settings.h>"));
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
	UpdateEventTable(false);
}

void wxsWindowRes::AddDeclarationsReq(wxsWidget* Widget,wxString& LocalCode,wxString& GlobalCode,bool& WasLocal)
{
	if ( !Widget ) return;

	static wxsCodeParams Params;

	int Count = Widget->GetChildCount();
	for ( int i=0; i<Count; i++ )
	{
		wxsWidget* Child = Widget->GetChild(i);
		if ( Child->GetBPType() & bptVariable )
        {
		    // Only VarName should be used when fetching declaration code
            Params.VarName = Child->BaseProperties.VarName;
            wxString Decl = Child->GetDeclarationCode(Params);
            if ( Decl.Length() )
            {
                bool Member = Child->BaseProperties.IsMember;
                wxString& Code = Member ? GlobalCode : LocalCode;
                Code.Append(Decl);
                Code.Append('\n');
                WasLocal |= !Member;
            }
		}
		AddDeclarationsReq(Child,LocalCode,GlobalCode,WasLocal);
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

        wxsBaseProperties& Params = Child->BaseProperties;

        bool UpdateVar = ( Child->GetBPType() & bptVariable ) &&
                         ( Params.VarName.Length() == 0 );
        bool UpdateId  = ( Child->GetBPType() & bptId ) &&
                         ( Params.IdName.Length() == 0 );
        if ( UpdateVar || UpdateId )
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
            while ( ( UpdateVar && NamesMap.find(Name) != NamesMap.end() ) ||
                    ( UpdateId  && IdsMap.find(Id)     != IdsMap.end() ) );

            if ( UpdateVar )
            {
                Params.VarName = Name;
                NamesMap[Name] = Child;
            }
            if ( UpdateId )
            {
                Params.IdName = Id;
                IdsMap[Id] = Child;
            }
            if ( UpdateVar || UpdateId )
            {
            	SetModified();
            }
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
            if ( Child->BaseProperties.VarName.Length() )
            {
                NamesMap[Child->BaseProperties.VarName.c_str()] = Child;
            }

            if ( Child->BaseProperties.IdName.Length() )
            {
                IdsMap[Child->BaseProperties.IdName.c_str()] = Child;
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

		NamesMap[Child->BaseProperties.VarName] = Child;
		IdsMap[Child->BaseProperties.IdName] = Child;

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

    if ( Changed->GetBPType() & bptVariable )
    {
    	wxString& VarName = Changed->BaseProperties.VarName;
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
    		else
    		{
    		    DBGLOG(_T("wxSmith: Widget has empty variable name"));
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
            	else
            	{
            	    DBGLOG(_T("wxSmith: Variable name : \"%s\" is not a valid c++ identifier (invalid character \"%c\" at position %d)"),VarName.c_str(),VarName.GetChar(0),0);
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
                    else
                    {
                        DBGLOG(_T("wxSmith: Variable name : \"%s\" is not a valid c++ identifier (invalid character \"%c\" at position %d)"),VarName.c_str(),VarName.GetChar(i),i);
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
            	else
            	{
            	    DBGLOG(_T("wxSmith: Duplicated variable name: \"%s\""),VarName.c_str());
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

    if ( Changed->GetBPType() & bptId )
    {
    	wxString& IdName = Changed->BaseProperties.IdName;
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
    		else
    		{
    		    DBGLOG(_T("wxSmith: Empty identifier"));
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
    	    long IdValue;
    	    if ( !IdName.ToLong(&IdValue,0) )
    	    {
                // Validating id as C++ identifier
                if ( wxString(_T("ABCDEFGHIJKLMNOPQRSTUVWXYZ")
                              _T("abcdefghijklmnopqrstuvwxyz")
                              _T("_") ).Find(IdName.GetChar(0)) == -1 )
                {
                    if ( !Correct )
                    {
                        wxMessageBox(wxString::Format(_("Invalid character: '%c' in id name"),IdName.GetChar(0)));
                        return false;
                    }
                    else
                    {
                        DBGLOG(_T("wxSmith: Identifier name : \"%s\" is not a valid c++ name (invalid character \"%c\" at position %d)"),IdName.c_str(),IdName.GetChar(0),0);
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
                            wxMessageBox(wxString::Format(_("Invalid character: '%c' in id name"),IdName.GetChar(i)));
                            return false;
                        }
                        else
                        {
                            DBGLOG(_T("wxSmith: Identifier name : \"%s\" is not a valid c++ name (invalid character \"%c\" at position %d)"),IdName.c_str(),IdName.GetChar(i),i);
                        }
                        Valid = false;
                    }
                    else
                    {
                        Corrected.Append(IdName.GetChar(i));
                    }
                }

                // Searching for another widget with same name

                bool Predefined = false;
                for ( int i=0; i<wxsPredefinedIdsCount; i++ )
                {
                    if ( wxsPredefinedIds[i] == Corrected )
                    {
                        Predefined = true;
                        break;
                    }
                }

                if ( (!Predefined) &&
                     ( IdsMap.find(Corrected) != IdsMap.end() ) )
                {
                    if ( !Correct )
                    {
                        wxMessageBox(wxString::Format(_("Item with identifier '%s' already exists"),Corrected.c_str()));
                        return false;
                    }
                    else
                    {
                        DBGLOG(_T("wxSmith: Duplicated identifier name: \"%s\""),IdName.c_str());
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
    	    else
    	    {
    	        if ( GetEditMode() != wxsREMSource )
    	        {
    	            if ( IdName != _T("-1") )
    	            {
                        if ( !Correct )
                        {
                            wxMessageBox(wxString::Format(_("XRC allow only -1 value instead of identifier name."),Corrected.c_str()));
                            return false;
                        }
                        else
                        {
                            DBGLOG(_T("wxSmith: Invalid numeric id value: \"%s\""),IdName.c_str());
                        }

                        IdName = _T("-1");
                        Valid = false;
    	            }
    	        }
    	    }

    	}

    	if ( Correct )
    	{
    		IdName = Corrected;
    	}
    }

    if ( !Valid && Correct ) SetModified();

	return Valid;
}

void wxsWindowRes::BuildIdsArray(wxsWidget* Widget,wxArrayString& Array)
{
	int Cnt = Widget->GetChildCount();
	for ( int i=0; i<Cnt; i++ )
	{
		wxsWidget* Child = Widget->GetChild(i);
		if ( Child->GetBPType() & bptId )
		{
		    const wxString& Name = Child->BaseProperties.IdName;
		    long Value;
		    bool Predefined = Name.ToLong(&Value,0);
		    if ( !Predefined )
		    {
		        for ( int i=0; i<wxsPredefinedIdsCount; i++ )
                {
                    if ( Name == wxsPredefinedIds[i] )
                    {
                        Predefined = true;
                        break;
                    }
                }
		    }
		    if ( !Predefined )
		    {
                Array.Add(Child->BaseProperties.IdName);
		    }
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

void wxsWindowRes::UpdateEventTable(bool NotifyChange)
{
    if ( !GetProject() ) return;

	wxString CodeHeader;
	CodeHeader.Printf(wxsBHeaderF("EventTable"),ClassName.c_str());
	wxString Code = CodeHeader;
	Code.Append(_T('\n'));
	CollectEventTableEnteries(Code,RootWidget);
	wxsCoder::Get()->AddCode(GetProject()->GetProjectFileName(GetSourceFile()),CodeHeader,Code);

	if ( !NotifyChange ) return;

    // Applying modified state
    if ( GetEditor() )
    {
    	// Must process inside editor (updating titile)
    	GetEditor()->SetModified();
    }
    else
    {
        SetModified();
    }

    // Storing change inside undo buffer
    if ( GetEditor() )
    {
    	((wxsWindowEditor*)GetEditor())->GetUndoBuff()->StoreChange();
    }
}

void wxsWindowRes::CollectEventTableEnteries(wxString& Code,wxsWidget* Widget)
{
	int Cnt = Widget->GetChildCount();
    Code += Widget->GetEvents()->GetArrayEnteries();
	for ( int i=0; i<Cnt; i++ )
	{
		wxsWidget* Child = Widget->GetChild(i);
		CollectEventTableEnteries(Code,Child);
	}
}

void wxsWindowRes::GenXrcFetchingCode(wxString& Code,wxsWidget* Widget)
{
	int Cnt = Widget->GetChildCount();
	for ( int i=0; i<Cnt; i++ )
	{
		wxsWidget* Child = Widget->GetChild(i);
		if ( ( Child->GetBPType() & bptVariable ) &&
		     ( Child->BaseProperties.IsMember ) )
		{
			Code.Append(Child->BaseProperties.VarName);
			Code.Append(_T(" = XRCCTRL(*this,\""));
			Code.Append(Child->BaseProperties.IdName);
			Code.Append(_T("\","));
			Code.Append(Child->GetInfo().Name);
			Code.Append(_T(");\n"));
		}
		GenXrcFetchingCode(Code,Child);
	}
}

TiXmlDocument* wxsWindowRes::GenerateXrc()
{
	int EMStore = GetEditMode();
	SetEditMode(wxsREMFile);
	TiXmlDocument* Generated = GenerateXml();
	SetEditMode(EMStore);
	return Generated;
}

void wxsWindowRes::SetModified(bool modified)
{
    if ( !GetEditor() ) return;

    Modified = modified;

	// Changing unmodified entry inside undo buffer
	if ( !modified )
	{
		((wxsWindowEditor*)GetEditor())->GetUndoBuff()->Saved();
	}
}

void wxsWindowRes::EditorClosed()
{
    bool Old = wxsTREE()->SkipSelectionChange(true);
	wxsBlockSelectEvents();
	GetRootWidget()->KillTree(wxsTREE());
	Clear();
	if ( !GetProject() )
	{
	    wxsEXTRES()->ResClosed(this);
	}
	Modified = false;
	Manager::Get()->Yield();
	wxsBlockSelectEvents(false);
	wxsTREE()->SkipSelectionChange(Old);
}

void wxsWindowRes::BuildTree(wxTreeCtrl* Tree,wxTreeItemId WhereToAdd,bool NoWidgets)
{
	SetTreeItemId(
        Tree->AppendItem(
            WhereToAdd,
            GetClassName(),
            -1,-1,
            new wxsResourceTreeData(this) ) );
    if ( !NoWidgets )
    {
        GetRootWidget()->BuildTree(Tree,GetTreeItemId());
    }
}

void wxsWindowRes::RebuildTree(wxTreeCtrl* Tree,bool NoWidgets)
{
    Tree->DeleteChildren(GetTreeItemId());
    if ( !NoWidgets )
    {
        GetRootWidget()->BuildTree(Tree,GetTreeItemId());
    }
}

bool wxsWindowRes::ChangeRootWidget(wxsWidget* NewRoot,bool DeletePrevious)
{
	if ( !NewRoot ) return false;
	// New root must be of the same type as current
	if ( RootWidget->GetInfo().Name != NewRoot->GetInfo().Name ) return false;
    wxsBlockSelectEvents();
	RootWidget->KillTree(wxsTREE());
	if ( GetEditor() )
	{
		((wxsWindowEditor*)GetEditor())->KillPreview();
	}
	if ( DeletePrevious ) wxsFACTORY()->Kill(RootWidget);
	RootWidget = NewRoot;
    wxTreeCtrl* Tree = wxsTREE();
    Tree->SelectItem(GetTreeItemId());
    if ( GetEditor() )
    {
        GetRootWidget()->BuildTree(Tree,GetTreeItemId());
        ((wxsWindowEditor*)GetEditor())->BuildPreview();
    }
    RebuildCode();
    wxsBlockSelectEvents(false);
	return true;
}

void wxsWindowRes::OnSelect()
{
    wxsSelectWidget(GetRootWidget());
}
