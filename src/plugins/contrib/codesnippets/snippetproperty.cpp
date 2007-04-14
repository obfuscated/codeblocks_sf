/*
	This file is part of Code Snippets, a plugin for Code::Blocks
	Copyright (C) 2006 Arto Jonsson
    Parts Copyright (C) 2007 Pecan Heber

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/
// RCS-ID: $Id: snippetproperty.cpp 43 2007-04-11 19:25:25Z Pecan $
// ----------------------------------------------------------------------------
//  SnippetProperty.cpp                                         //(pecan 2006/9/12)
// ----------------------------------------------------------------------------

#ifdef WX_PRECOMP
    #include "wx_pch.h"
#else
    #include <wx/datetime.h>
#endif
    #include <wx/file.h>
    #include <wx/filename.h>
    #include <wx/dnd.h>

#if defined(BUILDING_PLUGIN)
    #include "sdk.h"
#endif

#include "codesnippetswindow.h"
#include "snippetitemdata.h"
#include "menuidentifiers.h"
#include "messagebox.h"
#include "snippetsconfig.h"
#include "snippetproperty.h"

    // wxScintila (Edit) stuff
    //! global print data, to remember settings during the session
    wxPrintData *g_printData = (wxPrintData*) NULL;
    wxPageSetupData *g_pageSetupData = (wxPageSetupData*) NULL;

   BEGIN_EVENT_TABLE(SnippetProperty, SnippetPropertyForm)
    EVT_BUTTON(wxID_OK,             SnippetProperty::OnOk)
    EVT_BUTTON(wxID_CANCEL,         SnippetProperty::OnCancel)
    EVT_BUTTON(ID_SNIPPETBUTTON,    SnippetProperty::OnSnippetButton)
    EVT_BUTTON(ID_FILESELECTBUTTON, SnippetProperty::OnFileSelectButton)
   	//-EVT_LEAVE_WINDOW(               SnippetProperty::OnLeaveWindow)
   END_EVENT_TABLE()

// ----------------------------------------------------------------------------
class SnippetDropTarget : public wxTextDropTarget
// ----------------------------------------------------------------------------
{
    // Drop target used to place dragged data into Properties dialog

	public:
		SnippetDropTarget(SnippetProperty* window) : m_Window(window) {}
		~SnippetDropTarget() {}
		bool OnDropText(wxCoord x, wxCoord y, const wxString& data);
	private:
		SnippetProperty* m_Window;
};
// ----------------------------------------------------------------------------
bool SnippetDropTarget::OnDropText(wxCoord x, wxCoord y, const wxString& data)
// ----------------------------------------------------------------------------
{
    // Put dragged text into SnippetTextCtrl
    #ifdef LOGGING
     LOGIT( _T("Dragged Data[%s]"), data.GetData() );
    #endif //LOGGING
    //m_Window->m_SnippetEditCtrl->WriteText(data);
    m_Window->m_SnippetEditCtrl->AddText(data);
    return true;

} // end of OnDropText
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//  SnippetProperty methods
// ----------------------------------------------------------------------------
SnippetProperty::SnippetProperty(wxTreeCtrl* pTree, wxTreeItemId itemId, wxSemaphore* pWaitSem)
// ----------------------------------------------------------------------------
    :SnippetPropertyForm(pTree->GetParent(),-1,wxT("Snippet Properties"))
{
    // ctor
    m_pWaitingSemaphore = pWaitSem;
    m_nScrollWidthMax = 0;
    // Move dialog into midpoint of parent window
    wxPoint mousePosn = ::wxGetMousePosition();
    this->Move(mousePosn.x, mousePosn.y);
    this->SetSize(mousePosn.x, mousePosn.y, 460, 260);
    //SetSize(460, 260);
    GetConfig()->CenterChildOnParent(this);

    m_pTreeCtrl = pTree;
    m_TreeItemId = itemId;

    // Initialize the properties fields
    m_ItemLabelTextCtrl->   SetValue( pTree->GetItemText(itemId) );
    m_SnippetEditCtrl->     SetText( wxT("Enter text or Filename") );
    m_SnippetEditCtrl->SetFocus();


	// Get the item
	if (( m_pSnippetDataItem = (SnippetItemData*)(pTree->GetItemData(itemId))))
	{
		// Check that we're using the correct item type
		if (m_pSnippetDataItem->GetType() != SnippetItemData::TYPE_SNIPPET)
		{
			return;
		}

        wxString snippetText = m_pSnippetDataItem->GetSnippet() ;
        if ( not snippetText.IsEmpty() )
            GetSnippetEditCtrl()-> SetText( snippetText );

        // reset horiz scroll max width
        m_nScrollWidthMax = GetSnippetEditCtrl()->GetLongestLinePixelWidth();
        GetSnippetEditCtrl()->SetScrollWidth(m_nScrollWidthMax);
	}

	SetDropTarget(new SnippetDropTarget(this));

}//SnippetProperty ctor
// ----------------------------------------------------------------------------
SnippetProperty::~SnippetProperty()
// ----------------------------------------------------------------------------
{
    //dtor
}
// ----------------------------------------------------------------------------
void SnippetProperty::OnOk(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    // set data to edited snippet
    m_pSnippetDataItem->SetSnippet( m_SnippetEditCtrl->GetText() );
    // label may have been edited
    m_pTreeCtrl->SetItemText( m_TreeItemId, m_ItemLabelTextCtrl->GetValue() );
    if (m_pWaitingSemaphore) m_pWaitingSemaphore->Post();
    this->EndModal(wxID_OK);
}
// ----------------------------------------------------------------------------
void SnippetProperty::OnCancel(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    if (m_pWaitingSemaphore) m_pWaitingSemaphore->Post();
    this->EndModal(wxID_CANCEL);
}
// ----------------------------------------------------------------------------
void SnippetProperty::OnSnippetButton(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    // Snippet button clicked from OpenAsFile context menu

    if ( GetActiveMenuId() == idMnuConvertToFileLink )
    {       // let user choose a file to hold snippet
        wxString ChosenFileName = wxFileSelector(wxT("Choose a Link target"));
        if (not ChosenFileName.IsEmpty())
            m_SnippetEditCtrl-> SetText( ChosenFileName );
        return;
    }

    // Snippet button clicked from menubar edit(Menu) or properties context(Mnu)
    if ( ( g_activeMenuId == idMnuProperties ) //note: mnu vs menu
        #if !defined(BUILDING_PLUGIN)
        || ( g_activeMenuId == idMenuProperties)
        #endif
        )
    {
        if ( GetConfig()->SettingsExternalEditor.IsEmpty())
        {   messageBox(wxT("Use Menu/Settings/Options to specify an external editor.") );
            return;
        }
            // let user edit the snippet text
            // write text to temp file
            // invoke the external editor
            // read text back into snippet
        if ( IsSnippetFile() )
             InvokeEditOnSnippetFile();
        else InvokeEditOnSnippetText();

    }//fi
    return;
}
// ----------------------------------------------------------------------------
void SnippetProperty::OnFileSelectButton(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    // Properties File Select button clicked

    // let choose a file name to insert into snippet property
    wxString ChosenFileName = wxFileSelector(wxT("Choose a file"));
    if (not ChosenFileName.IsEmpty())
        m_SnippetEditCtrl-> SetText( ChosenFileName );
    return;

}//OnFileSelectButton
// ----------------------------------------------------------------------------
void SnippetProperty::InvokeEditOnSnippetText()
// ----------------------------------------------------------------------------
{
        //Exec Edit Snippet Text

        wxFileName tmpFileName = wxFileName::CreateTempFileName(wxEmptyString);
        #ifdef LOGGING
         LOGIT( _T("fakename:[%s]"),tmpFileName.GetFullPath().GetData() );
        #endif //LOGGING

        wxFile tmpFile( tmpFileName.GetFullPath(), wxFile::write);
        if (not tmpFile.IsOpened() )
        {
            wxMessageBox(wxT("Open failed for:")+tmpFileName.GetFullPath());
            return ;
        }
        wxString snippetData( GetSnippet() );
        tmpFile.Write( cbU2C(snippetData), snippetData.Length());
        tmpFile.Close();
            // Invoke the external editor on the temporary file
            // file name must be surrounded with quotes when using wxExecute
        wxString execString = GetConfig()->SettingsExternalEditor + wxT(" \"") + tmpFileName.GetFullPath() + wxT("\"");

        #ifdef LOGGING
         LOGIT( _T("Properties wxExecute[%s]"), execString.GetData() );
        #endif //LOGGING

            // Invoke the external editor and wait for its termination
        ::wxExecute( execString, wxEXEC_SYNC);
            // Read the edited data back into the snippet text
        tmpFile.Open(tmpFileName.GetFullPath(), wxFile::read);
        if (not tmpFile.IsOpened() )
        {   messageBox(wxT("Abort.Error reading temp data file."));
            return;
        }
        unsigned long fileSize = tmpFile.Length();

        #ifdef LOGGING
         LOGIT( _T("New file length[%d]"),fileSize );
        #endif //LOGGING

            // check the data for validity
        char pBuf[fileSize+1];
        size_t nResult = tmpFile.Read( pBuf, fileSize );
        if ( wxInvalidOffset == (int)nResult )
            messageBox(wxT("InvokeEditOnSnippetText()\nError reading temp file"));
        pBuf[fileSize] = 0;
        tmpFile.Close();

        #ifdef LOGGING
          //LOGIT( _T("pBuf[%s]"), pBuf );
        #endif //LOGGING

            // convert data back to internal format
        snippetData = cbC2U( pBuf );

         #ifdef LOGGING
          //LOGIT( _T("snippetData[%s]"), snippetData.GetData() );
         #endif //LOGGING

            // delete the temporary file
        ::wxRemoveFile( tmpFileName.GetFullPath() );

            // update Tree item
        m_SnippetEditCtrl-> SetText( snippetData );
}
// ----------------------------------------------------------------------------
void SnippetProperty::InvokeEditOnSnippetFile()
// ----------------------------------------------------------------------------
{
    // Open as file

    if (not IsSnippetFile() ) return;

	// If snippet is file, open it
	wxString FileName = GetSnippet();

    // we have an actual file name, not just text
    wxString pgmName = GetConfig()->SettingsExternalEditor;
    if ( pgmName.IsEmpty() )
    {
        #if defined(__WXMSW__)
                pgmName = wxT("notepad");
        #elif defined(__UNIX__)
                pgmName = wxT("gedit");
        #endif
    }

    // file name must be surrounded with quotes when using wxExecute
	wxString execString = pgmName + wxT(" \"") + FileName + wxT("\"");
    #ifdef LOGGING
     LOGIT( _T("InvokeEditOnSnippetFile[%s]"), execString.GetData() );
    #endif //LOGGING
    ::wxExecute( execString);
    return;
}//InvokeEditOnSnippetFile
// ----------------------------------------------------------------------------
void SnippetProperty::OnLeaveWindow(wxMouseEvent& event)
// ----------------------------------------------------------------------------
{
    // If mouse is dragging out of window with a text selection,
    // create a drag source of the selected text.
    // -----------------------
    // LEAVE_WINDOW
    // -----------------------

    #ifdef LOGGING
     //LOGIT( _T("SnippetProperty:MOUSE EVT_LEAVE_WINDOW") );
    #endif //LOGGING

    // Left mouse key must be down (dragging)
    if (not event.LeftIsDown() ) {event.Skip();return;}

    // check if data is available
    wxString selectedData = m_SnippetEditCtrl->GetSelectedText();
    if ( selectedData.IsEmpty()) {event.Skip();return;}

    // we now have data, create both a simple text and filename drop source
    wxTextDataObject* textData = new wxTextDataObject();
    wxFileDataObject* fileData = new wxFileDataObject();
        // fill text and file sources with snippet
    wxDropSource textSource( *textData, (wxWindow*)event.GetEventObject() );
    textData->SetText( selectedData );
    wxDropSource fileSource( *fileData, (wxWindow*)event.GetEventObject() );
    fileData->AddFile( (selectedData.Len() > 128) ? wxString(wxEmptyString) : selectedData );
        // set composite data object to contain both text and file data
    wxDataObjectComposite *data = new wxDataObjectComposite();
    data->Add( (wxDataObjectSimple*)textData );
    data->Add( (wxDataObjectSimple*)fileData, true ); // set file data as preferred
        // create the drop source containing both data types
    wxDropSource source( *data, (wxWindow*)event.GetEventObject()  );

    #ifdef LOGGING
     LOGIT( _T("DropSource Text[%s],File[%s]"),
                textData->GetText().GetData(),
                fileData->GetFilenames().Item(0).GetData() );
    #endif //LOGGING
        // allow both copy and move
    int flags = 0;
    flags |= wxDrag_AllowMove;
    // do the dragNdrop
    wxDragResult result = source.DoDragDrop(flags);
    // report the results
    #if LOGGING
        wxString pc;
        switch ( result )
        {
            case wxDragError:   pc = _T("Error!");    break;
            case wxDragNone:    pc = _T("Nothing");   break;
            case wxDragCopy:    pc = _T("Copied");    break;
            case wxDragMove:    pc = _T("Moved");     break;
            case wxDragCancel:  pc = _T("Cancelled"); break;
            default:            pc = _T("Huh?");      break;
        }
        LOGIT( wxT("SnippetProperty::OnLeftDown DoDragDrop returned[%s]"),pc.GetData() );
    #else
        wxUnusedVar(result);
    #endif // wxUSE_STATUSBAR


    delete textData; //wxTextDataObject
    delete fileData; //wxFileDataObject

    event.Skip();
    return;

}
