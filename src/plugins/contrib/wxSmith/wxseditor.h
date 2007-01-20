#ifndef WXSEDITOR_H
#define WXSEDITOR_H

#include <editorbase.h>

class wxsResource;

/** \brief Base class for editors used inside wxSmith
 *
 * This class is responsible for proper binding / unbinging editor
 * from resource.
 *
 * Resource is only an information that given resource should exist.
 * Editor does load this resource and allow user to edit it.
 *
 * When edting resource it should always be done in such form:
 *  - Call BeginChange() to put resource into unstable state
 *  - Change datga of reosource (without updating informations on screen)
 *  - Call EndChange() which should update data on screen
 *
 */
class wxsEditor: public EditorBase
{
	public:

        /** \brief Ctor */
		wxsEditor(wxWindow* parent,const wxString& title,wxsResource* Resource);

		/** \brief Dctor */
		virtual ~wxsEditor();

		/** \brief Getting current resouce */
		inline wxsResource* GetResource() { return m_Resource; }

	private:

        /** \brief Currently associated resource */
        wxsResource* m_Resource;
};

#endif
