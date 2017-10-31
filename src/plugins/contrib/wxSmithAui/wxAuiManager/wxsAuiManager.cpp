/*
* This file is part of wxSmithAui plugin for Code::Blocks Studio
* Copyright (C) 2008-2009  César Fernández Domínguez
*
* wxSmithAui is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* wxSmithAui is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmithAui. If not, see <http://www.gnu.org/licenses/>.
*/

#include "wxsAuiManager.h"
#include "wxsAuiManagerParentQP.h"

#include <wxwidgets/wxsitemresdata.h>
#include <wxwidgets/wxsflags.h>
#include <logmanager.h>

#include <wx/dcclient.h>

using namespace wxsFlags;

namespace
{
    #include "../images/wxsAuiManager16.xpm"
    #include "../images/wxsAuiManager32.xpm"

    wxsRegisterItem<wxsAuiManager> Reg(
        _T("wxAuiManager"),                 // Class name
        wxsTContainer,                      // Item type
        _T("wxWindows"),                    // License
        _T("Benjamin I. Williams"),         // Author
        _T(""),                             // Author's email
        _T(""),                             // Item's homepage
        _T("Aui"),                          // Category in palette
        60,                                 // Priority in palette
        _T("AuiManager"),                   // Base part of names for new items
        wxsCPP,                             // List of coding languages supported by this item
        2, 8,                               // Version
        wxBitmap(wxsAuiManager32_xpm),      // 32x32 bitmap
        wxBitmap(wxsAuiManager16_xpm),      // 16x16 bitmap
        false);                             // Allow in XRC

    class wxsAuiManagerPreview: public wxPanel
    {
        public:
            wxsAuiManagerPreview(wxWindow* Parent): wxPanel(Parent,-1,wxDefaultPosition,wxDefaultSize, wxTAB_TRAVERSAL)
            {
                InheritAttributes();
                Connect(wxID_ANY,wxEVT_PAINT,(wxObjectEventFunction)&wxsAuiManagerPreview::OnPaint);
            }

            void SetAuiManager(wxSmithAuiManager* AuiManagerIn)
            {
                Disconnect(wxID_ANY,wxEVT_PAINT,(wxObjectEventFunction)&wxsAuiManagerPreview::OnPaint);
                AuiManager = AuiManagerIn;
            }

        private:

            void OnPaint(wxPaintEvent& /*event*/)
            {
                // Drawing additional border around te panel
                wxPaintDC DC(this);
                int W, H;
                GetSize(&W,&H);
                DC.SetBrush(*wxTRANSPARENT_BRUSH);
                DC.SetPen(*wxGREEN_PEN);
                DC.DrawRectangle(0,0,W,H);
            }

            wxSmithAuiManager* AuiManager;
    };

    WXS_ST_BEGIN(wxsAuiMangagerStyles,_T("wxAUI_MGR_DEFAULT"))
        WXS_ST_CATEGORY("wxAuiManager")
        WXS_ST(wxAUI_MGR_ALLOW_FLOATING)
        WXS_ST(wxAUI_MGR_ALLOW_ACTIVE_PANE)
        WXS_ST(wxAUI_MGR_TRANSPARENT_DRAG)
        WXS_ST(wxAUI_MGR_TRANSPARENT_HINT)
        WXS_ST(wxAUI_MGR_VENETIAN_BLINDS_HINT)
        WXS_ST(wxAUI_MGR_RECTANGLE_HINT)
        WXS_ST(wxAUI_MGR_HINT_FADE)
        WXS_ST(wxAUI_MGR_NO_VENETIAN_BLINDS_FADE)
        WXS_ST(wxAUI_MGR_DEFAULT)
    WXS_ST_END()

    WXS_EV_BEGIN(wxsAuiManagerEvents)
        WXS_EV(EVT_AUI_PANE_ACTIVATED,wxEVT_AUI_PANE_ACTIVATED,wxAuiManagerEvent,Activated)  // added in 3.0
        WXS_EV(EVT_AUI_PANE_BUTTON,wxEVT_AUI_PANE_BUTTON,wxAuiManagerEvent,PaneButton)
        WXS_EV(EVT_AUI_PANE_CLOSE,wxEVT_AUI_PANE_CLOSE,wxAuiManagerEvent,PaneClose)
        WXS_EV(EVT_AUI_PANE_MAXIMIZE,wxEVT_AUI_PANE_MAXIMIZE,wxAuiManagerEvent,PaneMaximize)
        WXS_EV(EVT_AUI_PANE_RESTORE,wxEVT_AUI_PANE_RESTORE,wxAuiManagerEvent,PaneRestore)
        WXS_EV(EVT_AUI_RENDER,wxEVT_AUI_RENDER,wxAuiManagerEvent,Render)
        WXS_EV(EVT_AUI_FIND_MANAGER,wxEVT_AUI_FIND_MANAGER,wxAuiManagerEvent,FindManager)
    WXS_EV_END()
}

void wxsAuiPaneInfoExtra::OnEnumProperties(long /*Flags*/)
{
    //Name
    WXS_SHORT_STRING_P(wxsAuiPaneInfoExtra,m_Name,_("AUI Name"),_T("name"),_(""),false,1002);

    //Standard pane type
    static const long    StandardPaneValues[] = { 0, DefaultPane, CenterPane, ToolbarPane, 0 };
    static const wxChar* StandardPaneNames[]  = { _("None"), _("Default"), _("Center"), _("Toolbar"), 0 };
    WXS_ENUM_P(wxsAuiPaneInfoExtra,m_StandardPane,_("AUI Standard pane type"),_T("standardpanetype"),StandardPaneValues,StandardPaneNames,0,1001)

    //Caption and caption bar buttons
    WXS_SHORT_STRING_P(wxsAuiPaneInfoExtra,m_Caption,_("AUI Caption"),_T("caption"),_(""),false,1000)
    WXS_BOOL_P(wxsAuiPaneInfoExtra,m_CaptionVisible,_("AUI Caption Visible"),_T("captionvisible"),true,990)
    WXS_BOOL_P(wxsAuiPaneInfoExtra,m_MinimizeButton,_("AUI Minimize button"),_T("minimizebutton"),true,989)
    WXS_BOOL_P(wxsAuiPaneInfoExtra,m_MaximizeButton,_("AUI Maximize button"),_T("maximizebutton"),true,988)
    WXS_BOOL_P(wxsAuiPaneInfoExtra,m_PinButton,_("AUI Pin button"),_T("pinbutton"),true,987)
    WXS_BOOL_P(wxsAuiPaneInfoExtra,m_CloseButton,_("AUI Close button"),_T("closebutton"),true,985)

    //Layer, row and position
    WXS_LONG_P(wxsAuiPaneInfoExtra,m_Layer,_("AUI Layer"),_T("layer"),0,980)
    WXS_LONG_P(wxsAuiPaneInfoExtra,m_Row,_("AUI Row"),_T("row"),0,977)
    WXS_LONG_P(wxsAuiPaneInfoExtra,m_Position,_("AUI Position"),_T("position"),0,976)

    //Dock or Float
    WXS_BOOL_P(wxsAuiPaneInfoExtra,m_Docked,_("AUI Docked"),_T("docked"),true,975)

    //Dock Direction
    static const long    DockValues[] =
        { wxAUI_DOCK_TOP,
          wxAUI_DOCK_BOTTOM,
          wxAUI_DOCK_LEFT,
          wxAUI_DOCK_RIGHT,
          wxAUI_DOCK_CENTER,
          0 };
    static const wxChar* DockNames[]  =
        { _("Top"),
          _("Bottom"),
          _("Left"),
          _("Right"),
          _("Center"),
          0 };
    WXS_ENUM_P(wxsAuiPaneInfoExtra,m_DockDirection,_("AUI Dock Direction"),_T("dockdirection"),DockValues,DockNames,wxAUI_DOCK_TOP,970)

    //Dock Fixed
    WXS_BOOL_P(wxsAuiPaneInfoExtra,m_DockFixed,_("AUI Dock Fixed"),_T("dockfixed"),true,965)

    //Available Dockable
    WXS_AUIMANAGERFLAGS_P(wxsAuiPaneInfoExtra,m_DockableFlags,960)

    //Floatable
    WXS_BOOL_P(wxsAuiPaneInfoExtra,m_Floatable,_("AUI Floatable"),_T("floatable"),true,950)

    //Floating properties
    WXS_POSITION_P(wxsAuiPaneInfoExtra,m_FloatingPosition,_("AUI Default Floating Position"),_("  Left"),_("  Top"),_("  Pos in dialog units"),_T("floatingposition"),956)
    WXS_SIZE_P(wxsAuiPaneInfoExtra,m_FloatingSize,_("AUI Default Floating Size"),_("  Floating Width"),_("  Floating Height"),_("  Size in dialog units"),_T("floatingsize"),955)

    //Resizable
    WXS_BOOL_P(wxsAuiPaneInfoExtra,m_Resizable,_("AUI Resizable"),_T("resizable"),true,940)

    //Movable
    WXS_BOOL_P(wxsAuiPaneInfoExtra,m_Movable,_("AUI Movable"),_T("movable"),true,925)

    //Gripper
    static const long    GripperValues[] = { 0, wxLEFT, wxTOP, 0 };
    static const wxChar* GripperNames[]  = { _("None"), _("Default"), _("Top"), 0 };
    WXS_ENUM_P(wxsAuiPaneInfoExtra,m_Gripper,_("AUI Gripper"),_T("gripper"),GripperValues,GripperNames,0,920)

    //Pane Border
    WXS_BOOL_P(wxsAuiPaneInfoExtra,m_PaneBorder,_("AUI Pane border"),_T("paneborder"),true,915)

    //Visible
    WXS_BOOL_P(wxsAuiPaneInfoExtra,m_Visible,_("AUI Visible"),_T("visible"),true,910)

    //Destroy on close
    WXS_BOOL_P(wxsAuiPaneInfoExtra,m_DestroyOnClose,_("AUI Destroy on close"),_T("destroyonclose"),true,900)

    //First time add
    WXS_FIRSTADD(wxsAuiPaneInfoExtra,m_FirstAdd,_("firstadd"),true)
}

void wxsAuiPaneInfoExtra::OnPropertyChanged()
{
    if ( m_LastDockDirection == 0 )
    {
        m_LastDockDirection = m_DockDirection;
        m_LastStandardPane  = m_StandardPane;
        return;
    }

    if ( m_LastDockDirection != m_DockDirection )
    {
        m_LastDockDirection = m_DockDirection;

        switch ( m_DockDirection )
        {
            case wxAUI_DOCK_TOP:
                m_DockableFlags |= wxsAuiDockableProperty::TopDockable;
                break;

            case wxAUI_DOCK_BOTTOM:
                m_DockableFlags |= wxsAuiDockableProperty::BottomDockable;
                break;

            case wxAUI_DOCK_LEFT:
                m_DockableFlags |= wxsAuiDockableProperty::LeftDockable;
                break;

            case wxAUI_DOCK_RIGHT:
                m_DockableFlags |= wxsAuiDockableProperty::RightDockable;
                break;

            case wxAUI_DOCK_CENTER:
                m_DockableFlags |= wxsAuiDockableProperty::Dockable;
                break;

            default:
                break;
        }
    }

    if ( m_LastStandardPane != m_StandardPane )
    {
        m_LastStandardPane = m_StandardPane;
        switch ( m_StandardPane )
        {
            case DefaultPane:
                m_DockableFlags  = wxsAuiDockableProperty::Dockable;
                m_Floatable      = true;
                m_Movable        = true;
                m_Resizable      = true;
                m_CaptionVisible = true;
                m_PaneBorder     = true;
                m_CloseButton    = true;
                break;

            case CenterPane:
                m_Docked         = true;
                m_Visible        = true;
                m_DockableFlags  = wxsAuiDockableProperty::Dockable;
                m_Floatable      = false;
                m_Movable        = false;
                m_Resizable      = true;
                m_PaneBorder     = true;
                m_CaptionVisible = false;
                m_Gripper        = 0;
                m_DestroyOnClose = false;
                m_DockFixed      = false;
                m_CloseButton    = false;
                m_MaximizeButton = false;
                m_MinimizeButton = false;
                m_PinButton      = false;
                m_DockDirection  = wxAUI_DOCK_CENTER;
                break;

            case ToolbarPane:
                m_DockableFlags  = wxsAuiDockableProperty::Dockable;
                m_Floatable      = true;
                m_Movable        = true;
                m_Resizable      = false;
                m_CaptionVisible = false;
                m_PaneBorder     = true;
                m_CloseButton    = true;
                m_Gripper        = wxLEFT;
                if ( m_Layer == 0 ) m_Layer = 10;
                break;

            default:
                break;
        }
    }

    NotifyPropertyChange();
}

wxString wxsAuiPaneInfoExtra::AllParamsCode(wxsCoderContext* Ctx,wxsItem* ChildPane)
{
    wxString str;
    wxsBaseProperties* BaseProps = ChildPane->GetBaseProps();

    switch ( Ctx->m_Language )
    {
        case wxsCPP:
            //Caption and caption bar buttons
            if ( m_Name != wxEmptyString ) str << _T(".Name(_T(\"") << m_Name << _T("\"))");

            //Standard pane type
            switch ( m_StandardPane )
            {
                case CenterPane:
                    str << _T(".CenterPane()");
                    break;

                case ToolbarPane:
                    str << _T(".ToolbarPane()");
                    break;

                case DefaultPane: // fall-though
                default:
                    str << _T(".DefaultPane()");
                    break;
            }

            //Caption and caption bar buttons
            if ( m_Caption != wxEmptyString ) str << _T(".Caption(_(\"") << m_Caption << _T("\"))");
            if ( m_CaptionVisible && !(m_StandardPane == DefaultPane) ) str << _T(".CaptionVisible()");
            if ( !m_CaptionVisible && !(m_StandardPane == CenterPane) && !(m_StandardPane == ToolbarPane) )
                str << _T(".CaptionVisible(false)");

            if ( m_MinimizeButton ) str << _T(".MinimizeButton()");
            if ( m_MaximizeButton ) str << _T(".MaximizeButton()");
            if ( m_PinButton      ) str << _T(".PinButton()");
                 if (  m_CloseButton &&  (m_StandardPane == CenterPane) ) str << _T(".CloseButton()");
            else if ( !m_CloseButton && !(m_StandardPane == CenterPane) ) str << _T(".CloseButton(false)");

            //Layer, row and position
            if ( m_Layer > 0    ) str << wxString::Format(_T(".Layer(%ld)"),m_Layer);
            if ( m_Row > 0      ) str << wxString::Format(_T(".Row(%ld)"),m_Row);
            if ( m_Position != 0 ) str << wxString::Format(_T(".Position(%ld)"),m_Position);

            //Dock or Float
            if ( !m_Docked ) str << _T(".Float()");

            //Dock Direction
                 if ( m_DockDirection == wxAUI_DOCK_TOP    ) str << _T(".Top()");
            else if ( m_DockDirection == wxAUI_DOCK_BOTTOM ) str << _T(".Bottom()");
            else if ( m_DockDirection == wxAUI_DOCK_LEFT   ) str << _T(".Left()");
            else if ( m_DockDirection == wxAUI_DOCK_RIGHT  ) str << _T(".Right()");
            else if ( m_DockDirection == wxAUI_DOCK_CENTER && !(m_StandardPane == CenterPane) )
                str << _T(".Center()");

            //DockFixed
            if ( m_DockFixed ) str << _T(".DockFixed()");

            //Available Dockable places
            str << wxsAuiDockableProperty::GetString(m_DockableFlags);

            //Floatable
                 if (  m_Floatable &&  (m_StandardPane == CenterPane) ) str << _T(".Floatable()");
            else if ( !m_Floatable && !(m_StandardPane == CenterPane) ) str << _T(".Floatable(false)");

            //Floating properties
            if ( !m_FloatingPosition.IsDefault ) str << _T(".FloatingPosition(") << m_FloatingPosition.GetPositionCode(Ctx) << _T(")");
            if ( !m_FloatingSize.IsDefault     ) str << _T(".FloatingSize("    ) << m_FloatingSize.GetSizeCode(Ctx        ) << _T(")");

            //Resizable
                 if (  m_Resizable &&  (m_StandardPane == ToolbarPane) ) str << _T(".Resizable()");
            else if ( !m_Resizable && !(m_StandardPane == ToolbarPane) ) str << _T(".Resizable(false)");

            //Size properties
            if ( !BaseProps->m_Size.IsDefault     ) str << _T(".BestSize(") << BaseProps->m_Size.   GetSizeCode(Ctx ) << _T(")");
            if ( !BaseProps->m_MinSize.IsDefault  ) str << _T(".MinSize(" ) << BaseProps->m_MinSize.GetSizeCode(Ctx ) << _T(")");
            if ( !BaseProps->m_MaxSize.IsDefault  ) str << _T(".MaxSize(" ) << BaseProps->m_MaxSize.GetSizeCode(Ctx ) << _T(")");

            //Movable
                 if (  m_Movable &&  (m_StandardPane == CenterPane) ) str << _T(".Movable(false)");
            else if ( !m_Movable && !(m_StandardPane == CenterPane) ) str << _T(".Movable(false)");

            //Gripper
                 if ( m_Gripper == 0 && (m_StandardPane == ToolbarPane) ) str << _T(".Gripper(false)");
            else if ( m_Gripper == wxLEFT                               ) str << _T(".Gripper()");
            else if ( m_Gripper == wxTOP                                ) str << _T(".Gripper().GripperTop()");

            //Pane border
            if ( !m_PaneBorder ) str << _T(".PaneBorder(false)");

            //Visible
            if ( !m_Visible ) str << _T(".Hide()");

            //Destroy on close
            if ( m_DestroyOnClose ) str << _T(".DestroyOnClose()");

        case wxsUnknownLanguage: // fall-through
        default:
            wxsCodeMarks::Unknown(_T("wxsAuiManagerExtra::AllParamsCode"),Ctx->m_Language);
    }

    return str;
}

wxAuiPaneInfo wxsAuiPaneInfoExtra::GetPaneInfoFlags(wxWindow* Parent,wxsItem* ChildPane,bool Exact)
{
    wxAuiPaneInfo PaneInfo;
    wxsBaseProperties* BaseProps = ChildPane->GetBaseProps();

    //Standard pane type
    switch ( m_StandardPane )
    {
        case CenterPane:
            PaneInfo.CenterPane();
            break;

        case ToolbarPane:
            PaneInfo.ToolbarPane();
            break;

        case DefaultPane: // fall-though
        default:
            PaneInfo.DefaultPane();
            break;
    }

    //Caption and caption bar buttons
    PaneInfo.Caption(m_Caption).CaptionVisible(m_CaptionVisible);

    PaneInfo.MinimizeButton(m_MinimizeButton);
    PaneInfo.MaximizeButton(m_MaximizeButton);
    PaneInfo.PinButton(m_PinButton);
    PaneInfo.CloseButton(m_CloseButton);

    //Layer, row and position
    if ( m_Layer > 0     ) PaneInfo.Layer(m_Layer);
    if ( m_Row > 0       ) PaneInfo.Row(m_Row);
    if ( m_Position != 0 ) PaneInfo.Position(m_Position);

    //Floating pane
    if ( Exact && !m_Docked ) PaneInfo.Float();

    //Dock Direction
         if ( m_DockDirection == wxAUI_DOCK_TOP    ) PaneInfo.Top();
    else if ( m_DockDirection == wxAUI_DOCK_BOTTOM ) PaneInfo.Bottom();
    else if ( m_DockDirection == wxAUI_DOCK_LEFT   ) PaneInfo.Left();
    else if ( m_DockDirection == wxAUI_DOCK_RIGHT  ) PaneInfo.Right();
    else if ( m_DockDirection == wxAUI_DOCK_CENTER ) PaneInfo.Center();

    //DockFixed
    if ( m_DockFixed ) PaneInfo.DockFixed();

    //Available Dockable places
    wxsAuiDockableProperty::GetDockableFlags(PaneInfo,m_DockableFlags);

    //Floatable
    if ( !m_Floatable ) PaneInfo.Floatable(false);

    //Floating properties
    if ( !m_FloatingPosition.IsDefault ) PaneInfo.FloatingPosition(m_FloatingPosition.GetPosition(Parent));
    if ( !m_FloatingSize.IsDefault     ) PaneInfo.FloatingSize(m_FloatingSize.GetSize(Parent));

    //Resizable
    if ( !m_Resizable ) PaneInfo.Resizable(false);

    //Size properties
    if ( !BaseProps->m_Size.IsDefault     ) PaneInfo.BestSize(BaseProps->m_Size.GetSize(Parent));
    if ( !BaseProps->m_MinSize.IsDefault  ) PaneInfo.MinSize (BaseProps->m_MinSize.GetSize(Parent));
    if ( !BaseProps->m_MaxSize.IsDefault  ) PaneInfo.MaxSize (BaseProps->m_MaxSize.GetSize(Parent));

    //Movable
    if ( !m_Movable ) PaneInfo.Movable(false);

    //Gripper
             if ( m_Gripper == 0      ) PaneInfo.Gripper(false);
        else if ( m_Gripper == wxLEFT ) PaneInfo.Gripper();
        else if ( m_Gripper == wxTOP  ) PaneInfo.Gripper().GripperTop();

    //Pane border
    if ( !m_PaneBorder ) PaneInfo.PaneBorder(false);

    //Show
    if ( !m_Visible && Exact ) PaneInfo.Hide();

    return PaneInfo;
}

wxsAuiManager::wxsAuiManager(wxsItemResData* Data):
    wxsParent(Data,&Reg.Info,flVariable|flSubclass|flExtraCode,wxsAuiManagerEvents,wxsAuiMangagerStyles)
{
}

void wxsAuiManager::OnBuildAuiManagerCreatingCode()
{
    switch ( GetLanguage() )
    {
        case wxsCPP:
        {
            AddHeader(_T("<wx/aui/aui.h>"),GetInfo().ClassName,0);
            AddHeader(_T("<wx/aui/aui.h>"),_T("wxAuiManagerEvent"),0);
            Codef(_T("%C(%W, %T);\n"));
            return;
        }

        case wxsUnknownLanguage: // fall-through
        default:
        {
            wxsCodeMarks::Unknown(_T("wxsAuiManager::OnBuildAuiManagerCreatingCode"),GetLanguage());
        }
    }
}

void wxsAuiManager::OnBuildCreatingCode()
{
    OnBuildAuiManagerCreatingCode();

    bool UnknownLang = false;
    int Count = GetChildCount();
    for ( int i=0; i<Count; i++ )
    {
        wxsItem* Child = GetChild(i);
        wxsAuiPaneInfoExtra* ANBExtra = (wxsAuiPaneInfoExtra*)GetChildExtra(i);

        if (!Child || !ANBExtra) continue;

        // Using same parent as we got, AuiManager is not a parent window
        Child->BuildCode(GetCoderContext());

        switch ( Child->GetType() )
        {
            case wxsTWidget:
            case wxsTContainer:
                switch ( GetLanguage() )
                {
                    case wxsCPP:
                    {
                        Codef(_T("%AAddPane(%o, wxAuiPaneInfo()%s);\n"),i,ANBExtra->AllParamsCode(GetCoderContext(),Child).wx_str());
                        break;
                    }

                    case wxsUnknownLanguage: // fall-through
                    default:
                    {
                        UnknownLang = true;
                    }
                }
                break;

            default:
                break;
        }
    }

    if (Count)
        Codef(_T("%AUpdate();\n"));

    if ( UnknownLang )
        wxsCodeMarks::Unknown(_T("wxsAuiManager::OnBuildCreatingCode"),GetLanguage());
}

wxObject* wxsAuiManager::OnBuildPreview(wxWindow* Parent,long Flags)
{
    int Count = GetChildCount();

    wxWindow* NewParent;
    wxSmithAuiManager* AuiManager;

    if ( !(Flags & pfExact) )
    {
        NewParent = new wxsAuiManagerPreview(Parent);
        if (Count)
            AuiManager = new wxSmithAuiManager(NewParent, Style());
    }
    else
    {
        NewParent = Parent;
        AuiManager = new wxSmithAuiManager(NewParent, Style());
    }

    if (Count)
    {
        wxAuiPaneInfo PaneInfo;
        for ( int i=0; i<Count; i++ )
        {
            wxsItem* Child = GetChild(i);
            wxsAuiPaneInfoExtra* APIExtra = (wxsAuiPaneInfoExtra*)GetChildExtra(i);

            // We pass either Parent passed to current BuildPreview function
            // or pointer to additional parent currently created
            wxObject* ChildPreview = Child->BuildPreview(NewParent, Flags);

            if ( !ChildPreview ) continue;

            wxWindow* ChildAsWindow = wxDynamicCast(ChildPreview, wxWindow);

            if (APIExtra)
                PaneInfo = APIExtra->GetPaneInfoFlags(NewParent, Child, Flags & pfExact);

            wxAuiToolBar* ChildAsToolBar = wxDynamicCast(ChildAsWindow, wxAuiToolBar);
            if ( ChildAsToolBar ) ( (wxsAuiToolBar*)Child )->m_GripperSize = AuiManager->GetArtProvider()->GetMetric(wxAUI_DOCKART_GRIPPER_SIZE);

            AuiManager->AddPane(ChildAsWindow, PaneInfo);
        }

        AuiManager->Update();
    }

    if ( !(Flags & pfExact) )
    {
        if (Count)
            wxDynamicCast(NewParent, wxsAuiManagerPreview)->SetAuiManager(AuiManager);
        return NewParent;
    }

    return AuiManager;
}

wxsPropertyContainer* wxsAuiManager::OnBuildExtra()
{
    return new wxsAuiPaneInfoExtra();
}

bool wxsAuiManager::OnCanAddChild(wxsItem* Item,bool ShowMessage)
{
    if ( Item->GetType() == wxsTSizer )
    {
        if ( ShowMessage )
            wxMessageBox(_("Can not add sizer into AuiManager.\nAdd panels first."));
        return false;
    }
    else if( Item->GetType() == wxsTSpacer )
    {
        if ( ShowMessage )
            wxMessageBox(_("Spacer can be added into sizer only"));
        return false;
    }

    return wxsParent::OnCanAddChild(Item,ShowMessage);
}

bool wxsAuiManager::OnCanAddToParent(wxsParent* Parent,bool ShowMessage)
{
    if ( Parent->GetClassName() == _T("wxAuiManager") )
    {
        if ( ShowMessage )
            wxMessageBox(_("wxAuiManager can't be added to a wxAuiManager. Add panels first."));
        return false;
    }

    if ( Parent->GetClassName().EndsWith(_T("book")) )
    {
        if ( ShowMessage )
            wxMessageBox(_("wxAuiManager can't be added to a book type widget. Add panels first."));
        return false;
    }

    if ( Parent->GetType() == wxsTSizer )
    {
        if ( ShowMessage )
            wxMessageBox(_("wxAuiManager can't be added to a sizer. Add panels first."));
        return false;
    }

    if ( !wxDynamicCast(Parent->BuildPreview(new wxFrame(0,-1,wxEmptyString),0),wxWindow) )
    {
        if ( ShowMessage )
            wxMessageBox(_("wxAuiManager can only be added to a wxWindow descendant."));
        return false;
    }

    return wxsParent::OnCanAddToParent(Parent,ShowMessage);
}

void wxsAuiManager::OnAddChildQPP(wxsItem* Child,wxsAdvQPP* QPP)
{
    wxsParent::OnAddChildQPP(Child,QPP);

    int Index = GetChildIndex(Child);

    wxsAuiPaneInfoExtra* ChildExtra = (wxsAuiPaneInfoExtra*)GetChildExtra(Index);

    if ( Index >= 0 )
        QPP->Register(new wxsAuiManagerParentQP(QPP,(wxsAuiPaneInfoExtra*)GetChildExtra(Index)),_("PaneInfo"));

    if ( ChildExtra->m_FirstAdd )
    {
        ChildExtra->m_FirstAdd = false;
        if ( wxDynamicCast(Child->BuildPreview(new wxFrame(0,-1,wxEmptyString),0),wxAuiToolBar) )
        {
            ChildExtra->m_StandardPane = wxsAuiPaneInfoExtra::ToolbarPane;
            ChildExtra->m_DockableFlags  = wxsAuiDockableProperty::Dockable;
            ChildExtra->m_Floatable      = true;
            ChildExtra->m_Movable        = true;
            ChildExtra->m_Resizable      = false;
            ChildExtra->m_CaptionVisible = false;
            ChildExtra->m_PaneBorder     = true;
            ChildExtra->m_CloseButton    = true;
            ChildExtra->m_Gripper        = wxLEFT;
            ChildExtra->m_DockDirection  = wxAUI_DOCK_TOP;
            if ( ChildExtra->m_Layer == 0 ) ChildExtra->m_Layer = 10;
            NotifyPropertyChange();
        }
    }
}

wxString wxsAuiManager::OnXmlGetExtraObjectClass()
{
    return _T("AuiManagerItem");
}

void wxsAuiManager::OnEnumItemProperties(long /*Flags*/)
{
}

void wxsAuiManager::OnAddItemQPP(wxsAdvQPP* QPP)
{
    OnAddAuiMangagerQPP(QPP);
}
