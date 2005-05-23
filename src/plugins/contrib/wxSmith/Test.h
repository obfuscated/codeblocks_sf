#ifndef __TEST_H
#define __TEST_H

#include <wx/mdi.h>

class _cbEditor : public wxMDIChildFrame
{
    DECLARE_EVENT_TABLE()
	public:
		/** cbEditor constructor.
		  * @param parent the parent notebook - you should use EditorManager::Get()
		  * @param filename the filename to open. If filename is empty, it creates a
		  * new, empty, editor.
		  * @param theme the initial color set to use\n
		  * <em>Note: you should not create a cbEditor object directly. Instead
		  * use EditorManager's methods to do it...</em>
		  */
		_cbEditor(wxMDIParentFrame* parent, const wxString& filename);
		/** cbEditor destructor. */
		virtual ~_cbEditor();

};

#endif
