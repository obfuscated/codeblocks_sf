#include "EditorConfigCommon.h"

#include <wx/utils.h>

const long ID_EDITOR_SETTINGS_CHANGED_EVENT = wxNewId();
DEFINE_EVENT_TYPE(wxEVT_EDITOR_SETTINGS_CHANGED_EVENT)
IMPLEMENT_DYNAMIC_CLASS(EditorSettingsChangedEvent, wxCommandEvent)
