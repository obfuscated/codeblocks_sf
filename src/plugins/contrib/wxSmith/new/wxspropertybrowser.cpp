#include "wxspropertybrowser.h"

#include "wxsevents.h"
#include "wxsitem.h"
#include "wxsproject.h"
#include "wxscoder.h"
#include "resources/wxswindowres.h"
#include <manager.h>
#include <editormanager.h>

#define NONE_STR        _("-- None --")
#define ADD_NEW_STR     _("-- Add new handler --")
#define EVENTS_PAGE     1

wxsPropertyBrowser::wxsPropertyBrowser(wxWindow* parent): wxsPropertyGridManager(parent,-1)
{
    // TODO: Add icons
    AddPage(_("Properties"));
    AddPage(_("Events"));
}

wxsPropertyBrowser::~wxsPropertyBrowser()
{
}

void wxsPropertyBrowser::OnContainerChanged(wxsPropertyContainer* NewContainer)
{
    EventIds.Clear();

    if ( !NewContainer )
    {
        ClearPage(EVENTS_PAGE);
        Item = NULL;
        Events = NULL;
        Res = NULL;
        Proj = NULL;
        SourceFile.Clear();
        HeaderFile.Clear();
        ClassName.Clear();
    }
    else
    {
        Item = (wxsItem*)NewContainer;
        Events = &Item->GetEvents();
        Res = Item->GetResource();
        Proj = Res->GetProject();
        if ( Proj )
        {
            SourceFile = Proj->GetProjectFileName(Res->GetSourceFile());
            HeaderFile = Proj->GetProjectFileName(Res->GetHeaderFile());
            ClassName = Res->GetClassName();
            BuildEvents();
        }
        else
        {
            ClearPage(EVENTS_PAGE);
        }
    }
}

void wxsPropertyBrowser::OnPropertyChanged(wxPropertyGridEvent& event)
{
    if ( EventIds.Index(event.GetProperty()) )
    {
        // Event property - updating events
        ReadEvents();
        // TODO: Update source code
    }
    else
    {
        // Forwarding event to wxsProperyGridManager
        event.Skip();
    }
}

void wxsPropertyBrowser::BuildEvents()
{
    ClearPage(EVENTS_PAGE);
    SelectPage(EVENTS_PAGE);

	int Cnt = Events->GetCount();
	for ( int i=0; i<Cnt; i++ )
	{
		const wxsEventDesc* Event = Events->GetDesc(i);

        // Building set of available handlers
		wxArrayString Functions;
		FindFunctions(Event->ArgType,Functions);

        // Creating contants
        wxPGConstants Const;
        int Index = 0;
        Const.Add(NONE_STR,0);
        Const.Add(ADD_NEW_STR,1);

        const wxString& FunctionName = Events->GetHandler(i);
        for ( int j=0; j<(int)Functions.Count(); j++ )
        {
        	Const.Add(Functions[j],j+2);
        	if ( Functions[j] == FunctionName )
        	{
        		Index = j+2;
        	}
        }

        EventIds.Add( Append(wxEnumProperty(Event->Entry,wxPG_LABEL,Const,Index)) );
	}
}

void wxsPropertyBrowser::ReadEvents()
{
	bool NeedUpdate = false;
	int Cnt = Events->GetCount();
	for ( int i = 0; i < Cnt; ++i )
	{
		wxPGId Id = EventIds[i];
        wxString Selection = GetPropertyValueAsString(Id);
        if ( Selection == NONE_STR )
        {
            Events->SetHandler(i,_T(""));
        }
        else if ( Selection == ADD_NEW_STR )
        {
            Events->SetHandler(i,GetNewFunction(Events->GetDesc(i)));
            NeedUpdate = true;
        }
        else
        {
            Events->SetHandler(i,Selection);
        }
	}

	if ( NeedUpdate )
	{
		ClearPage(EVENTS_PAGE);
		BuildEvents();
	}
}

void wxsPropertyBrowser::FindFunctions(const wxString& EventType,wxArrayString& Array)
{
	wxString Code = wxsGETCODE(
        HeaderFile,
        wxString::Format(wxsBHeaderF("Handlers"),ClassName.c_str()),
        wxsBEnd() );

    // Basic parsing
    for(;;)
    {
    	// Searching for void statement - it may begin new fuunction declaration
    	int Pos = Code.Find(_T("void"));
    	if ( Pos == -1 ) break;

    	// Removing all before function name
    	Code.Remove(0,Pos+4).Trim(false);

    	// Getting function name
    	Pos  = 0;
    	while ( (int)Code.Length() > Pos )
    	{
    		wxChar First = Code.GetChar(Pos);
    		if ( ( First<_T('A') || First>_T('Z') ) &&
    		     ( First<_T('a') || First>_T('z') ) &&
    		     ( First<_T('0') || First>_T('9') ) &&
    		     ( First!=_T('_') ) )
            {
            	break;
            }
            Pos++;
    	}
    	wxString NewFunctionName = Code.Mid(0,Pos);
    	Code.Remove(0,Pos).Trim(false);
    	if ( !Code.Length() ) break;

    	// Parsing arguments
    	if ( Code.GetChar(0) != _T('(') ) continue;
    	Code.Remove(0,1).Trim(false);
    	if ( !Code.Length() ) break;

    	// Getting argument type
    	Pos  = 0;
    	while ( (int)Code.Length() > Pos )
    	{
    		wxChar First = Code.GetChar(Pos);
    		if ( ( First<_T('A') || First>_T('Z') ) &&
    		     ( First<_T('a') || First>_T('z') ) &&
    		     ( First<_T('0') || First>_T('9') ) &&
    		     ( First!=_T('_') ) )
            {
            	break;
            }
            Pos++;
    	}
    	wxString NewEventType = Code.Mid(0,Pos);
    	Code.Remove(0,Pos).Trim(false);
    	if ( !Code.Length() ) break;

    	// Checking if the rest of declaratin is valid
    	if ( Code.GetChar(0) != _T('&') ) continue;
    	Code.Remove(0,1).Trim(false);
    	if ( !Code.Length() ) break;

    	// Skipping argument name
    	Pos  = 0;
    	while ( (int)Code.Length() > Pos )
    	{
    		wxChar First = Code.GetChar(Pos);
    		if ( ( First<_T('A') || First>_T('Z') ) &&
    		     ( First<_T('a') || First>_T('z') ) &&
    		     ( First<_T('0') || First>_T('9') ) &&
    		     ( First<_T('_') ) )
            {
            	break;
            }
            Pos++;
    	}
    	Code.Remove(0,Pos).Trim(false);
    	if ( !Code.Length() ) break;

    	if ( Code.GetChar(0) != _T(')') ) continue;
    	Code.Remove(0,1).Trim(false);
    	if ( !Code.Length() ) break;

    	if ( Code.GetChar(0) != _T(';') ) continue;
    	Code.Remove(0,1).Trim(false);

    	if ( NewFunctionName.Length() == 0 || NewEventType.Length() == 0 ) continue;

    	// We got new function, checking event type and adding to array
    	if ( !EventType.Length() || EventType == NewEventType )
    	{
    		Array.Add(NewFunctionName);
    	}
    }
}

wxString wxsPropertyBrowser::GetFunctionProposition(const wxsEventDesc* Event)
{
    // Creating proposition of new function name

    wxString NewNameBase = _T("On");
    NewNameBase << Item->GetVarName() << Event->NewFuncNameBase;

    int Suffix = 0;
    wxArrayString Functions;
    FindFunctions(_T(""),Functions);
    wxString NewName = NewNameBase;

    while ( Functions.Index(NewName) != wxNOT_FOUND )
    {
    	NewName = NewNameBase;
    	NewName.Append(wxString::Format(_T("%d"),++Suffix));
    }

    return NewName;
}

wxString wxsPropertyBrowser::GetNewFunction(const wxsEventDesc* Event)
{
    wxArrayString Functions;
    FindFunctions(_T(""),Functions);

	wxString Name = GetFunctionProposition(Event);

	for (;;)
	{
		Name = ::wxGetTextFromUser(_("Enter name for new handler:"),_("New handler"),Name);
		if ( !Name.Length() ) return _T("");

        if ( !wxsValidateIdentifier(Name) )
        {
        	wxMessageBox(_("Invalid name"));
        	continue;
        }

        if ( Functions.Index(Name) != wxNOT_FOUND )
        {
        	wxMessageBox(_("Handler with this name already exists"));
        	continue;
        }

        break;
	}

	// Creating new function
	if ( !CreateNewFunction(Event,Name) )
	{
		wxMessageBox(_("Couldn't add new handler"));
		return _T("");
	}

	return Name;
}

bool wxsPropertyBrowser::CreateNewFunction(const wxsEventDesc* Event,const wxString& NewFunctionName)
{
	wxString DeclarationsHeader;
	DeclarationsHeader.Printf(wxsBHeaderF("Handlers"),ClassName.c_str());
	wxString Declarations = wxsGETCODE(HeaderFile,DeclarationsHeader,wxsBEnd());
	Declarations << _T("void ") << NewFunctionName << _T('(');
	Declarations << Event->ArgType << _T("& event);\n");
	wxsADDCODE(HeaderFile,DeclarationsHeader,wxsBEnd(),Declarations);

	// TODO: Update source code
	// Res->UpdateEventTable();

	cbEditor* Editor = Manager::Get()->GetEditorManager()->Open(SourceFile);
	if ( !Editor ) return false;

	wxString NewFunctionCode;
	NewFunctionCode << _T("\nvoid ") << ClassName << _T("::") << NewFunctionName
	                << _T("(") << Event->ArgType << _T("& event)\n")
                    << _T("{\n}\n");
    cbStyledTextCtrl* Ctrl = Editor->GetControl();
    Ctrl->DocumentEnd();
    Ctrl->AddText(NewFunctionCode);
    Ctrl->LineUp();
    Ctrl->LineUp();
    Ctrl->LineEnd();
    Editor->SetModified();
    return true;
}

BEGIN_EVENT_TABLE(wxsPropertyBrowser,wxsPropertyGridManager)
    EVT_PG_CHANGED(-1,wxsPropertyBrowser::OnPropertyChanged)
END_EVENT_TABLE()
