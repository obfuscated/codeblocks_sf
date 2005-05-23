#include "wxsstdmanager.h"

#include "wxsbutton.h"
#include "wxsframe.h"
#include "wxspanel.h"
#include "wxsdialog.h"
#include "wxsgridsizer.h"
#include "wxsstatictext.h"

/******************************************************************************/
/* Infos for standard widgets                                                 */
/******************************************************************************/

static const char* DefLicence     = "wxWidgets License";
static const char* DefAuthor      = "wxWidgets Team";
static const char* DefAuthorEmail = "";
static const char* DefAuthorSite  = "www.wxwidgets.org";
static const char* DefCategory    = "Standard";

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
        NULL
    },
    
    { 
        "wxButton",
        DefLicence,
        DefAuthor,
        DefAuthorEmail,
        DefAuthorSite,
        "http://www.wxwidgets.org/manuals/2.4.2/wx46.htm#wxbutton",
        DefCategory,
        false,
        false,
        2, 42,
        NULL,
        &wxsStdManager,
        wxsButtonId,
        wxsButtonStyles
    },

    { 
        "wxStaticText",
        DefLicence,
        DefAuthor,
        DefAuthorEmail,
        DefAuthorSite,
        "http://www.wxwidgets.org/manuals/2.4.2/wx362.htm#wxstatictext",
        DefCategory,
        false,
        false,
        2, 42,
        NULL,
        &wxsStdManager,
        wxsStaticTextId,
        wxsStaticTextStyles
    },

    { 
        "wxDialog",
        DefLicence,
        DefAuthor,
        DefAuthorEmail,
        DefAuthorSite,
        "http://www.wxwidgets.org/manuals/2.4.2/wx109.htm#wxdialog",
        DefCategory,
        true,
        false,
        2, 42,
        NULL,
        &wxsStdManager,
        wxsDialogId,
        wxsDialogStyles
    },

    { 
        "wxFrame",
        DefLicence,
        DefAuthor,
        DefAuthorEmail,
        DefAuthorSite,
        "http://www.wxwidgets.org/manuals/2.4.2/wx163.htm#wxframe",
        DefCategory,
        true,
        false,
        2, 42,
        NULL,
        &wxsStdManager,
        wxsFrameId
        // TODO (SpOoN#1#): Add styles
    },
    
    { 
        "wxPanel",
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
        wxsPanelId
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
        case wxsGridSizerId:
            return new wxsGridSizer(this);
            
        case wxsButtonId:
            return new wxsButton(this);
            
        case wxsStaticTextId:
            return new wxsStaticText(this);
            
        case wxsPanelId:
            return new wxsPanel(this);
            
        case wxsFrameId:
            return new wxsFrame(this);
            
        case wxsDialogId:
            return new wxsDialog(this);
            
        
    }
    
    return NULL;
}

/** Killing widget */
void wxsStdManagerT::KillWidget(wxsWidget* Widget)
{
    if ( Widget ) delete Widget;
}

wxsStdManagerT wxsStdManager;
