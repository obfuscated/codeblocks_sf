/*
    This file is part of Browse Tracker, a plugin for Code::Blocks
    Copyright (C) 2007 Pecan Heber

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/
// RCS-ID: $Id$


#include <wx/listctrl.h>
#include <wx/listbox.h>
#include <wx/image.h>
#include <wx/dcclient.h>
#include <wx/kbdstate.h>

#include <wx/sizer.h>
#include <wx/dcmemory.h>
#include <wx/font.h>
#include <wx/settings.h>
#include "wx/xrc/xmlres.h"

#include "globals.h"
#include "editorbase.h"
#include "editormanager.h"
#include "cbeditor.h"
#include "configmanager.h"

#include "BrowseTracker.h"
#include "BrowseSelector.h"
#include "Version.h"
#include "BrowseTrackerDefs.h"

wxBitmap BrowseSelector::m_bmp;
namespace
{
    static bool firstPaint = true;
    int idMenuTrackerforward        = XRCID("ViewForwardEd");
    int idMenuTrackerBackward       = XRCID("ViewBackwardEd");
}

// ----------------------------------------------------------------------------
BrowseSelector::BrowseSelector(wxWindow* parent, BrowseTracker* pBrowseTracker, int menuID)
// ----------------------------------------------------------------------------
: m_listBox(NULL)
, m_selectedItem(-1)
, m_panel(NULL)
, m_pBrowseTracker(pBrowseTracker)
, m_menuID(menuID)
{

    Create(parent, pBrowseTracker, m_menuID);
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);
    GetSizer()->Layout();
    Centre();

    int maxFilenameWidth = PopulateListControl( static_cast<EditorBase*>( parent ) );
    wxRect rect = this->GetClientRect();
    int winWidth = Manager::Get()->GetAppWindow()->GetRect().GetWidth();
    int textWidth = 0;
    int textHeight = 0;
    m_listBox->GetTextExtent( wxString(_T('M'), maxFilenameWidth+4), &textWidth, &textHeight);
    rect.width = wxMin(textWidth, winWidth );
    rect.width = wxMax(rect.width, 200);
    this->SetSize(wxSize(rect.width+4,rect.height+4));
    m_panel->SetSize(rect.width,24);
    m_listBox->SetSize(wxSize(rect.width,rect.height));
    firstPaint = true;
    m_KeyDownCode = 0;
    m_KeyDownMods = 0;

    //// put the mouse in this window (debugging)
    //wxRect dlgRect = this->GetRect();
    //int mouseX = dlgRect.GetWidth() >> 1;
    //int mouseY = dlgRect.GetHeight() >> 1;
    //-this->WarpPointer(mouseX, mouseY);
}
// ----------------------------------------------------------------------------
BrowseSelector::BrowseSelector()
// ----------------------------------------------------------------------------
    : wxScrollingDialog()
    , m_listBox(NULL)
    , m_selectedItem(-1)
    , m_panel(NULL)
{
    // constructor without parent is marked private and unused
}

// ----------------------------------------------------------------------------
BrowseSelector::~BrowseSelector()
// ----------------------------------------------------------------------------
{
    //Destructor
}

// ----------------------------------------------------------------------------
void BrowseSelector::Create(wxWindow* parent, BrowseTracker* pBrowseTracker, int menuID)
// ----------------------------------------------------------------------------
{
    m_pBrowseTracker = pBrowseTracker;
    m_menuID = menuID;

    long style = wxWANTS_CHARS;
    if (!wxScrollingDialog::Create(parent, wxID_ANY, wxEmptyString, wxDefaultPosition,
                                   wxDefaultSize, style))
    {
        return;
    }

    wxBoxSizer *sz = new wxBoxSizer(wxVERTICAL);
    SetSizer(sz);

    const long flags = wxLB_SINGLE | wxNO_BORDER | wxWANTS_CHARS;
    m_listBox = new wxListBox(this, wxID_ANY, wxDefaultPosition, wxSize(400, 150), 0, NULL, flags);

    static int panelHeight = 0;
    static int imageSize = 0;
    if (panelHeight == 0)
    {
        wxMemoryDC mem_dc;

        // bitmap must be set before it can be used for anything
        wxBitmap bmp(10, 10);
        mem_dc.SelectObject(bmp);

        wxFont font(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
        font.SetWeight( wxFONTWEIGHT_BOLD );
        mem_dc.SetFont(font);
        int w;
        mem_dc.GetTextExtent(wxT("Tp"), &w, &panelHeight);

        imageSize = cbFindMinSize16to64(panelHeight);
        panelHeight = imageSize + 4; // Place a spacer of 2 pixels

        font.SetWeight(wxFONTWEIGHT_NORMAL);
        mem_dc.SetFont(font);

    }

    m_panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(800, panelHeight));
    // above panel/banner with set to 800, to allow it to grow

    sz->Add(m_panel);
    sz->Add(m_listBox, 1, wxEXPAND);

    SetSizer(sz);

    // Connect events to the list box
    m_listBox->Connect(wxID_ANY, wxEVT_KEY_UP, wxKeyEventHandler(BrowseSelector::OnKeyUp), NULL, this);
    m_listBox->Connect(wxID_ANY, wxEVT_KEY_DOWN, wxKeyEventHandler(BrowseSelector::OnKeyDown), NULL, this);
    m_listBox->Connect(wxID_ANY, wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler(BrowseSelector::OnItemSelected), NULL, this);
    //m_listBox->Connect(wxID_ANY, wxEVT_KILL_FOCUS, wxFocusEventHandler(BrowseSelector::OnWindowKillFocus),NULL,this); //debugging

    // Connect paint event to the panel
    m_panel->Connect(wxID_ANY, wxEVT_PAINT, wxPaintEventHandler(BrowseSelector::OnPanelPaint), NULL, this);
    m_panel->Connect(wxID_ANY, wxEVT_ERASE_BACKGROUND, wxEraseEventHandler(BrowseSelector::OnPanelEraseBg), NULL, this);

    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
    m_listBox->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
    int logfontsize = Manager::Get()->GetConfigManager(_T("message_manager"))->ReadInt(_T("/log_font_size"), 10);
    wxFont cbFont = Manager::Get()->GetAppWindow()->GetFont();
    cbFont.SetPointSize(logfontsize);
    //cbFont.SetWeight( wxFONTWEIGHT_BOLD );
    // Try using font settings from user editor choices
    wxString fontstring = Manager::Get()->GetConfigManager(_T("editor"))->Read(_T("/font"), wxEmptyString);
    if (!fontstring.IsEmpty())
    {
        wxNativeFontInfo nfi;
        nfi.FromString(fontstring);
        cbFont.SetNativeFontInfo(nfi);
    }
    m_listBox->SetFont(cbFont);

    // Create the bitmap, only once
    if (!m_bmp.Ok())
    {
        const wxString filename = ConfigManager::GetDataFolder()
                                + wxString::Format(wxT("/BrowseTracker.zip#zip:images/%dx%d/signpost.png"),
                                                   imageSize, imageSize);
        m_bmp = cbLoadBitmapScaled(filename, wxBITMAP_TYPE_PNG, cbGetContentScaleFactor(*this));
    }
    //?m_listBox->SetFocus();
}
// ----------------------------------------------------------------------------
void BrowseSelector::OnWindowKillFocus(wxFocusEvent& event) //debugging
// ----------------------------------------------------------------------------
{
    // Once in awhile, the selector dialog hangs until it re-given focus.
    // Here, trying to figure out who is stealing the focus.
    event.Skip();
    return; //<-- remove this for testing

    wxBell();
    wxWindow* p = (wxWindow*)event.GetEventObject();
    //if (p == this) asm("int3"); /*trap*/
    LOGIT( _T("BT SetFocusEvent for[%p]"), p);
}
// ----------------------------------------------------------------------------
void BrowseSelector::OnKeyDown(wxKeyEvent &event)
// ----------------------------------------------------------------------------
{
    //int eventKey = event.GetKeyCode();
    event.Skip();
    m_KeyDownCode = 0;
    m_KeyDownMods = 0;
    m_KeyDownCode  = event.GetKeyCode();
    m_KeyDownMods |= event.ControlDown() << 2;
    m_KeyDownMods |= event.AltDown() << 1;
    m_KeyDownMods |= event.ShiftDown() << 0;
}
// ----------------------------------------------------------------------------
void BrowseSelector::OnKeyUp(wxKeyEvent &event)
// ----------------------------------------------------------------------------
{
    int keyUpCode = event.GetKeyCode();
    int keyUpMods = 0;
    keyUpMods |= event.ControlDown() << 2;
    keyUpMods |= event.AltDown() << 1;
    keyUpMods |= event.ShiftDown() << 0;

    if ((m_KeyDownCode == 0) and (m_KeyDownMods == 0))
    {
        //this keyup is from inital menu keydown which we can't see
        m_KeyDownMods = keyUpMods;
        keyUpCode = WXK_DOWN;
        if (m_menuID == idMenuTrackerBackward )
            keyUpCode = WXK_UP;
    }

    if( keyUpMods != m_KeyDownMods )
    {
        CloseDialog();  //user lifted modifier key
    }
    if( m_KeyDownCode == WXK_RETURN )
    {
        // if we see a EnterKey keydown, this keyup is ours,
        // else it's the initial menu item activation.
        CloseDialog();
    }
    if(keyUpCode == WXK_ESCAPE ) //2020/06/19
    {
        CloseDialog();
    }
    if ( (keyUpCode == WXK_UP ) or (keyUpCode == WXK_LEFT) )
    {
        OnNavigationKey(event);
        return;
    }
    if ( (keyUpCode == WXK_DOWN ) or (keyUpCode == WXK_RIGHT) )
    {
        OnNavigationKey(event);
        return;
    }

    event.Skip();
    return;
}
// ----------------------------------------------------------------------------
void BrowseSelector::OnNavigationKey(wxKeyEvent &event)
// ----------------------------------------------------------------------------
{
    long selected = m_listBox->GetSelection();
    long maxItems = m_listBox->GetCount();
    long itemToSelect = 0;

    #if defined(LOGGING)
    LOGIT( _T("OnNavigationKey selected[%ld]maxItems[%ld]key[%d]"), selected, maxItems, event.GetKeyCode() );
    #endif

    if( (event.GetKeyCode() == WXK_LEFT) || (event.GetKeyCode() == WXK_UP) )
    {
        //  Select previous page
        if( selected == 0 )
            itemToSelect = maxItems - 1;
        else
            itemToSelect = selected - 1;
    }
    if( (event.GetKeyCode() == WXK_RIGHT) || (event.GetKeyCode() == WXK_DOWN) )
    {
        // Select next page
        if (selected == maxItems - 1)
            itemToSelect = 0;
        else
            itemToSelect = selected + 1;
    }

    m_listBox->SetSelection( itemToSelect );
    LOGIT( _T("OnNavigationKey Selection[%ld]"), itemToSelect );
}
// ----------------------------------------------------------------------------
int BrowseSelector::PopulateListControl(EditorBase* /*pEditor*/)
// ----------------------------------------------------------------------------
{
    wxString editorFilename;

    // memorize current selection
    int selection = m_pBrowseTracker->GetCurrentEditorIndex();
    int maxCount     = MaxEntries;
    int maxWidth     = 0;


    int itemIdx = 0;
    for(int c=0; c < maxCount; c++)
    {
        editorFilename = m_pBrowseTracker->GetPageFilename(c) ;
        if (not editorFilename.IsEmpty())
        {
            maxWidth = wxMax(maxWidth, (int)editorFilename.Length());
            m_listBox->Append( editorFilename );
            m_indexMap[itemIdx] = c;
            if ( selection == c ) selection = itemIdx;
            itemIdx++;
        }
    }//for


    // Select the entry before/after current entry
    m_listBox->SetSelection( selection );

    return maxWidth;
}

// ----------------------------------------------------------------------------
void BrowseSelector::OnItemSelected(wxCommandEvent & event )
// ----------------------------------------------------------------------------
{
    wxUnusedVar( event );
    CloseDialog();
}

// ----------------------------------------------------------------------------
void BrowseSelector::CloseDialog()
// ----------------------------------------------------------------------------
{
    m_selectedItem = m_listBox->GetSelection();

    if ((m_selectedItem > -1) && (m_selectedItem < MaxEntries))
    {   std::map<int, int>::iterator iter = m_indexMap.find(m_selectedItem);
        LOGIT( _T("ListBox[%ld] Map[%d]"), m_selectedItem, iter->second );
        // we have to end the dlg before activating the editor or else
        // the old editor get re-activated.
        //-m_pBrowseTracker->SetSelection( iter->second ); logic error
        m_pBrowseTracker->m_UpdateUIEditorIndex = iter->second;
    }

    if (not IsModal() ) return; //avoid assert "not modal" error 2020/06/19
    EndModal( wxID_OK );
}
// ----------------------------------------------------------------------------
void BrowseSelector::OnPanelPaint(wxPaintEvent &event)
// ----------------------------------------------------------------------------
{
    wxUnusedVar(event);
    wxPaintDC dc(m_panel);
    wxRect rect = m_panel->GetClientRect();

    firstPaint = true;
    static wxBitmap bmp( rect.width, rect.height );

    if( firstPaint )
    {
        firstPaint = false;
        wxMemoryDC mem_dc;
        mem_dc.SelectObject( bmp );

        wxColour endColour( wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW) );
        wxColour startColour( LightColour(endColour, 50) );
        PaintStraightGradientBox(mem_dc, rect, startColour, endColour);

        // Draw the caption title and place the bitmap
        wxPoint bmpPt;
        wxPoint txtPt;

        // get the bitmap optimal position, and draw it
        bmpPt.y = (rect.height - m_bmp.GetHeight()) / 2;
        bmpPt.x = 3;
        mem_dc.DrawBitmap( m_bmp, bmpPt, true );

        // get the text position, and draw it
        int fontHeight(0), w(0);
        wxFont font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
        font.SetWeight( wxFONTWEIGHT_BOLD );
        mem_dc.SetFont( font );
        mem_dc.GetTextExtent( wxT("Tp"), &w, &fontHeight );

        txtPt.x = bmpPt.x + m_bmp.GetWidth() + 4;
        txtPt.y = (rect.height - fontHeight)/2;
        mem_dc.SetTextForeground( *wxWHITE );
        mem_dc.DrawText( wxT("Browsed Tabs:"), txtPt );
        mem_dc.SelectObject( wxNullBitmap );
    }

    dc.DrawBitmap( bmp, 0, 0 );
}

// ----------------------------------------------------------------------------
void BrowseSelector::OnPanelEraseBg(wxEraseEvent &event)
// ----------------------------------------------------------------------------
{
    wxUnusedVar(event);

}
// ----------------------------------------------------------------------------
wxColor BrowseSelector::LightColour(const wxColour& color, int percent)
// ----------------------------------------------------------------------------
{
    int rd, gd, bd, high = 0;
    wxColor end_color = wxT("WHITE");
    rd = end_color.Red() - color.Red();
    gd = end_color.Green() - color.Green();
    bd = end_color.Blue() - color.Blue();
    high = 100;

    // We take the percent way of the color from color --> white
    int i = percent;
    int r = color.Red() +  ((i*rd*100)/high)/100;
    int g = color.Green() + ((i*gd*100)/high)/100;
    int b = color.Blue() + ((i*bd*100)/high)/100;
    return wxColor(r, g, b);
}
// ----------------------------------------------------------------------------
void BrowseSelector::PaintStraightGradientBox(wxDC& dc, const wxRect& rect, const wxColour& startColor, const wxColour& endColor, bool  vertical)
// ----------------------------------------------------------------------------
{
    int rd, gd, bd, high = 0;
    rd = endColor.Red() - startColor.Red();
    gd = endColor.Green() - startColor.Green();
    bd = endColor.Blue() - startColor.Blue();

    /// Save the current pen and brush
    wxPen savedPen = dc.GetPen();
    wxBrush savedBrush = dc.GetBrush();

    if ( vertical )
        high = rect.GetHeight()-1;
    else
        high = rect.GetWidth()-1;

    if( high < 1 )
        return;

    for (int i = 0; i <= high; ++i)
    {
        int r = startColor.Red() +  ((i*rd*100)/high)/100;
        int g = startColor.Green() + ((i*gd*100)/high)/100;
        int b = startColor.Blue() + ((i*bd*100)/high)/100;

        wxPen p(wxColor(r, g, b));
        dc.SetPen(p);

        if ( vertical )
            dc.DrawLine(rect.x, rect.y+i, rect.x+rect.width, rect.y+i);
        else
            dc.DrawLine(rect.x+i, rect.y, rect.x+i, rect.y+rect.height);
    }

    /// Restore the pen and brush
    dc.SetPen( savedPen );
    dc.SetBrush( savedBrush );
}
// ----------------------------------------------------------------------------
