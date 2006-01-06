///////////////////////////////////////////////////////////////////////////////
// Name:	wxFlatNotebook.cpp
// Purpose:     generic implementation of flat style notebook class.
// Author:      Eran Ifrah <admin@eistware.com>
// Modified by: Priyank Bolia <soft@priyank.in>
// URL:			http://www.eistware.com/ and http://www.priyank.in
// Created:     30/12/2005
// Modified:    01/01/2006
// Copyright:   All contributors.
// Licence:     wxWindows license <http://www.wxwidgets.org/licence3.txt>
///////////////////////////////////////////////////////////////////////////////

#include "wxFlatNotebook.h"
#include <algorithm>
#include <wx/tooltip.h>
#include <wx/tipwin.h>

#define BUTTONS_AREA_WIDTH  53

IMPLEMENT_DYNAMIC_CLASS(wxFlatNotebookEvent, wxNotifyEvent)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_FLATNOTEBOOK_PAGE_CHANGED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_FLATNOTEBOOK_PAGE_CHANGING)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_FLATNOTEBOOK_PAGE_CLOSING)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_FLATNOTEBOOK_CONTEXT_MENU)

IMPLEMENT_DYNAMIC_CLASS(wxFlatNotebook, wxPanel)

BEGIN_EVENT_TABLE(wxFlatNotebook, wxPanel)
EVT_NAVIGATION_KEY(wxFlatNotebook::OnNavigationKey)
EVT_ERASE_BACKGROUND(wxFlatNotebook::OnEraseBackground)
EVT_PAINT(wxFlatNotebook::OnPaint)
EVT_SIZE(wxFlatNotebook::OnSize)
END_EVENT_TABLE()

wxFlatNotebook::wxFlatNotebook(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
: wxPanel(parent, id, pos, size, wxTAB_TRAVERSAL | wxNO_BORDER, name)
, m_nFrom(0)
, m_nPadding(4)
{
	m_pages = new wxPageContainer(this, wxID_ANY);
	m_pages->m_nStyle = style;
	m_mainSizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(m_mainSizer);

	// Add the tab container to the sizer
	m_mainSizer->Insert(0, m_pages, 0, wxEXPAND);
	m_pages->SetSizeHints(wxSize(-1, 24));
	m_pages->m_nFrom = m_nFrom;
}

wxFlatNotebook::~wxFlatNotebook(void)
{
}

void wxFlatNotebook::OnPaint(wxPaintEvent& WXUNUSED(event))
{
	wxBufferedPaintDC dc(this);
	wxBrush backBrush = wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_3DLIGHT));
	wxPen   borderPen = wxPen((m_pages->m_nStyle & wxFNB_BORDER) ? wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW) : wxSystemSettings::GetColour(wxSYS_COLOUR_3DLIGHT));

	dc.BeginDrawing();
	wxSize size = GetSize();

	// Background
	dc.SetBrush(backBrush);
	dc.SetPen(borderPen);
	dc.DrawRectangle(0, 0, size.x, size.y);
	dc.EndDrawing();
}

void wxFlatNotebook::OnSize(wxSizeEvent& event)
{
	Refresh();
	event.Skip();
}

void wxFlatNotebook::AddPage(wxWindow* window, const wxString& caption, const bool selected, const int imgindex)
{
    // sanity check
    if (!window)
        return;

    // reparent the window to us
    window->Reparent(this);

	// Add tab
	bool bSelected = selected || m_windows.empty();
	int curSel = m_pages->GetSelection();

	if( !m_pages->IsShown() )
		m_pages->Show();

	m_pages->AddPage(caption, bSelected, imgindex);
	m_windows.push_back(window);

	Freeze();

	// Check if a new selection was made
	if(bSelected)
	{
		if(curSel >= 0)
		{
			// Remove the window from the main sizer
			m_mainSizer->Detach(m_windows[curSel]);
			m_windows[curSel]->Hide();
		}
		if(m_pages->m_nStyle & wxFNB_BORDER)
		{
			if(m_pages->m_nStyle & wxFNB_BOTTOM)
			{
				m_mainSizer->Insert(0, window, 1, wxEXPAND|wxALL, 1);
			}
			else
			{
				// We leave a space of 1 pixel around the window
				m_mainSizer->Add(window, 1, wxEXPAND|wxALL, 1);
			}
		}
		else
		{
			if(m_pages->m_nStyle & wxFNB_BOTTOM)
			{
				m_mainSizer->Insert(0, window, 1, wxEXPAND);
			}
			else
			{
				// We leave a space of 1 pixel around the window
				m_mainSizer->Add(window, 1, wxEXPAND);
			}
		}
	}
	else
	{
		// Hide the page
		window->Hide();
	}
	m_mainSizer->Layout();
	Thaw();
	Refresh();

}

void wxFlatNotebook::SetImageList(wxFlatNotebookImageList * imglist)
{
	m_pages->SetImageList(imglist);
}

wxFlatNotebookImageList * wxFlatNotebook::GetImageList()
{
	return m_pages->GetImageList();
}

bool wxFlatNotebook::InsertPage(size_t index, wxWindow* page, const wxString& text, bool select, const int imgindex)
{
    // sanity check
    if (!page)
        return false;

    // reparent the window to us
    page->Reparent(this);

	// Insert tab
	bool bSelected = select || m_windows.empty();
	int curSel = m_pages->GetSelection();

	std::vector<wxWindow*>::iterator iter = m_windows.begin() + index;
	m_windows.insert(iter, page);
	m_pages->InsertPage(index, page, text, bSelected, imgindex);

	Freeze();

	// Check if a new selection was made
	if(bSelected)
	{
		if(curSel >= 0)
		{
			// Remove the window from the main sizer
			m_mainSizer->Detach(m_windows[curSel]);
			m_windows[curSel]->Hide();
		}
		m_pages->SetSelection(index);
	}
	else
	{
		// Hide the page
		page->Hide();
	}
	Thaw();
	m_mainSizer->Layout();
	Refresh();

	return true;
}

void wxFlatNotebook::SetSelection(size_t page)
{
	if(page >= m_windows.size())
		return;

	int curSel = m_pages->GetSelection();

	// program allows the page change
	Freeze();
	if(curSel >= 0)
	{
		// Remove the window from the main sizer
		m_mainSizer->Detach(m_windows[curSel]);
		m_windows[curSel]->Hide();
	}
	if(m_pages->m_nStyle & wxFNB_BORDER)
	{
		if(m_pages->m_nStyle & wxFNB_BOTTOM)
		{
			m_mainSizer->Insert(0, m_windows[page], 1, wxEXPAND|wxALL, 1);
		}
		else
		{
			// We leave a space of 1 pixel around the window
			m_mainSizer->Add(m_windows[page], 1, wxEXPAND|wxALL, 1);
		}
	}
	else
	{
		if(m_pages->m_nStyle & wxFNB_BOTTOM)
		{
			m_mainSizer->Insert(0, m_windows[page], 1, wxEXPAND);
		}
		else
		{
			// We leave a space of 1 pixel around the window
			m_mainSizer->Add(m_windows[page], 1, wxEXPAND);
		}
	}

	m_windows[page]->Show();
	Thaw();

	m_mainSizer->Layout();
	m_pages->m_iActivePage = (int)page;
	m_pages->DoSetSelection(page);
}

void wxFlatNotebook::DeletePage(size_t page, bool notify)
{
	if(page >= m_windows.size())
		return;

    if (notify)
    {
	// Fire a closing event
	wxFlatNotebookEvent event(wxEVT_COMMAND_FLATNOTEBOOK_PAGE_CLOSING, GetId());
	event.SetSelection((int)page);
	event.SetEventObject(this);
	GetEventHandler()->ProcessEvent(event);

	// The event handler allows it?
    if (!event.IsAllowed())
        return;
    }

	Freeze();

	// Delete the requested page
	wxWindow *pageRemoved = m_windows[page];

	// If the page is the current window, remove it from the sizer
	// as well
	if((int)page == m_pages->GetSelection())
	{
		m_mainSizer->Detach(pageRemoved);
	}

	// Remove it from the array as well
	std::vector<wxWindow*>::iterator iter = std::find(m_windows.begin(), m_windows.end(), pageRemoved);
	if(iter != m_windows.end())
		m_windows.erase(iter);

	// Now we can destroy it; in wxWidgets use Destroy instead of delete
	pageRemoved->Destroy();

	Thaw();

	m_pages->DoDeletePage(page);
	Refresh();
}

bool wxFlatNotebook::DeleteAllPages()
{
	if(m_windows.empty())
		return false;

	Freeze();
	std::vector<wxWindow*>::iterator iter = m_windows.begin();
	for(; iter != m_windows.end(); iter++)
	{
		delete (*iter);
	}

	m_windows.clear();

	Thaw();

	// Clear the container of the tabs as well
	m_pages->DeleteAllPages();
	return true;
}

wxWindow* wxFlatNotebook::GetCurrentPage() const
{
	int sel = m_pages->GetSelection();
	if(sel < 0)
		return NULL;

	return m_windows[sel];
}

wxWindow* wxFlatNotebook::GetPage(size_t page) const
{
	if(page >= m_windows.size())
		return NULL;

	return m_windows[page];
}

int wxFlatNotebook::GetPageIndex(wxWindow* win) const
{
    for (size_t i = 0; i < m_windows.size(); ++i)
    {
        if (m_windows[i] == win)
            return (int)i;
    }
    return -1;
}

int wxFlatNotebook::GetSelection() const
{
	return m_pages->GetSelection();
}

void wxFlatNotebook::AdvanceSelection(bool bForward)
{
	m_pages->AdvanceSelection(bForward);
}

int wxFlatNotebook::GetPageCount() const
{
	return (int)m_pages->GetPageCount();
}

void wxFlatNotebook::OnNavigationKey(wxNavigationKeyEvent& event)
{
	if ( event.IsWindowChange() )
	{
		// change pages
		AdvanceSelection(event.GetDirection());
	}
	else
	{
		// pass to the parent
		if ( GetParent() )
		{
			event.SetCurrentFocus(this);
			GetParent()->ProcessEvent(event);
		}
	}
}

bool wxFlatNotebook::GetPageShapeAngle(int page_index, unsigned int * result)
{
	if(page_index < 0 || page_index >= (int)m_pages->m_pagesInfoVec.size()) return false;
	*result = m_pages->m_pagesInfoVec[page_index].GetTabAngle();
	return true;
}

void wxFlatNotebook::SetPageShapeAngle(int page_index, unsigned int angle)
{
	if(page_index < 0 || page_index >= (int)m_pages->m_pagesInfoVec.size()) return;
	m_pages->m_pagesInfoVec[page_index].SetTabAngle(angle);
}

void wxFlatNotebook::SetAllPagesShapeAngle(unsigned int angle)
{
	for(unsigned int i = 0; i < m_pages->m_pagesInfoVec.size(); i++)
	{
		m_pages->m_pagesInfoVec[i].SetTabAngle(angle);
	}
	Refresh();
}

wxSize wxFlatNotebook::GetPageBestSize()
{
	return m_pages->GetClientSize();
}

bool wxFlatNotebook::SetPageText(size_t page, const wxString& text)
{
	bool bVal = m_pages->SetPageText(page, text);
	m_pages->Refresh();
	return bVal;
}

void wxFlatNotebook::SetPadding(const wxSize& padding)
{
	m_nPadding = padding.GetWidth();
}

void wxFlatNotebook::SetBookStyle(long style)
{
	m_pages->m_nStyle = style;
	m_pages->Refresh();

	// For changing the tab position (i.e. placing them top/bottom)
	// refreshing the tab container is not enough
	SetSelection(m_pages->m_iActivePage);
}

long wxFlatNotebook::GetBookStyle()
{
	return m_pages->m_nStyle;
}

bool wxFlatNotebook::RemovePage(size_t page, bool notify)
{
	if(page >= m_windows.size())
		return false;

    if (notify)
    {
	// Fire a closing event
	wxFlatNotebookEvent event(wxEVT_COMMAND_FLATNOTEBOOK_PAGE_CLOSING, GetId());
	event.SetSelection((int)page);
	event.SetEventObject(this);
	GetEventHandler()->ProcessEvent(event);

	// The event handler allows it?
    if (!event.IsAllowed())
        return false;
    }

	Freeze();

	// Remove the requested page
	wxWindow *pageRemoved = m_windows[page];

	// If the page is the current window, remove it from the sizer
	// as well
	if((int)page == m_pages->GetSelection())
	{
		m_mainSizer->Detach(pageRemoved);
	}

	// The page is only removed, not deleted.
	// pageRemoved->Destory();

	// Remove it from the array as well
	std::vector<wxWindow*>::iterator iter = std::find(m_windows.begin(), m_windows.end(), pageRemoved);
	if(iter != m_windows.end())
		m_windows.erase(iter);

	Thaw();

	m_pages->DoDeletePage(page);

	return true;
}

void wxFlatNotebook::SetRightClickMenu(wxMenu* menu)
{
	m_pages->m_pRightClickMenu = menu;
}

wxString wxFlatNotebook::GetPageText(size_t page)
{
	return m_pages->GetPageText(page);
}

void wxFlatNotebook::SetGradientColors(const wxColour& from, const wxColour& to, const wxColour& border)
{
	m_pages->m_colorFrom = from;
	m_pages->m_colorTo   = to;
	m_pages->m_colorBorder = border;
}

void wxFlatNotebook::SetGradientColorFrom(const wxColour& from)
{
	m_pages->m_colorFrom = from;
}

void wxFlatNotebook::SetGradientColorTo(const wxColour& to)
{
	m_pages->m_colorTo   = to;
}

void wxFlatNotebook::SetGradientColorBorder(const wxColour& border)
{
	m_pages->m_colorBorder = border;
}

void wxFlatNotebook::SetPageImageIndex(size_t page, int imgindex)
{
	m_pages->SetPageImageIndex(page, imgindex);
}

int wxFlatNotebook::GetPageImageIndex(size_t page)
{
	return m_pages->GetPageImageIndex(page);
}

///////////////////////////////////////////////////////////////////////////////////////////
//
//	wxPageContainer
//
///////////////////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(wxPageContainer, wxControl)
EVT_PAINT(wxPageContainer::OnPaint)
EVT_SIZE(wxPageContainer::OnSize)
EVT_LEFT_DOWN(wxPageContainer::OnLeftDown)
EVT_RIGHT_DOWN(wxPageContainer::OnRightDown)
EVT_MIDDLE_DOWN(wxPageContainer::OnMiddleDown)
EVT_MOTION(wxPageContainer::OnMouseMove)
EVT_ERASE_BACKGROUND(wxPageContainer::OnEraseBackground)
EVT_LEAVE_WINDOW(wxPageContainer::OnMouseLeave)
END_EVENT_TABLE()



wxPageContainer::wxPageContainer(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
: m_ImageList(NULL)
, m_iActivePage(-1)
, m_bHoverX(false)
, m_bHoverLeftArrow(false)
, m_bHoverRightArrow(false)
, m_pDropTarget(NULL)
, m_pParent(parent)
, m_pRightClickMenu(NULL)
{
	m_colorTo = wxColor(255, 255, 255);
	m_colorFrom   = wxColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
	m_colorBorder = wxColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW));

	wxWindow::Create(parent, id, pos, size, style | wxNO_BORDER | wxNO_FULL_REPAINT_ON_RESIZE);
	m_pDropTarget = new CTextDropTarget<wxPageContainer>(this, &wxPageContainer::OnTextDropTarget);
	SetDropTarget(m_pDropTarget);
}

wxPageContainer::~wxPageContainer(void)
{
	if(m_pRightClickMenu)
	{
		delete m_pRightClickMenu;
		m_pRightClickMenu = NULL;
	}
}

void wxPageContainer::OnPaint(wxPaintEvent &event)
{
	const int TABS_LEFT_SPACE = 3;

	wxBufferedPaintDC dc(this);
	if(m_pagesInfoVec.empty() || m_nFrom >= (int)m_pagesInfoVec.size())
	{
		Hide();
		event.Skip();
		return;
	}

	// Get the text hight
	int height, width, tabHeight;
	wxString stam = wxT("Tp");	//Temp data to get the text height;
	dc.GetTextExtent(stam, &width, &height);

	tabHeight = height + 8; // We use 6 pixels as padding

	// Calculate the number of rows required for drawing the tabs
	wxRect rect = GetClientRect();
	int clientWidth = rect.width;

	// Set the maximum client size
	SetSizeHints(wxSize(BUTTONS_AREA_WIDTH, tabHeight));

	// Set brushes, pens and fonts
	wxFont normalFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	wxFont boldFont = normalFont;
	boldFont.SetWeight(wxFONTWEIGHT_BOLD);

	wxPen borderPen = wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW));

	wxBrush backBrush;
	if(m_nStyle & wxFNB_VC71)
		backBrush = wxBrush(wxColour(247, 243, 233));
	else
		backBrush = wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_3DLIGHT));

	wxBrush noselBrush = wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
	wxBrush selBrush = wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNHIGHLIGHT));

	dc.BeginDrawing();
	wxSize size = GetSize();

	// Background
	dc.SetTextBackground(m_nStyle & wxFNB_VC71 ? wxColour(247, 243, 233) : GetBackgroundColour());
	dc.SetTextForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT));
	dc.SetBrush(backBrush);
	dc.SetPen(borderPen);
	dc.DrawRectangle(0, 0, size.x, size.y);

	if(m_nStyle & wxFNB_VC71)
	{
		wxPen pen = wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
		pen.SetWidth(4);
		dc.SetPen(pen);
		dc.DrawLine(0, size.y, size.x, size.y);

		// Restore the pen
		dc.SetPen(borderPen);
	}

	// Draw labels
	int pom;
	dc.SetFont(boldFont);
	int posx = ((wxFlatNotebook *)m_pParent)->m_nPadding;
	int i = 0;

	// Update all the tabs from 0 to 'm_nFrom' to be non visible
	for(i=0; i<m_nFrom; i++)
		m_pagesInfoVec[i].SetPosition(wxPoint(-1, -1));

	wxPoint tabPoints[7];
	int shapePoints(0);

	if(m_nStyle & wxFNB_VC71)
		tabHeight = (m_nStyle & wxFNB_BOTTOM) ? tabHeight - 4 :  tabHeight;
	else if(m_nStyle & wxFNB_FANCY_TABS)
		tabHeight = (m_nStyle & wxFNB_BOTTOM) ? tabHeight - 3 :  tabHeight;

	// Draw the visible tabs
	for(i=m_nFrom; i<(int)m_pagesInfoVec.size(); i++)
	{
		if(m_nStyle != wxFNB_VC71)
			shapePoints = (int)(tabHeight*tan((double)m_pagesInfoVec[i].GetTabAngle()/180.0*M_PI));
		else
			shapePoints = 0;

		dc.SetPen(borderPen);
		dc.SetFont((i==GetSelection()) ? boldFont : normalFont);
		dc.SetBrush((i==GetSelection()) ? selBrush : noselBrush);
		dc.GetTextExtent(GetPageText(i), &width, &pom);

		// Set a minimum size to a tab
		if(width < 20)
			width = 20;

		// Add the padding to the tab width
		int tabWidth = ((wxFlatNotebook *)m_pParent)->m_nPadding * 2 + width;
		int imageYCoord = m_nStyle & wxFNB_BOTTOM ? 3 : 6;

		if(!(m_nStyle & wxFNB_VC71))
			// Default style
			tabWidth += 2 * shapePoints;

		bool hasImage = (m_ImageList != NULL);
		if(hasImage) hasImage &= m_pagesInfoVec[i].GetImageIndex() != -1;

		// For VC71 style, we only add the icon size (16 pixels)
		if(hasImage && ( (m_nStyle & wxFNB_VC71) || (m_nStyle & wxFNB_FANCY_TABS)) )
			tabWidth += 16;
		else
			// Default style
			tabWidth += 16 + shapePoints / 2;

		// Check if we can draw more
		if(posx + tabWidth + BUTTONS_AREA_WIDTH >= clientWidth)
			break;

		if(m_nStyle & wxFNB_FANCY_TABS)
		{
			// Fancy tabs - like with VC71 but with the following differences:
			// - The Selected tab is colored with gradient color
			wxPen pen = (i==GetSelection()) ? wxPen(m_colorBorder) : wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
			dc.SetPen(pen);
			dc.SetBrush((i==GetSelection()) ? wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE)) : wxBrush(wxColour(247, 243, 233)));
			if(i == GetSelection())
			{
				int posy = (m_nStyle & wxFNB_BOTTOM) ? 0 : TABS_LEFT_SPACE;


				wxRect rect(posx, posy, tabWidth, tabHeight);
				FillGradientColor(dc, rect);
				dc.SetBrush(*wxTRANSPARENT_BRUSH);
				pen.SetWidth(1);
				dc.SetPen(pen);

				dc.DrawRectangle(rect);
				pen.SetWidth(1);

				// Incase we are drawing a bottom tab, remove the upper line of the rectangle
				if(m_nStyle & wxFNB_BOTTOM)
				{
					dc.SetPen(wxPen(m_colorFrom));
					dc.DrawLine(posx, posy, posx + tabWidth, posy);
				}
				dc.SetPen(pen);
			}
			else
			{
				// We dont draw a rectangle for non selected tabs, but only
				// vertical line on the left
				dc.SetPen(borderPen);
				dc.DrawLine(posx + tabWidth, TABS_LEFT_SPACE + 3, posx + tabWidth, tabHeight - 4);
			}

			// Draw the image for the tab if any
			if(hasImage)
			{
				dc.DrawBitmap((*m_ImageList)[m_pagesInfoVec[i].GetImageIndex()],
					posx + ((wxFlatNotebook *)m_pParent)->m_nPadding + shapePoints, imageYCoord, true);
			}
		}
		else if(m_nStyle & wxFNB_VC71)
		{
			// Visual studio 7.1 style
			dc.SetPen((i==GetSelection()) ? wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE)) : borderPen);
			dc.SetBrush((i==GetSelection()) ? wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE)) : wxBrush(wxColour(247, 243, 233)));

			if(i == GetSelection())
			{
				int posy = (m_nStyle & wxFNB_BOTTOM) ? 0 : TABS_LEFT_SPACE;
				dc.DrawRectangle(posx, posy, tabWidth, tabHeight);

				// Draw a black line on the left side of the
				// rectangle
				wxPen pen = wxPen(*wxBLACK);
				dc.SetPen(pen);

				int blackLineHeight = (m_nStyle & wxFNB_BOTTOM) ? TABS_LEFT_SPACE + tabHeight - 3 : TABS_LEFT_SPACE + tabHeight - 5;
				dc.DrawLine(posx + tabWidth, (m_nStyle & wxFNB_BOTTOM) ? 0 : TABS_LEFT_SPACE, posx + tabWidth, blackLineHeight + 1);

				// To give the tab more 3D look we do the following
				// Incase the tab is on top,
				// Draw a thik white line on topof the rectangle
				// Otherwise, draw a thin (1 pixel) black line at the bottom

				pen = wxPen((m_nStyle & wxFNB_BOTTOM) ? *wxBLACK : *wxWHITE);
				pen.SetWidth((m_nStyle & wxFNB_BOTTOM) ? 1 : 2);
				dc.SetPen(pen);
				int whiteLinePosY = (m_nStyle & wxFNB_BOTTOM) ? blackLineHeight : TABS_LEFT_SPACE ;
				dc.DrawLine(posx , whiteLinePosY, posx + tabWidth, whiteLinePosY);
			}
			else
			{
				// We dont draw a rectangle for non selected tabs, but only
				// vertical line on the left
				int blackLineHeight = (m_nStyle & wxFNB_BOTTOM) ? tabHeight - 5 : TABS_LEFT_SPACE + tabHeight - 8;
				dc.DrawLine(posx + tabWidth, (m_nStyle & wxFNB_BOTTOM) ? 3 : TABS_LEFT_SPACE + 1, posx + tabWidth, blackLineHeight + 1);
			}

			// Draw the image for the tab if any
			if(hasImage)
			{
				dc.DrawBitmap((*m_ImageList)[m_pagesInfoVec[i].GetImageIndex()],
					posx + ((wxFlatNotebook *)m_pParent)->m_nPadding + shapePoints, imageYCoord, true);
			}
		}
		else
		{
			// Default style
			tabPoints[0].x = posx;
			tabPoints[0].y = (m_nStyle & wxFNB_BOTTOM) ? 0 : TABS_LEFT_SPACE+tabHeight;

			tabPoints[1].x = (int)(posx+(tabHeight-2)*tan((double)m_pagesInfoVec[i].GetTabAngle()/180.0*M_PI));
			tabPoints[1].y = (m_nStyle & wxFNB_BOTTOM) ? tabHeight - (TABS_LEFT_SPACE+2) : (TABS_LEFT_SPACE+2);

			tabPoints[2].x = tabPoints[1].x+2;
			tabPoints[2].y = (m_nStyle & wxFNB_BOTTOM) ? tabHeight - TABS_LEFT_SPACE : TABS_LEFT_SPACE;

			tabPoints[3].x = (int)(posx+tabWidth-(tabHeight-2)*tan((double)m_pagesInfoVec[i].GetTabAngle()/180.0*M_PI))-2;
			tabPoints[3].y = (m_nStyle & wxFNB_BOTTOM) ? tabHeight - TABS_LEFT_SPACE : TABS_LEFT_SPACE;

			tabPoints[4].x = tabPoints[3].x+2;
			tabPoints[4].y = (m_nStyle & wxFNB_BOTTOM) ? tabHeight - (TABS_LEFT_SPACE+2) : (TABS_LEFT_SPACE+2);

			tabPoints[5].x = posx+tabWidth;
			tabPoints[5].y = (m_nStyle & wxFNB_BOTTOM) ? 0 : TABS_LEFT_SPACE+tabHeight;

			tabPoints[6].x = tabPoints[0].x;
			tabPoints[6].y = tabPoints[0].y;

			// Draw the tab as rounded rectangle
			dc.DrawPolygon(7, tabPoints);

			if(m_nStyle & wxFNB_BOTTOM && i == GetSelection())
			{
				wxPen savePen = dc.GetPen();
				wxPen whitePen = wxPen(*wxWHITE);
				whitePen.SetWidth(2);
				dc.SetPen(whitePen);

				dc.DrawLine(tabPoints[0], tabPoints[5]);

				// Restore the pen
				dc.SetPen(savePen);
			}

			// Draw the image for the tab if any
			if(hasImage)
			{
				dc.DrawBitmap((*m_ImageList)[m_pagesInfoVec[i].GetImageIndex()],
					posx + ((wxFlatNotebook *)m_pParent)->m_nPadding + shapePoints, imageYCoord, true);
			}

			// for non-selected tabs we draw a bottom line
			if(i != GetSelection())
				dc.DrawLine(posx, rect.height-1, posx+tabWidth, rect.height-1);
		}

		// Text drawing offset from the left border of the
		// rectangle
		int offset;

		// The width of the images are 16 pixels
		offset = hasImage ? (16+((wxFlatNotebook *)m_pParent)->m_nPadding)+2 : 8;

		if((m_nStyle & wxFNB_VC71 || m_nStyle & wxFNB_FANCY_TABS) && i != GetSelection())
		{
			// Set the text background to be like the vertical lines (light brown)
			dc.SetTextForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_APPWORKSPACE));
		}
		dc.DrawText(GetPageText(i), posx+offset+shapePoints, imageYCoord);

		// Restore the text forground
		dc.SetTextForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT));

		// Update the tab position & size
		m_pagesInfoVec[i].SetPosition(wxPoint(posx, TABS_LEFT_SPACE));
		m_pagesInfoVec[i].SetSize(wxSize(tabWidth, tabHeight));

		posx += tabWidth -1;
	}

	// Update all tabs that can not fit into the screen as non-visible
	for(; i<(int)m_pagesInfoVec.size(); i++)
		m_pagesInfoVec[i].SetPosition(wxPoint(-1, -1));

	// Draw the left/right/close buttons
	// Left arrow
	DrawLeftArrow(m_bHoverLeftArrow, dc);
	DrawRightArrow(m_bHoverRightArrow, dc);
	DrawX(m_bHoverX, dc);
	dc.EndDrawing();

	// No need to skip this event;
	//event.Skip();
}

void wxPageContainer::AddPage(const wxString& caption, const bool selected, const int imgindex)
{
	if(selected)
	{
		m_iActivePage = (int)m_pagesInfoVec.size();
	}
	m_pagesInfoVec.push_back(wxPageInfo(caption, imgindex));
	Refresh();
}

bool wxPageContainer::InsertPage(size_t index, wxWindow* /*page*/, const wxString& text, bool select, const int imgindex)
{
	if(select)
	{
		m_iActivePage = (int)m_pagesInfoVec.size();
	}
	std::vector<wxPageInfo>::iterator iter = m_pagesInfoVec.begin() + index;
	m_pagesInfoVec.insert(iter, wxPageInfo(text, imgindex));
	Refresh();
	return true;
}

void wxPageContainer::OnSize(wxSizeEvent& WXUNUSED(event))
{
	Refresh(); // Call on paint
}

void wxPageContainer::OnMiddleDown(wxMouseEvent& event)
{
	// Test if this style is enabled
	if(!(m_nStyle & wxFNB_MOUSE_MIDDLE_CLOSES_TABS))
		return;

	wxPageInfo pgInfo;
	int tabIdx;
	int where = HitTest(event.GetPosition(), pgInfo, tabIdx);
	switch(where)
	{
	case wxFNB_TAB:
		{
			wxFlatNotebookEvent event(wxEVT_COMMAND_FLATNOTEBOOK_PAGE_CLOSING, GetParent()->GetId());
			event.SetSelection(tabIdx);
			event.SetEventObject(GetParent());
			GetParent()->GetEventHandler()->ProcessEvent(event);
			if (event.IsAllowed())
			{
			// Set the current tab to be active
			SetSelection((size_t)tabIdx);
                DeletePage((size_t)tabIdx, false);
			}
			break;
		}
	default:
		break;
	}
	event.Skip();
}

void wxPageContainer::OnRightDown(wxMouseEvent& event)
{
		wxPageInfo pgInfo;
		int tabIdx;
		int where = HitTest(event.GetPosition(), pgInfo, tabIdx);
		switch(where)
		{
		case wxFNB_TAB:
        {
			// Set the current tab to be active
			SetSelection((size_t)tabIdx);

            // If the owner has defined a context menu for the tabs,
            // popup the right click menu
            if (m_pRightClickMenu)
                PopupMenu(m_pRightClickMenu);
            else
            {
                // send a message to popup a custom menu
                wxFlatNotebookEvent event(wxEVT_COMMAND_FLATNOTEBOOK_CONTEXT_MENU, GetParent()->GetId());
                event.SetSelection((int)tabIdx);
                event.SetOldSelection((int)m_iActivePage);
                event.SetEventObject(GetParent());
                GetParent()->GetEventHandler()->ProcessEvent(event);
            }
        }
			break;
		default:
			break;
		}
	event.Skip();
}

void wxPageContainer::OnLeftDown(wxMouseEvent& event)
{
	wxPageInfo pgInfo;
	int tabIdx;
	int where = HitTest(event.GetPosition(), pgInfo, tabIdx);
	switch(where)
	{
	case wxFNB_LEFT_ARROW:
		{
			if(m_nFrom == 0)
				break;

			// We scroll left with bulks of 5
			int scrollLeft = GetNumTabsCanScrollLeft();

			m_nFrom -= scrollLeft;
			if(m_nFrom < 0)
				m_nFrom = 0;

			Refresh();
			break;
		}
	case wxFNB_RIGHT_ARROW:
		{
			if(m_nFrom >= (int)m_pagesInfoVec.size() - 1)
				break;

			// Check if the right most tab is visible, if it is
			// don't rotate right anymore
			if(m_pagesInfoVec[m_pagesInfoVec.size()-1].GetPosition() != wxPoint(-1, -1))
				break;

			int lastVisibleTab = GetLastVisibleTab();
			if(lastVisibleTab < 0)
			{
				// Probably the screen is too small for displaying even a single
				// tab, in this case we do nothing
				break;
			}

			m_nFrom += GetNumOfVisibleTabs();
			Refresh();
			break;
		}
	case wxFNB_TAB:
		{
			if(m_iActivePage != tabIdx)
			{
				wxFlatNotebookEvent event(wxEVT_COMMAND_FLATNOTEBOOK_PAGE_CHANGING, GetParent()->GetId());
				event.SetSelection((int)tabIdx);
				event.SetOldSelection((int)m_iActivePage);
				event.SetEventObject(GetParent());
				if(!GetParent()->GetEventHandler()->ProcessEvent(event) || event.IsAllowed())
				{
					SetSelection(tabIdx);

					// Fire a wxEVT_COMMAND_TABBEDCTRL_PAGE_CHANGED event
					event.SetEventType(wxEVT_COMMAND_FLATNOTEBOOK_PAGE_CHANGED);
					event.SetOldSelection((int)m_iActivePage);
					GetParent()->GetEventHandler()->ProcessEvent(event);
				}
			}
			break;
		}
	case wxFNB_X:
		{
			wxFlatNotebookEvent event(wxEVT_COMMAND_FLATNOTEBOOK_PAGE_CLOSING, GetParent()->GetId());
			event.SetSelection(m_iActivePage);
			event.SetEventObject(GetParent());
			GetParent()->GetEventHandler()->ProcessEvent(event);
			if (event.IsAllowed())
                DeletePage((size_t)m_iActivePage, false);
			break;
		}
	}
	event.Skip();
}

int wxPageContainer::HitTest(const wxPoint& pt, wxPageInfo& pageInfo, int &tabIdx)
{
	wxRect rect = GetClientRect();
	int clientWidth = rect.width;
	int btnLeftPos = clientWidth - 54;
	int btnRightPos = clientWidth - 38;
	int btnXPos = clientWidth - 22;
	tabIdx = -1;
	if(m_pagesInfoVec.empty())
	{
		return wxFNB_NOWHERE;
	}

	rect = wxRect(btnXPos, 5, 12, 12);
	if(rect.Inside(pt))
	{
		return (m_nStyle & wxFNB_NO_X_BUTTON) ? wxFNB_NOWHERE : wxFNB_X;
	}

	rect = wxRect(btnRightPos, 5, 12, 12);
	if(rect.Inside(pt))
	{
		return (m_nStyle & wxFNB_NO_NAV_BUTTONS) ? wxFNB_NOWHERE : wxFNB_RIGHT_ARROW;
	}
	rect = wxRect(btnLeftPos, 5, 12, 12);
	if(rect.Inside(pt))
	{
		return (m_nStyle & wxFNB_NO_NAV_BUTTONS) ? wxFNB_NOWHERE : wxFNB_LEFT_ARROW;
	}

	// Test whether a left click was made on a tab
	for(size_t cur=m_nFrom; cur<m_pagesInfoVec.size(); cur++)
	{
		wxPageInfo pgInfo = m_pagesInfoVec[cur];
		if(pgInfo.GetPosition() == wxPoint(-1, -1))
			continue;

		wxRect tabRect = wxRect(pgInfo.GetPosition().x, pgInfo.GetPosition().y,
			pgInfo.GetSize().x, pgInfo.GetSize().y);
		if(tabRect.Inside(pt))
		{
			// We have a match
			// wxMessageBox(pgInfo.m_strCaption);
			pageInfo = pgInfo;
			tabIdx = (int)cur;
			return wxFNB_TAB;
		}
	}
	// Default
	return wxFNB_NOWHERE;
}

void wxPageContainer::SetSelection(size_t page)
{
	wxFlatNotebook* book = (wxFlatNotebook*)GetParent();
	book->SetSelection(page);
	DoSetSelection(page);
}

void wxPageContainer::DoSetSelection(size_t page)
{
	// Make sure that the selection is visible
	if(m_nStyle & wxFNB_NO_NAV_BUTTONS)
	{
		// Incase that we dont have navigation buttons,
		// there is no point of checking if the tab is visible
		// Just do the refresh
		Refresh();
		return;
	}

	if(!IsTabVisible(page))
	{
		if(page == m_pagesInfoVec.size() - 1)
		{
			// Incase the added tab is last,
			// the function IsTabVisible() will always return false
			// and thus will cause an evil behaviour that the new
			// tab will hide all other tabs, we need to check if the
			// new selected tab can fit to the current screen
			if(!CanFitToScreen(page))
			{
				m_nFrom = (int)page;
			}
			Refresh();
		}
		else
		{
			// Redraw the tabs starting from page
            m_nFrom = (int)page;
            Refresh();
		}
	}
}

void wxPageContainer::DeletePage(size_t page, bool notify)
{
	wxFlatNotebook* book = (wxFlatNotebook*)GetParent();
	book->DeletePage(page, notify);
	book->Refresh();
}

bool wxPageContainer::IsTabVisible(size_t page)
{
	int iPage = (int)page;
	int iLastVisiblePage = GetLastVisibleTab();

	return iPage <= iLastVisiblePage && iPage >= m_nFrom;
}

void wxPageContainer::DoDeletePage(size_t page)
{
	// Remove the page from the vector
	wxFlatNotebook* book = (wxFlatNotebook*)GetParent();
	std::vector<wxPageInfo>::iterator iter = m_pagesInfoVec.begin();
	std::vector<wxPageInfo>::iterator endIter = m_pagesInfoVec.end();

	m_pagesInfoVec.erase(iter + page);
	m_iActivePage--;

	// The delete page was the last first on the array,
	// but the book still has more pages, so we set the
	// active page to be the first one (0)
	if(m_iActivePage < 0 && !m_pagesInfoVec.empty())
		m_iActivePage = 0;

	// Refresh the tabs
	if(m_iActivePage >= 0)
		book->SetSelection(m_iActivePage);

	if(m_pagesInfoVec.empty())
	{
		// Erase the page container drawings
		wxClientDC dc(this);
		dc.Clear();
	}
}

void wxPageContainer::DeleteAllPages()
{
	m_iActivePage = -1;
	m_nFrom = 0;
	m_pagesInfoVec.clear();

	// Erase the page container drawings
	wxClientDC dc(this);
	dc.Clear();
}

void wxPageContainer::DrawLeftArrow(bool hover, wxDC& dc)
{
	if(m_nStyle & wxFNB_NO_NAV_BUTTONS)
		return;

	// Make sure that there are pages in the container
	if(m_pagesInfoVec.empty())
		return;

	if(hover)
	{
		if(m_nFrom == 0)
		{
			// First draw the button as diabled, then return
			// Erase any previous drawing
			wxRect rect = GetClientRect();
			int clientWidth = rect.width;
			int btnLeftPos = clientWidth - 54;
			rect = wxRect(btnLeftPos, 5, 12, 12);

			wxPen pen = wxPen(m_nStyle & wxFNB_VC71 ? wxColour(247, 243, 233) : wxSystemSettings::GetColour(wxSYS_COLOUR_3DLIGHT));
			wxBrush brush = wxBrush(m_nStyle & wxFNB_VC71 ? wxColour(247, 243, 233) : wxSystemSettings::GetColour(wxSYS_COLOUR_3DLIGHT));
			dc.SetPen(pen);
			dc.SetBrush(brush);

			dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);
			wxPoint points[] = { wxPoint(rect.x + 3, rect.y + 6), wxPoint(rect.x + 7, rect.y + 2), wxPoint(rect.x + 7, rect.y + 10) };

			pen   = wxPen(wxColor(128, 128, 128));
			dc.SetPen(pen);
			dc.SetBrush(brush);

			dc.DrawPolygon(3, points);
			return;
		}

		wxPen pen = wxPen(*wxWHITE);
		wxBrush brush = wxBrush(*wxRED);
		dc.SetPen(pen);
		dc.SetBrush(brush);

		// Draw a white rectangle around the arrow
		wxRect rect = GetClientRect();

		int clientWidth = rect.width;
		int btnLeftPos = clientWidth - 54;
		rect = wxRect(btnLeftPos, 5, 12, 12);
		dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);

		wxPoint points[] = { wxPoint(rect.x + 3, rect.y + 6), wxPoint(rect.x + 7, rect.y + 2), wxPoint(rect.x + 7, rect.y + 10) };

		brush = wxBrush(*wxWHITE);
		dc.SetBrush(brush);
		dc.DrawPolygon(3, points);
	}
	else
	{
		// Erase any previous drawing
		wxRect rect = GetClientRect();

		// Draw the black triangle
		int clientWidth = rect.width;
		int btnLeftPos = clientWidth - 54;
		rect = wxRect(btnLeftPos, 5, 12, 12);

		wxPen pen = wxPen(m_nStyle & wxFNB_VC71 ? wxColour(247, 243, 233) : wxSystemSettings::GetColour(wxSYS_COLOUR_3DLIGHT));
		wxBrush brush = wxBrush(m_nStyle & wxFNB_VC71 ? wxColour(247, 243, 233) : wxSystemSettings::GetColour(wxSYS_COLOUR_3DLIGHT));
		dc.SetPen(pen);
		dc.SetBrush(brush);

		dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);
		wxPoint points[] = { wxPoint(rect.x + 3, rect.y + 6), wxPoint(rect.x + 7, rect.y + 2), wxPoint(rect.x + 7, rect.y + 10) };

		if(m_nFrom == 0)
		{
			pen   = wxPen(wxColor(128, 128, 128));
		}
		else
		{
			pen   = wxPen(wxColor(128, 128, 128));
			brush = wxBrush(wxColor(128, 128, 128));
		}
		dc.SetPen(pen);
		dc.SetBrush(brush);

		dc.DrawPolygon(3, points);
	}
}

void wxPageContainer::DrawRightArrow(bool hover, wxDC& dc)
{
	if(m_nStyle & wxFNB_NO_NAV_BUTTONS)
		return;

	// Make sure that there are pages in the container
	if(m_pagesInfoVec.empty())
		return;

	if(hover)
	{
		// Check if the right most tab is visible, if it is
		// don't rotate right anymore
		if(m_pagesInfoVec[m_pagesInfoVec.size()-1].GetPosition() != wxPoint(-1, -1))
		{
			// Draw the button as disabled
			wxRect rect = GetClientRect();
			int clientWidth = rect.width;
			int btnLeftPos = clientWidth - 38;
			rect = wxRect(btnLeftPos, 5, 12, 12);

			wxPen pen = wxPen(m_nStyle & wxFNB_VC71 ? wxColour(247, 243, 233) : wxSystemSettings::GetColour(wxSYS_COLOUR_3DLIGHT));
			wxBrush brush = wxBrush(m_nStyle & wxFNB_VC71 ? wxColour(247, 243, 233) :wxSystemSettings::GetColour(wxSYS_COLOUR_3DLIGHT));
			dc.SetPen(pen);
			dc.SetBrush(brush);
			dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);
			wxPoint points[] = { wxPoint(rect.x + 8, rect.y + 6), wxPoint(rect.x + 4, rect.y + 2), wxPoint(rect.x + 4, rect.y + 10) };
			pen   = wxPen(wxColor(128, 128, 128));
			dc.SetPen(pen);
			dc.SetBrush(brush);
			dc.DrawPolygon(3, points);
			return;
		}

		wxPen pen = wxPen(*wxWHITE);
		wxBrush brush = wxBrush(*wxRED);
		dc.SetPen(pen);
		dc.SetBrush(brush);

		// Draw a white rectangle around the arrow
		wxRect rect = GetClientRect();
		int clientWidth = rect.width;
		int btnLeftPos = clientWidth - 38;
		rect = wxRect(btnLeftPos, 5, 12, 12);
		dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);
		brush = wxBrush(*wxWHITE);
		wxPoint points[] = { wxPoint(rect.x + 8, rect.y + 6), wxPoint(rect.x + 4, rect.y + 2), wxPoint(rect.x + 4, rect.y + 10) };
		dc.SetBrush(brush);
		dc.DrawPolygon(3, points);
	}
	else
	{
		wxRect rect = GetClientRect();
		int clientWidth = rect.width;
		int btnLeftPos = clientWidth - 38;
		rect = wxRect(btnLeftPos, 5, 12, 12);

		wxPen pen = wxPen(m_nStyle & wxFNB_VC71 ? wxColour(247, 243, 233) : wxSystemSettings::GetColour(wxSYS_COLOUR_3DLIGHT));
		wxBrush brush = wxBrush(m_nStyle & wxFNB_VC71 ? wxColour(247, 243, 233) : wxSystemSettings::GetColour(wxSYS_COLOUR_3DLIGHT));
		dc.SetPen(pen);
		dc.SetBrush(brush);
		dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);
		wxPoint points[] = { wxPoint(rect.x + 8, rect.y + 6), wxPoint(rect.x + 4, rect.y + 2), wxPoint(rect.x + 4, rect.y + 10) };
		if(m_pagesInfoVec[m_pagesInfoVec.size()-1].GetPosition() != wxPoint(-1, -1))
		{
			pen   = wxPen(wxColor(128, 128, 128));
		}
		else
		{
			pen   = wxPen(wxColor(128, 128, 128));
			brush = wxBrush(wxColor(128, 128, 128));
		}
		dc.SetPen(pen);
		dc.SetBrush(brush);
		dc.DrawPolygon(3, points);
	}
}
void wxPageContainer::DrawX(bool hover, wxDC& dc)
{
	// Check if this style is enabled
	if(m_nStyle & wxFNB_NO_X_BUTTON)
		return;

	// Make sure that there are pages in the container
	if(m_pagesInfoVec.empty())
		return;

	wxRect rect = GetClientRect();
	int clientWidth = rect.width;
	int btnLeftPos = clientWidth - 22;
	rect = wxRect(btnLeftPos, 5, 12, 12);
	if(hover)
	{
		wxBrush brush = wxBrush(*wxRED);
		wxPen pen = wxPen(*wxWHITE);
		pen.SetWidth(1);
		dc.SetPen(pen);
		dc.SetBrush(brush);

		dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);
		pen = wxPen(*wxWHITE);
		pen.SetWidth(2);
		dc.SetPen(pen);

		dc.DrawLine(rect.x + 2, rect.y + 2, rect.x + 9, rect.y + 9);
		dc.DrawLine(rect.x + 9, rect.y + 2, rect.x + 2, rect.y + 9);
	}
	else
	{
		// erase any rectangle drawn from previous drawings
		wxPen pen = wxPen(m_nStyle & wxFNB_VC71 ? wxColour(247, 243, 233) : wxSystemSettings::GetColour(wxSYS_COLOUR_3DLIGHT));
		wxBrush brush = wxBrush(m_nStyle & wxFNB_VC71 ? wxColour(247, 243, 233) :wxSystemSettings::GetColour(wxSYS_COLOUR_3DLIGHT));
		pen.SetWidth(1);
		dc.SetPen(pen);
		dc.SetBrush(brush);

		dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);

		// Draw black X with default background color
		pen = wxPen(*wxBLACK);
		pen.SetWidth(2);
		dc.SetPen(pen);
		dc.SetBrush(brush);

		dc.DrawLine(rect.x + 2, rect.y + 2, rect.x + 9, rect.y + 9);
		dc.DrawLine(rect.x + 9, rect.y + 2, rect.x + 2, rect.y + 9);
	}
}

void wxPageContainer::OnMouseMove(wxMouseEvent& event)
{
	// TODO:: Add here a tooltip support
	wxPageInfo dummy;
	int tabIdx;

	if(m_pagesInfoVec.empty())
	{
		m_bHoverX = false;
		m_bHoverLeftArrow = false;
		m_bHoverRightArrow = false;
		event.Skip();
		return;
	}

	int where = HitTest(event.GetPosition(), dummy, tabIdx);
	wxRect rect(m_pagesInfoVec[tabIdx].GetPosition(), m_pagesInfoVec[tabIdx].GetSize());
	switch(where)
	{
	case wxFNB_X:
		m_bHoverLeftArrow = false;
		m_bHoverRightArrow = false;
		m_bHoverX = true;
		break;
	case wxFNB_RIGHT_ARROW:
		m_bHoverLeftArrow = false;
		m_bHoverX = false;
		m_bHoverRightArrow = true;
		break;
	case wxFNB_LEFT_ARROW:
		m_bHoverLeftArrow = true;
		m_bHoverX = false;
		m_bHoverRightArrow = false;
		break;
	case wxFNB_TAB:
		// Call virtual method for showing tooltip
		ShowTabTooltip(tabIdx);
	case wxFNB_NOWHERE:
	default:
		m_bHoverX = false;
		m_bHoverLeftArrow = false;
		m_bHoverRightArrow = false;
		break;
	}

	if(event.LeftIsDown() && where == wxFNB_TAB)
	{
		wxString strData;
		strData = strData.Format(wxT("wxFlatNotebook tab page index: %d"), tabIdx);
		wxTextDataObject dragData(strData);
		wxDropSource dragSource(this);
		dragSource.SetData(dragData);
		wxDragResult result = dragSource.DoDragDrop(TRUE);
		if(result == wxDragMove)
		{
		}
	}

	wxClientDC dc(this);
	dc.BeginDrawing();

	DrawX(m_bHoverX, dc);
	DrawLeftArrow(m_bHoverLeftArrow, dc);
	DrawRightArrow(m_bHoverRightArrow, dc);

	dc.EndDrawing();
	event.Skip();
}

int wxPageContainer::GetLastVisibleTab()
{
	int i;
	for(i=m_nFrom; i<(int)m_pagesInfoVec.size(); i++)
	{
		if(m_pagesInfoVec[i].GetPosition() == wxPoint(-1, -1))
			break;
	}
	return (i-1);
}

int wxPageContainer::GetNumTabsCanScrollLeft()
{
	int i;

	// Reserved area for the buttons (<>x)
	wxRect rect = GetClientRect();
	int clientWidth = rect.width;
	int posx = ((wxFlatNotebook *)m_pParent)->m_nPadding, numTabs = 0, pom = 0;

	int tabWidth = 0;

	wxClientDC dc(this);

	// Incase we have error prevent crash
	if(m_nFrom < 0)
		return 0;

	for(i=m_nFrom; i>=0; i--)
	{
		dc.GetTextExtent(GetPageText(i), &tabWidth, &pom);
		tabWidth += 6;	// 3 pixles space from the tab borders for the text

		bool hasImage = (m_pagesInfoVec[i].GetImageIndex() != -1);
		if(hasImage)
			tabWidth += 16;

		if(posx + tabWidth + BUTTONS_AREA_WIDTH >= clientWidth)
		{
			break;
		}

		numTabs++;
		posx += tabWidth -1;
	}
	return numTabs;
}

void wxPageContainer::AdvanceSelection(bool bForward)
{
	int nSel = GetSelection();

	if(nSel < 0)
		return;

	int nMax = (int)GetPageCount() - 1;
	if ( bForward )
		SetSelection(nSel == nMax ? 0 : nSel + 1);
	else
		SetSelection(nSel == 0 ? nMax : nSel - 1);
}

void wxPageContainer::OnMouseLeave(wxMouseEvent& event)
{
	m_bHoverLeftArrow = false;
	m_bHoverX = false;
	m_bHoverRightArrow = false;

	wxClientDC dc(this);
	dc.BeginDrawing();

	DrawX(m_bHoverX, dc);
	DrawLeftArrow(m_bHoverLeftArrow, dc);
	DrawRightArrow(m_bHoverRightArrow, dc);

	dc.EndDrawing();
	event.Skip();
}

void wxPageContainer::ShowTabTooltip(int tabIdx)
{
	wxWindow *pWindow = ((wxFlatNotebook *)m_pParent)->GetPage(tabIdx);
	wxToolTip *pToolTip = pWindow->GetToolTip();
	if(pToolTip && pToolTip->GetWindow() == pWindow)
		SetToolTip(pToolTip->GetTip());
}

void wxPageContainer::FillGradientColor(wxBufferedDC& dc, const wxRect& rect)
{
	// gradient fill from colour 1 to colour 2 with top to bottom

	if(rect.height < 1 || rect.width < 1)
		return;

	int size = rect.height;

	// calculate gradient coefficients
	wxColour col2 = (m_nStyle & wxFNB_BOTTOM) ? m_colorTo : m_colorFrom;
	wxColour col1 = (m_nStyle & wxFNB_BOTTOM) ? m_colorFrom : m_colorTo;

	double rstep = double((col2.Red() -   col1.Red())) / double(size), rf = 0,
		gstep = double((col2.Green() - col1.Green())) / double(size), gf = 0,
		bstep = double((col2.Blue() -  col1.Blue())) / double(size), bf = 0;

	wxColour currCol;
	for(int y = rect.y; y < rect.y + size; y++)
	{
		currCol.Set(
			(unsigned char)(col1.Red() + rf),
			(unsigned char)(col1.Green() + gf),
			(unsigned char)(col1.Blue() + bf)
			);
		dc.SetBrush( wxBrush( currCol, wxSOLID ) );
		dc.SetPen(wxPen(currCol));
		dc.DrawLine(rect.x, y, rect.x + rect.width, y);
		rf += rstep; gf += gstep; bf += bstep;
	}
}

void wxPageContainer::SetPageImageIndex(size_t page, int imgindex)
{
	if(page < m_pagesInfoVec.size())
	{
		m_pagesInfoVec[page].SetImageIndex(imgindex);
		Refresh();
	}
}

int wxPageContainer::GetPageImageIndex(size_t page)
{
	if(page < m_pagesInfoVec.size())
	{
		return m_pagesInfoVec[page].GetImageIndex();
	}
	return -1;
}

bool wxPageContainer::OnTextDropTarget(wxCoord x, wxCoord y, const wxString& data)
{
	int nIndex = -1;
	wxPageInfo pgInfo;
	if(data.Find(wxT("wxFlatNotebook tab page index: ")) == 0)
	{
		wxString str = data;
		int nTabPage = wxAtoi(str.Remove(0, 31));
		int where = HitTest(wxPoint(x, y), pgInfo, nIndex);
		if(nTabPage >= 0)
		{
			switch(where)
			{
			case wxFNB_TAB:
				MoveTabPage(nTabPage, nIndex);
				break;
			case wxFNB_NOWHERE:
				MoveTabPage(nTabPage, GetLastVisibleTab()+1);
				break;
			default:
				break;
			}
		}
	}
	return false;
}

void wxPageContainer::MoveTabPage(int nMove, int nMoveTo)
{
	if(nMove == nMoveTo)
		return;
	else if(nMoveTo < nMove)
		nMoveTo++;

	// Remove the window from the main sizer
	int nCurSel = ((wxFlatNotebook *)m_pParent)->m_pages->GetSelection();
	((wxFlatNotebook *)m_pParent)->m_mainSizer->Detach(((wxFlatNotebook *)m_pParent)->m_windows[nCurSel]);
	((wxFlatNotebook *)m_pParent)->m_windows[nCurSel]->Hide();

	wxWindow *pWindow = ((wxFlatNotebook *)m_pParent)->m_windows[nMove];
	((wxFlatNotebook *)m_pParent)->m_windows.erase(((wxFlatNotebook *)m_pParent)->m_windows.begin() + nMove);
	((wxFlatNotebook *)m_pParent)->m_windows.insert(((wxFlatNotebook *)m_pParent)->m_windows.begin() + nMoveTo - 1, pWindow);

	wxPageInfo pgInfo = m_pagesInfoVec[nMove];
	m_pagesInfoVec.erase(m_pagesInfoVec.begin() + nMove);
	m_pagesInfoVec.insert(m_pagesInfoVec.begin() + nMoveTo - 1, pgInfo);

	// Add the page according to the style
	wxBoxSizer* pSizer = ((wxFlatNotebook *)m_pParent)->m_mainSizer;

	if(m_nStyle & wxFNB_BORDER)
	{
		if(m_nStyle & wxFNB_BOTTOM)
		{
			pSizer->Insert(0, pWindow, 1, wxEXPAND|wxALL, 1);
		}
		else
		{
			// We leave a space of 1 pixel around the window
			pSizer->Add(pWindow, 1, wxEXPAND|wxALL, 1);
		}
	}
	else
	{
		if(m_nStyle & wxFNB_BOTTOM)
		{
			pSizer->Insert(0, pWindow, 1, wxEXPAND);
		}
		else
		{
			// We leave a space of 1 pixel around the window
			pSizer->Add(pWindow, 1, wxEXPAND);
		}
	}
	pWindow->Show();

	pSizer->Layout();
	m_iActivePage = nMoveTo-1;
	DoSetSelection(m_iActivePage);
	Refresh();
}

bool wxPageContainer::CanFitToScreen(size_t page)
{
	// Incase the from is greater than page,
	// we need to reset the m_nFrom, so in order
	// to force the caller to do so, we return false
	if(m_nFrom > (int)page)
		return false;

	// Calculate the tab width including borders and image if any
	wxClientDC dc(this);

	int width, pom, shapePoints, height, tabHeight;

	wxString stam = wxT("Tp");	// Temp data to get the text height;
	dc.GetTextExtent(stam, &width, &height);
	dc.GetTextExtent(GetPageText(page), &width, &pom);

	tabHeight = height + 8; // We use 6 pixels as padding

	if(m_nStyle & wxFNB_VC71)
		tabHeight = (m_nStyle & wxFNB_BOTTOM) ? tabHeight - 4 :  tabHeight;
	else if(m_nStyle & wxFNB_FANCY_TABS)
		tabHeight = (m_nStyle & wxFNB_BOTTOM) ? tabHeight - 3 :  tabHeight;

	int tabWidth = ((wxFlatNotebook *)m_pParent)->m_nPadding * 2 + width;

	if(m_nStyle != wxFNB_VC71)
		shapePoints = (int)(tabHeight*tan((double)m_pagesInfoVec[page].GetTabAngle()/180.0*M_PI));
	else
		shapePoints = 0;

	if(!(m_nStyle & wxFNB_VC71))
		// Default style
		tabWidth += 2 * shapePoints;

	bool hasImage = (m_ImageList != NULL);
	if(hasImage) hasImage &= m_pagesInfoVec[page].GetImageIndex() != -1;

	// For VC71 style, we only add the icon size (16 pixels)
	if(hasImage && ( (m_nStyle & wxFNB_VC71) || (m_nStyle & wxFNB_FANCY_TABS)) )
		tabWidth += 16;
	else
		// Default style
		tabWidth += 16 + shapePoints / 2;

	// Check if we can draw more
	int posx = ((wxFlatNotebook *)m_pParent)->m_nPadding;

	if(m_nFrom >= 0)
	{
		for(int i=m_nFrom; i<(int)m_pagesInfoVec.size(); i++)
		{
			if(m_pagesInfoVec[i].GetPosition() == wxPoint(-1, -1))
				break;
			posx += m_pagesInfoVec[i].GetSize().x;
		}
	}

	wxRect rect = GetClientRect();
	int clientWidth = rect.width;

	if(posx + tabWidth + BUTTONS_AREA_WIDTH >= clientWidth)
		return false;
	return true;
}

int wxPageContainer::GetNumOfVisibleTabs()
{
	int i=m_nFrom;
	int counter = 0;
	for(; i<(int)m_pagesInfoVec.size(); i++, ++counter)
	{
		if(m_pagesInfoVec[i].GetPosition() == wxPoint(-1, -1))
			break;
	}
	return counter;
}

