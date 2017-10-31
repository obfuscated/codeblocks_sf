#include "TextCtrl.h"
#include "NassiDiagramWindow.h"


BEGIN_EVENT_TABLE(TextCtrl, wxTextCtrl)
    EVT_TEXT(wxID_ANY, TextCtrl::OnText)
    EVT_MOUSEWHEEL(    TextCtrl::OnMouseWheel)

END_EVENT_TABLE()

TextCtrl::TextCtrl(wxWindow* parent, wxWindowID id, const wxString& value, const wxPoint& pos, const wxSize& size):
    wxTextCtrl(parent, id, value, pos, size, wxTE_MULTILINE | wxTE_RICH | wxTE_DONTWRAP | wxBORDER_NONE)
{}

TextCtrl::~TextCtrl()
{
    //dtor
}

void TextCtrl::SetOrigSize(wxSize size)
{
    m_origSize = size;
}

void TextCtrl::OnText(wxCommandEvent & /*event*/)
{
    wxClientDC dc(this);

    wxSize size;
    dc.SetFont(GetDefaultStyle().GetFont());

    size = dc.GetMultiLineTextExtent(GetValue());
    size.x += dc.GetCharWidth();
    size.y += dc.GetCharHeight();

    if ( size.x < m_origSize.x )
        size.x = m_origSize.x;
    if ( size.y < m_origSize.y )
        size.y = m_origSize.y;
    SetSize(size);

}

void TextCtrl::OnMouseWheel(wxMouseEvent &event)
{
    NassiDiagramWindow *wnd = dynamic_cast<NassiDiagramWindow *>(GetParent());

    if ( wnd )
    {
        event.Skip(false);
        wnd->OnMouseWheel(event);
    }
}

