
#include <scripting/bindings/sq_wx/sq_wx.h>
#include <scripting/bindings/sc_binding_util.h>
#include <wx/choice.h>
#include <wx/hyperlink.h>
#include <wx/propgrid/propgrid.h>

namespace ScriptBindings
{


void bind_wxConstants(HSQUIRRELVM vm)
{
/** \defgroup sq_wx_constants wxWidgets constants bound to squirrel
 *  \ingroup Squirrel
 *  \brief
*/

    // path separator for filenames
    BIND_WXSTR_CONSTANT_NAMED(wxString(wxFILE_SEP_PATH), "wxFILE_SEP_PATH");

    // wxPathFormat
    BIND_INT_CONSTANT(wxPATH_NATIVE);
    BIND_INT_CONSTANT(wxPATH_UNIX);
    BIND_INT_CONSTANT(wxPATH_BEOS);
    BIND_INT_CONSTANT(wxPATH_MAC);
    BIND_INT_CONSTANT(wxPATH_DOS);
    BIND_INT_CONSTANT(wxPATH_WIN);
    BIND_INT_CONSTANT(wxPATH_OS2);
    BIND_INT_CONSTANT(wxPATH_VMS);

    // for wxFileName::GetPath()
    BIND_INT_CONSTANT(wxPATH_GET_VOLUME);
    BIND_INT_CONSTANT(wxPATH_GET_SEPARATOR);

    // wxPathNormalize
    BIND_INT_CONSTANT(wxPATH_NORM_ENV_VARS);
    BIND_INT_CONSTANT(wxPATH_NORM_DOTS);
    BIND_INT_CONSTANT(wxPATH_NORM_TILDE);
    BIND_INT_CONSTANT(wxPATH_NORM_CASE);
    BIND_INT_CONSTANT(wxPATH_NORM_ABSOLUTE);
    BIND_INT_CONSTANT(wxPATH_NORM_LONG);
    BIND_INT_CONSTANT(wxPATH_NORM_SHORTCUT);
    BIND_INT_CONSTANT(wxPATH_NORM_ALL);

    // dialog buttons
    BIND_INT_CONSTANT(wxOK);
    BIND_INT_CONSTANT(wxYES_NO);
    BIND_INT_CONSTANT(wxCANCEL);
    BIND_INT_CONSTANT(wxID_OK);
    BIND_INT_CONSTANT(wxID_YES);
    BIND_INT_CONSTANT(wxID_NO);
    BIND_INT_CONSTANT(wxID_CANCEL);
    BIND_INT_CONSTANT(wxICON_QUESTION);
    BIND_INT_CONSTANT(wxICON_INFORMATION);
    BIND_INT_CONSTANT(wxICON_WARNING);
    BIND_INT_CONSTANT(wxICON_ERROR);
    BIND_INT_CONSTANT(wxICON_HAND);

}

}
