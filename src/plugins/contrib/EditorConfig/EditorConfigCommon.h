#ifndef EDITORCONFIGCOMMON_H
#define EDITORCONFIGCOMMON_H

#include <wx/event.h>
#include <wx/object.h>

#include <cbproject.h>

struct SEditorSettings
{
    bool active;
    bool use_tabs;
    bool tab_indents;
    int  tab_width;
    int  indent;
    int  eol_mode;
};
typedef struct SEditorSettings TEditorSettings;

class EditorSettingsChangedEvent : public wxCommandEvent
{
  DECLARE_DYNAMIC_CLASS(EditorSettingsChangedEvent)
public:
  EditorSettingsChangedEvent(wxEventType cmd_type = wxEVT_NULL,
                             int         id       = wxID_ANY) :
    wxCommandEvent(cmd_type, id), m_EditorSettings(),   m_Project(0)   { ; }
  EditorSettingsChangedEvent(wxEventType cmd_type, int id, TEditorSettings es, cbProject* prj) :
    wxCommandEvent(cmd_type, id), m_EditorSettings(es), m_Project(prj) { ; }
  EditorSettingsChangedEvent(const EditorSettingsChangedEvent& from) :
    wxCommandEvent(from), m_EditorSettings(from.m_EditorSettings),
                          m_Project(from.m_Project)                    { ; }
  ~EditorSettingsChangedEvent()                                        { ; }

  virtual wxEvent *Clone() const { return new EditorSettingsChangedEvent(*this); }

  TEditorSettings GetEditorSettings() const
  { return m_EditorSettings; }

  cbProject*      GetProject() const
  { return m_Project; }

private:
  TEditorSettings m_EditorSettings;
  cbProject*      m_Project;
};
typedef void(wxEvtHandler::*wxEditorSettingsChangedEventFunction) (EditorSettingsChangedEvent&);

extern const long ID_EDITOR_SETTINGS_CHANGED_EVENT;
BEGIN_DECLARE_EVENT_TYPES()
//  DECLARE_EXPORTED_EVENT_TYPE(expdecl,  name,                              value                           )
    DECLARE_EXPORTED_EVENT_TYPE(WXEXPORT, wxEVT_EDITOR_SETTINGS_CHANGED_EVENT, ID_EDITOR_SETTINGS_CHANGED_EVENT)
END_DECLARE_EVENT_TYPES()
#define EVT_EDITOR_SETTINGS_CHANGED_EVENT(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        wxEVT_EDITOR_SETTINGS_CHANGED_EVENT, id, -1, \
        (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxEditorSettingsChangedEventFunction) & fn, (wxObject *) 0 ),

#endif // EDITORCONFIGCOMMON_H
