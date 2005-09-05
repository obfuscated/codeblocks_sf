#include "wxscoder.h"

#include <manager.h>
#include <editormanager.h>
#include <messagemanager.h>
#include <wx/ffile.h>

#include "wxsglobals.h"

#define DebLog Manager::Get()->GetMessageManager()->DebugLog

wxsCoder::wxsCoder():
	Enteries(NULL),
	BlockProcess(false)
{
}

wxsCoder::~wxsCoder()
{
	ProcessCodeQueue();
}

void wxsCoder::AddCode(const wxString& FileName,const wxString& BlockHeader,const wxString& Code,bool Immediately)
{
	DataMutex.Lock();
	
	// Iterating through all enteries searching for currently associated code

	CodeEntry* Entry;
	CodeEntry* Previous;
	for ( Entry = Enteries, Previous = NULL;
	      Entry != NULL;
	      Previous = Entry, Entry = Entry->Next )
	{
		if ( Entry->FileName == FileName && Entry->BlockHeader == BlockHeader )
			break;
	}
	
	if ( Entry )
	{
		( Previous ? Previous->Next : Enteries ) = Entry->Next;		
		Previous = Entry;
		if ( Previous ) while ( Previous->Next ) Previous = Previous->Next;
	}
	else
	{
		Entry = new CodeEntry;
		Entry->FileName = FileName;
		Entry->BlockHeader = BlockHeader;
	}

	Entry->Code = Code;
	
	if ( Immediately )
	{
		ApplyChanges(Entry);
		delete Entry;
	}
	else
	{
        time(&Entry->TimeStamp);
        Entry->Next = NULL;
        ( Previous ? Previous->Next : Enteries ) = Entry;
	}
	DataMutex.Unlock();
	
// TODO (SpOoN#1#): Add sheduling and replace direct processing
	
	ProcessCodeQueue();
}

bool wxsCoder::ProcessCodeQueue()
{
	wxMutexLocker Locker(DataMutex);
	
	if ( BlockProcess ) return false;
	BlockProcess = true;
	
	bool Result = true;
	time_t now;
	time(&now);

// TODO (SpOoN#1#): Process requests which are staying inside queue long enough
	while ( Enteries )
	{
		CodeEntry* Entry = Enteries;
		ApplyChanges(Entry);
		Enteries = Enteries->Next;
		delete Entry;
	}
	
	BlockProcess = false;
	return Result;
}

void wxsCoder::DropQueue()
{
	wxMutexLocker Locker(DataMutex);

	for ( CodeEntry* Entry = Enteries, *Next; Entry; Entry = Next )
	{
		Next = Entry->Next;
		delete Entry;
	}
}

bool wxsCoder::ApplyChanges(wxsCoder::CodeEntry* Entry)
{
	EditorManager* EM = Manager::Get()->GetEditorManager();
	assert ( EM != NULL );
	
    cbEditor* Editor = EM->GetBuiltinEditor(Entry->FileName);
    if ( Editor )
    {
        return ApplyChanges(Entry,Editor);
    }
    
    return ApplyChanges(Entry,Entry->FileName);
}

bool wxsCoder::ApplyChanges(wxsCoder::CodeEntry* Entry,cbEditor* Editor)
{
	assert ( Editor != NULL );
	cbStyledTextCtrl* Ctrl = Editor->GetControl();
	assert ( Ctrl != NULL );
	
	Ctrl->SetSearchFlags(wxSCI_FIND_MATCHCASE);
	
	Ctrl->SetTargetStart(0);
	Ctrl->SetTargetEnd(Ctrl->GetLength());
	
	int Position = Ctrl->SearchInTarget(Entry->BlockHeader);
	
	if ( Position == -1 )
	{
		wxMessageBox(wxString::Format(
			_("Couldn't find code with header:\n\t\"%s\"\nin file '%s'"),
			Entry->BlockHeader.c_str(),
			Editor->GetFilename().c_str()));
		return false;
	}

    // Beginning of this code block is in Position, now searching for end
    Ctrl->SetTargetStart(Position);
    Ctrl->SetTargetEnd(Ctrl->GetLength());
    int End = Ctrl->SearchInTarget(wxsBEnd());
    if ( End == -1 )
    {
        wxMessageBox(wxString::Format(
            _("Unfinished block of auto-generated code with header:\n\t\"%s\"\nin file '%s'"),
            Entry->BlockHeader.c_str(),
            Editor->GetFilename().c_str()));
        return false;
    }
    else
    {
        Ctrl->SetTargetStart(Position);
        Ctrl->SetTargetEnd(End);
        Ctrl->ReplaceTarget(Entry->Code);
        Editor->SetModified();
    }
		
	return true;
}

bool wxsCoder::ApplyChanges(wxsCoder::CodeEntry* Entry,const wxString& FileName)
{
    wxFFile File(FileName,_T("r"));
    if ( !File.IsOpened() )
    {
    	/*
		wxMessageBox(wxString::Format(
			_("Couldn't open file '%s' for reading"),
			FileName.c_str()));
        */
    	return false;
    }
    
    wxString Content;
    if ( !File.ReadAll(&Content) )
    {
    	/*
		wxMessageBox(wxString::Format(
			_("Couldn't read from file '%s'"),
			FileName.c_str()));
        */
    	return false;
    }

    int Position = Content.First(Entry->BlockHeader);
    
    if ( Position == -1 )   
    {
    	/*
		wxMessageBox(wxString::Format(
			_("Couldn't find code with header:\n\t\"%s\"\nin file '%s'"),
			Entry->BlockHeader.c_str(),
			FileName.c_str()));
        */
		return false;
    }
    
    wxString Result = Content.Left(Position);
    Content.Remove(0,Position);
    Position = Content.First(wxsBEnd());
    if ( Position == -1 )
    {
    	/*
        wxMessageBox(wxString::Format(
            _("Unfinished block of auto-generated code with header:\n\t\"%s\"\nin file '%s'"),
            Entry->BlockHeader.c_str(),
            FileName.c_str()));
        */
        return false;
    }
    
// FIXME (SpOoN#1#): Rebuild new code to support valid eol mode
    Result += Entry->Code;
    Result += Content.Remove(0,Position);
    
    File.Close();
    
    if ( !File.Open(FileName,_T("w")) )
    {
    	/*
		wxMessageBox(wxString::Format(
			_("Couldn't open file '%s' for writing"),
			FileName.c_str()));
        */
    	return false;
    }
    
    if ( !File.Write(Result) )
    {
    	/*
		wxMessageBox(wxString::Format(
			_("Couldn't write to file '%s'"),
			FileName.c_str()));
        */
    	return false;
    }
    
	return true;
}

bool wxsCoder::ProcessCodeForFile(const wxString& FileName)
{
	wxMutexLocker Locker(DataMutex);

	bool Result = true;
	
	for ( CodeEntry* Entry = Enteries, *Previous = NULL; Entry; )
	{
		if ( Entry->FileName == FileName )
		{
			ApplyChanges(Entry);
			CodeEntry* Next = ( Previous ? Previous->Next : Enteries ) = Entry->Next;
			delete Entry;
			Entry = Next;
		}
		else
		{
			Previous = Entry;
			Entry = Entry->Next;
		}
	}
	
	return Result;
}

static wxsCoder SingletonObject;
wxsCoder* wxsCoder::Singleton = &SingletonObject;

wxString wxsCoder::GetCode(const wxString& FileName,const wxString& BlockHeader)
{
	EditorManager* EM = Manager::Get()->GetEditorManager();
	assert ( EM != NULL );
    cbEditor* Editor = EM->GetBuiltinEditor(FileName);
    if ( Editor )
    {
        cbStyledTextCtrl* Ctrl = Editor->GetControl();
        Ctrl->SetSearchFlags(wxSCI_FIND_MATCHCASE);
        Ctrl->SetTargetStart(0);
        Ctrl->SetTargetEnd(Ctrl->GetLength());
        int Position = Ctrl->SearchInTarget(BlockHeader);
        if ( Position == -1 ) return _T("");
        Ctrl->SetTargetStart(Position);
        Ctrl->SetTargetEnd(Ctrl->GetLength());
        int End = Ctrl->SearchInTarget(wxsBEnd());
        if ( End == -1 ) return _T("");
        return Ctrl->GetTextRange(Position,End);
    }
		
    wxFFile File(FileName,_T("r"));
    wxString Content;
    if ( !File.IsOpened() ) return _T("");
    if ( !File.ReadAll(&Content) ) return _T("");
    int Position = Content.First(BlockHeader);
    if ( Position == -1 ) return _T("");
    Content.Remove(0,Position);
    Position = Content.First(wxsBEnd());
    if ( Position == -1 ) return _T("");
    Content.Remove(Position);
	return Content;
}
