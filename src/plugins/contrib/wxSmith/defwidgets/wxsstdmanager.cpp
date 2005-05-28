#include "wxsstdmanager.h"

#include "wxsbutton.h"
#include "wxsframe.h"
#include "wxspanel.h"
#include "wxsdialog.h"
#include "wxsgridsizer.h"
#include "wxsstatictext.h"
#include "wxscheckbox.h"
#include "wxstogglebutton.h"

/******************************************************************************/
/* Infos for standard widgets                                                 */
/******************************************************************************/

static const char* DefLicence     = "wxWidgets License";
static const char* DefAuthor      = "wxWidgets Team";
static const char* DefAuthorEmail = "";
static const char* DefAuthorSite  = "www.wxwidgets.org";
static const char* DefCategory    = "Standard";


#define Entry(Name,Link)    \
    {   "wx"#Name,          \
        DefLicence,         \
        DefAuthor,          \
        DefAuthorEmail,     \
        DefAuthorSite,      \
        Link,               \
        DefCategory,        \
        false,              \
        false,              \
        2, 42,              \
        NULL,               \
        &wxsStdManager,     \
        wxs##Name##Id,      \
        0,                  \
        wxs##Name##Styles   \
    },

#define EntryNoStyles(Name,Link)    \
    {   "wx"#Name,          \
        DefLicence,         \
        DefAuthor,          \
        DefAuthorEmail,     \
        DefAuthorSite,      \
        Link,               \
        DefCategory,        \
        false,              \
        false,              \
        2, 42,              \
        NULL,               \
        &wxsStdManager,     \
        wxs##Name##Id,      \
        0,                  \
        NULL                \
    },


static wxsWidgetInfo StdInfos[] =
{
    { "", "", "", "", "", "", "", false, false, 0, 0, NULL, NULL, wxsNoneId },  // NONE
    
    {   "wxGridSizer",
        DefLicence,
        DefAuthor,
        DefAuthorEmail,
        DefAuthorSite,
        "http://www.wxwidgets.org/manuals/2.4.2/wx189.htm#wxgridsizer",
        DefCategory,
        true,
        true,
        2, 42,
        NULL,
        &wxsStdManager,
        wxsGridSizerId,
        0,
        NULL
    },
 
    Entry(Button,      "http://www.wxwidgets.org/manuals/2.4.2/wx46.htm#wxbutton")
    Entry(ToggleButton,"http://www.wxwidgets.org/manuals/2.4.2/wx396.htm#wxtogglebutton")
    Entry(CheckBox,    "http://www.wxwidgets.org/manuals/2.4.2/wx52.htm#wxcheckbox")
    Entry(StaticText,  "http://www.wxwidgets.org/manuals/2.4.2/wx362.htm#wxstatictext")

    {   "wxDialog",
        DefLicence,
        DefAuthor,
        DefAuthorEmail,
        DefAuthorSite,
        "http://www.wxwidgets.org/manuals/2.4.2/wx109.htm#wxdialog",
        "",
        true,
        false,
        2, 42,
        NULL,
        &wxsStdManager,
        wxsDialogId,
        0,
        wxsDialogStyles
    },

    {   "wxFrame",
        DefLicence,
        DefAuthor,
        DefAuthorEmail,
        DefAuthorSite,
        "http://www.wxwidgets.org/manuals/2.4.2/wx163.htm#wxframe",
        "",
        true,
        false,
        2, 42,
        NULL,
        &wxsStdManager,
        wxsFrameId,
        0,
        // TODO (SpOoN#1#): Add styles
    },
    
    {   "wxPanel",
        DefLicence,
        DefAuthor,
        DefAuthorEmail,
        DefAuthorSite,
        "http://www.wxwidgets.org/manuals/2.4.2/wx291.htm#wxpanel",
        DefCategory,
        true,
        false,
        2, 42,
        NULL,
        &wxsStdManager,
        wxsPanelId,
        0
        // TODO (SpOoN#1#): Add styles
    },
};

static const int StdInfosCnt = sizeof(StdInfos) / sizeof(StdInfos[0]);


wxsStdManagerT::wxsStdManagerT()
{
	//ctor
}

wxsStdManagerT::~wxsStdManagerT()
{
	//dtor
}

int wxsStdManagerT::GetCount()
{
    return wxsStdIdCount;
}
        
/** Getting widget's info */
const wxsWidgetInfo* wxsStdManagerT::GetWidgetInfo(int Number)
{
    if ( Number < 0 || Number >= StdInfosCnt ) Number = wxsNoneId;
    
    assert ( StdInfos[Number].Id == Number );
    
    return &StdInfos[Number];
}

/** Getting new widget */
wxsWidget* wxsStdManagerT::ProduceWidget(int Id)
{
    switch ( Id )
    {
        case wxsGridSizerId:    return new wxsGridSizer(this);
        case wxsButtonId:       return new wxsButton(this);
        case wxsCheckBoxId:     return new wxsCheckBox(this);
        case wxsStaticTextId:   return new wxsStaticText(this);
        case wxsPanelId:        return new wxsPanel(this);
        case wxsFrameId:        return new wxsFrame(this);
        case wxsDialogId:       return new wxsDialog(this);
        case wxsToggleButtonId: return new wxsToggleButton(this);
    }
    
    return NULL;
}

/** Killing widget */
void wxsStdManagerT::KillWidget(wxsWidget* Widget)
{
    if ( Widget ) delete Widget;
}

wxsStdManagerT wxsStdManager;
