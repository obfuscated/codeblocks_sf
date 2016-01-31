/***************************************************************
 * Name:      wxledpanel.cpp
 * Purpose:   Code for Class wxLEDPanel
 * Author:    Christian Gräfe (info@mcs-soft.de)
 * Created:   2007-02-28
 * Copyright: Christian Gräfe (www.mcs-soft.de)
 * License:      wxWindows licence
 **************************************************************/

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/dcbuffer.h>
#include "wx/wxledpanel.h"

#define TIMER_SCROLL_ID 1000

BEGIN_EVENT_TABLE(wxLEDPanel, wxControl)
    EVT_PAINT(wxLEDPanel::OnPaint)
    EVT_ERASE_BACKGROUND(wxLEDPanel::OnEraseBackground)
    EVT_TIMER(TIMER_SCROLL_ID,wxLEDPanel::OnScrollTimer)
END_EVENT_TABLE()

wxLEDPanel::wxLEDPanel() :
    m_align(wxALIGN_LEFT|wxALIGN_TOP),
    m_padLeft(1),
    m_padRight(1),
    m_invert(false),
    m_show_inactivs(true),
    m_scrollspeed(0),
    m_scrolldirection(wxALL),
    m_aniFrameNr(-1)
{
}

wxLEDPanel::wxLEDPanel(wxWindow* parent, wxWindowID id, const wxSize& ledsize,
                    const wxSize& fieldsize, int padding, const wxPoint& pos,
                    long style, const wxValidator& validator) :
    m_align(wxALIGN_LEFT|wxALIGN_TOP),
    m_padLeft(1),
    m_padRight(1),
    m_invert(false),
    m_show_inactivs(true),
    m_scrollspeed(0),
    m_scrolldirection(wxALL),
    m_aniFrameNr(-1)
{
    Create(parent,id,ledsize,fieldsize,padding,pos,style,validator);
}

wxLEDPanel::~wxLEDPanel()
{
}

bool wxLEDPanel::Create(wxWindow* parent, wxWindowID id, const wxSize& ledsize,
                    const wxSize& fieldsize, int padding, const wxPoint& pos,
                    long style, const wxValidator& validator)
{
    // save in member
    m_ledsize=ledsize;
    m_padding=padding;
    wxSize size;
    size.SetWidth((ledsize.GetWidth()+padding)*fieldsize.GetWidth()+padding);
    size.SetHeight((ledsize.GetHeight()+padding)*fieldsize.GetHeight()+padding);

    // create the control
    if(!wxControl::Create(parent,id,pos,size,style,validator))
        return false;

    // initialise MatrixObjekt
    m_field.Init(0,fieldsize.GetWidth(),fieldsize.GetHeight());

    // default backgroundcolor is black (call parent, to prevent the call of PrepareBackground)
    wxWindow::SetBackgroundColour(*wxBLACK);

    // default led-color is red
    this->SetLEDColour(wxLED_COLOUR_RED);

    // no Input Events
    this->Enable(false);

    // bind timer
    m_scrollTimer.SetOwner(this,TIMER_SCROLL_ID);

    return true;
}

wxSize wxLEDPanel::DoGetBestSize() const
{
    wxSize size;
    size.SetWidth((m_ledsize.GetWidth()+m_padding)*m_field.GetWidth()+m_padding);
    size.SetHeight((m_ledsize.GetHeight()+m_padding)*m_field.GetHeight()+m_padding);
    return size;
}

void wxLEDPanel::Clear()
{
    m_field.Clear();
}

void wxLEDPanel::Reset()
{
    SetText(m_text);
}

/** @return the size of the field in points */
wxSize wxLEDPanel::GetFieldsize() const
{
    return m_field.GetSize();
}

/** @return the size of one LED on the field */
wxSize wxLEDPanel::GetLEDSize() const
{
    return m_ledsize;
}

/**
* Sets the colour of the LEDs
* @param colourID the ID of the new colour
*/
void wxLEDPanel::SetLEDColour(wxLEDColour colourID)
{
    // for drawing
    wxBrush brush;
    wxPen pen;

    // colourID speichern
    m_activ_colour_id=colourID;

    int w=m_ledsize.GetWidth()+m_padding;
    int h=m_ledsize.GetHeight()+m_padding;

    // create Bitmaps for "LED on" und "LED off"
    wxBitmap led_on(w,h);
    wxBitmap led_off(w,h);
    wxBitmap led_none(w,h);

    // draw "LED on"
    m_mdc_led_on.SelectObject(led_on);

    // Clear Background
    m_mdc_led_on.SetBackground(this->GetBackgroundColour());
    m_mdc_led_on.Clear();

    // complete point
    pen.SetColour(s_colour_dark[colourID-1]);
    brush.SetColour(s_colour[colourID-1]);
    m_mdc_led_on.SetPen(pen);
    m_mdc_led_on.SetBrush(brush);
    m_mdc_led_on.DrawEllipse(wxPoint(0,0),m_ledsize);

    // left top corner in lighter colour
    pen.SetColour(s_colour_light[colourID-1]);
    m_mdc_led_on.SetPen(pen);
    m_mdc_led_on.DrawEllipticArc(0,0,m_ledsize.GetWidth(),m_ledsize.GetHeight(),75.0,195.0);


    // draw "LED off"
    m_mdc_led_off.SelectObject(led_off);

    // cleare Background
    m_mdc_led_off.SetBackground(this->GetBackgroundColour());
    m_mdc_led_off.Clear();

    // complete point
    pen.SetColour(s_colour_dark[colourID-1]);
    brush.SetColour(s_colour_verydark[colourID-1]);
    m_mdc_led_off.SetPen(pen);
    m_mdc_led_off.SetBrush(brush);
    m_mdc_led_off.DrawEllipse(wxPoint(0,0),m_ledsize);


    // draw "no LED"
    m_mdc_led_none.SelectObject(led_none);
    m_mdc_led_none.SetBackground(this->GetBackgroundColour());
    m_mdc_led_none.Clear();


    PrepareBackground();
}

/** @return the real colour of a LED */
const wxColour& wxLEDPanel::GetLEDColour() const
{
    return s_colour[m_activ_colour_id];
}

/** Overwritten to prepare the background with the new backgroundcolour
* @param colour the new backroundcolour
*/
bool wxLEDPanel::SetBackgroundColour(const wxColour& colour)
{
    if (wxWindow::SetBackgroundColour(colour))
    {
      PrepareBackground();
      return true;
    }

    return false;
}

/** Sets the speed for the scrolling
* @param speed the speed in ms (optimal range between 80-120)
*/
void wxLEDPanel::SetScrollSpeed(int speed)
{
    // the save way
    m_scrollTimer.Stop();

    // save speed
    m_scrollspeed=speed;

    // start timer
    if(m_scrollspeed>0 && m_scrolldirection!=wxALL)
        m_scrollTimer.Start(speed,true);
}

/** @return the speed of the scrolling */
int wxLEDPanel::GetScrollSpeed() const
{
    return m_scrollspeed;
}

/** Sets the direction to scroll
* @param d the direction (wxALL for no scrolling)
*/
void wxLEDPanel::SetScrollDirection(wxDirection d)
{
    // the save way
    m_scrollTimer.Stop();

    // save direction
    m_scrolldirection=d;

    if(m_scrollspeed>0 && m_scrolldirection!=wxALL)
        m_scrollTimer.Start(m_scrollspeed,true);
}

/** @return the current direction of the scrolling (wxALL for no scrolling)*/
wxDirection wxLEDPanel::GetScrollDirection() const
{
    return m_scrolldirection;
}

/** Swaps the LED states
* @param invert if true, all active LEDs are drawn as inactiv and all inactiv drawn as activ
*/
void wxLEDPanel::ShowInvertet(bool invert)
{
    if(m_invert==invert) return;

    m_invert=invert;
    PrepareBackground();
}

/** Should the inactive LEDs be drawn */
void wxLEDPanel::ShowInactivLEDs(bool show_inactivs)
{
    if(m_show_inactivs==show_inactivs) return;

    m_show_inactivs=show_inactivs;
    PrepareBackground();
}

void wxLEDPanel::SetContentAlign(int a)
{
    // save value
    m_align=a;

    // Reset the Horizontal position
    ResetPos();

    // Reinit the field
    m_field.Clear();
    m_field.SetDatesAt(m_pos,m_content_mo);
}

int wxLEDPanel::GetContentAlign() const
{
    return m_align;
}

void wxLEDPanel::SetText(const wxString& text, int align)
{
    // String emtpy
    if(text.IsEmpty()) return;

    // the MO for the Text
    MatrixObject* tmp=NULL;

    // save the align
    if(align!=-1) m_align=align;

    // save the string
    m_text=text;
    m_aniFrameNr=-1;

    // get the MO for the text
    if(m_align&wxALIGN_CENTER_HORIZONTAL)
        tmp=m_font.GetMOForText(text,wxALIGN_CENTER_HORIZONTAL);
    else if(m_align&wxALIGN_RIGHT)
        tmp=m_font.GetMOForText(text,wxALIGN_RIGHT);
    else tmp=m_font.GetMOForText(text);    // wxALIGN_LEFT

    // save the MO, and delete the tmp
    m_content_mo.Init(*tmp);
    delete tmp;

    // Find the place for the text
    ResetPos();

    // Set in field
    m_field.Clear();
    m_field.SetDatesAt(m_pos,m_content_mo);
}

/** @return the current text */
wxString wxLEDPanel::GetText() const
{
    return m_text;
}

void wxLEDPanel::SetImage(const wxImage img)
{
    if(!img.IsOk()) return;
    m_text.Empty();

    m_content_mo.Init(img);
    m_aniFrameNr=-1;

    // Find the place for the bitmap
    ResetPos();

    // Set in field
    m_field.Clear();
    m_field.SetDatesAt(m_pos,m_content_mo);
}

wxImage wxLEDPanel::GetContentAsImage() const
{
    return m_content_mo.GetAsImage();
}

void wxLEDPanel::SetAnimation(const wxAnimation ani)
{
    if(!ani.IsOk() || ani.GetFrameCount()==0) return;

    m_ani = ani;
    m_text.Empty();
    m_aniFrameNr = 0;

    m_content_mo.Init(ani.GetFrame(0));

    // Find the place for the bitmap
    ResetPos();

    // Set in field
    m_field.Clear();
    m_field.SetDatesAt(m_pos,m_content_mo);

    // start timer
    m_scrollTimer.Stop();
    m_scrollspeed = m_ani.GetDelay(0);
    m_scrollTimer.Start(m_scrollspeed,true);
}

const wxAnimation wxLEDPanel::GetAnimation() const
{
    return m_ani;
}

void wxLEDPanel::SetContentPaddingLeft(int padLeft)
{
    // Save value
    m_padLeft=padLeft;

    // Reset the text position
    ResetPos();

    // Reinit the field
    m_field.Clear();
    m_field.SetDatesAt(m_pos,m_content_mo);
}

int wxLEDPanel::GetContentPaddingLeft() const
{
    return m_padLeft;
}

void wxLEDPanel::SetContentPaddingRight(int padRight)
{
    // Save the Value
    m_padRight=padRight;

    // Reset the text position
    ResetPos();

    // Reinit the field
    m_field.Clear();
    m_field.SetDatesAt(m_pos,m_content_mo);
}

int wxLEDPanel::GetContentPaddingRight() const
{
    return m_padRight;
}

/** Sets the space between two letters
* @param leterSpace the space in points (one point = one LED)
*/
void wxLEDPanel::SetLetterSpace(int letterSpace)
{
    // is already this size?
    if(m_font.GetLetterSpace()==letterSpace) return;

    m_font.SetLetterSpace(letterSpace);
    Reset();
}

/** @return the space between two letters in points */
int wxLEDPanel::GetLetterSpace() const
{
    return m_font.GetLetterSpace();
}

void wxLEDPanel::SetFontType(wxLEDFontType t)
{
    if(m_font.GetFontType()==t) return;

    m_font.SetFontType(t);
    Reset();
}

wxLEDFontType wxLEDPanel::GetFontType() const
{
    return m_font.GetFontType();
}

/** this draws the data on the Control */
void wxLEDPanel::DrawField(wxDC& dc, bool backgroundMode)
{
    wxPoint point;
    int w=m_ledsize.GetWidth()+m_padding;
    int h=m_ledsize.GetHeight()+m_padding;

    // Zähler für Zeile und Spalte
    int x=0,y=0;

    // Pointer to avoid unnesecerie if blocks in the for block
    wxMemoryDC* p_mdc_data=((m_invert)?((m_show_inactivs)?(&m_mdc_led_off):(&m_mdc_led_none)):(&m_mdc_led_on));
    wxMemoryDC* p_mdc_nodata=((m_invert)?(&m_mdc_led_on):((m_show_inactivs)?(&m_mdc_led_off):(&m_mdc_led_none)));

    int l = m_field.GetLength();
    int fw = m_field.GetWidth();
    const char* field = m_field.GetData();
    for(int i=0;i<l;++i)
    {
        // Koordinaten
        point.x=x*w+m_padding;
        point.y=y*h+m_padding;

        // zeichnen
        if(field[i] && !backgroundMode)
        {
            dc.Blit(point.x,point.y,w,h,p_mdc_data,0,0);
        }
        else if(backgroundMode)
        {
            dc.Blit(point.x,point.y,w,h,p_mdc_nodata,0,0);
        }

        // hochzählen
        ++x;
        if(x==fw) {++y; x=0;}
    }
}

/** Do nothing to avoid flicker */
void wxLEDPanel::OnEraseBackground(wxEraseEvent& event)
{
    (void)event;
}

void wxLEDPanel::OnPaint(wxPaintEvent &event)
{
    (void)event;
    wxBufferedPaintDC dc(this);
    //dc.SetBackground(this->GetBackgroundColour());
    //dc.Clear();

    // background
    dc.Blit(0,0,m_mdc_background.GetSize().GetWidth(),m_mdc_background.GetSize().GetHeight(),&m_mdc_background,0,0);
    // field
    DrawField(dc);
}

void wxLEDPanel::ShiftLeft()
{
    // new text Pos
    m_pos.x--;

    // out of bound
    if(m_pos.x+m_content_mo.GetWidth()<=0)
    {
        m_pos.x=m_field.GetWidth();
        return;
    }

    // Shift
    m_field.ShiftLeft();

    // TODO check bounds!
    // data for the new line
    for(int i=0;i<m_content_mo.GetHeight();++i)
    {
        char d=m_content_mo.GetDataFrom(abs(m_pos.x-m_field.GetWidth()+1),i);
        if(d>0) m_field.SetDataAt(m_field.GetWidth()-1,m_pos.y+i,d);
    }
}

void wxLEDPanel::ShiftRight()
{
    // new text Pos
    m_pos.x++;
    // out of bound
    if(m_pos.x>=m_field.GetWidth())
    {
        m_pos.x=-m_content_mo.GetWidth();    // TODO without +1 error (in SetDatesAt??)
        return;
    }

    // Shift
    m_field.ShiftRight();

    // TODO check bounds!
    // TODO at first run -> false y-pos!
    // data for the new line
    for(int i=0;i<m_content_mo.GetHeight();++i)
    {
        char d=m_content_mo.GetDataFrom(abs(m_pos.x-m_field.GetWidth()+1),i);
        if(d>0) m_field.SetDataAt(0,m_pos.y+i,d);
    }
}

void wxLEDPanel::ShiftUp()
{
    // new text Pos
    m_pos.y--;
    // out of bound
    if(m_pos.y+m_content_mo.GetHeight()<=0)
        m_pos.y=m_field.GetHeight();

    // TODO optimize with shift
    m_field.Clear();
    m_field.SetDatesAt(m_pos,m_content_mo);
}

void wxLEDPanel::ShiftDown()
{
    // new text Pos
    m_pos.y++;
    // out of bound
    if(m_pos.y>=m_field.GetHeight())
        m_pos.y=-m_content_mo.GetHeight();

    // TODO optimize with shift
    m_field.Clear();
    m_field.SetDatesAt(m_pos,m_content_mo);

}

void wxLEDPanel::OnScrollTimer(wxTimerEvent& event)
{
    (void)event;
    if(m_scrollspeed==0||m_content_mo.IsEmpty()) return;

    // the save way
    m_scrollTimer.Stop();

    if(m_aniFrameNr < 0)
    {

        // Scroll
        switch(m_scrolldirection)
        {
            case wxALL: return;
            case wxLEFT: this->ShiftLeft(); break;
            case wxRIGHT: this->ShiftRight(); break;
            case wxDOWN: this->ShiftDown(); break;
            case wxUP: this->ShiftUp(); break;
            default: return;
        }
    }
    else
    {
        m_aniFrameNr++;
        if(m_aniFrameNr >= m_ani.GetFrameCount())
            m_aniFrameNr=0;

        m_content_mo.Init(m_ani.GetFrame(m_aniFrameNr));
        m_field.Clear();
        m_field.SetDatesAt(m_pos,m_content_mo);
        m_scrollspeed = m_ani.GetDelay(m_aniFrameNr);
    }

    // Repaint
    this->Refresh();

    // start timer again
    m_scrollTimer.Start(m_scrollspeed,true);
}

/** Resets the position of the content after scrolling */
void wxLEDPanel::ResetPos()
{
    // has a text?
    if(m_content_mo.GetData()==NULL) return;

    // horizontal text pos
    if(m_scrolldirection!=wxLEFT && m_scrolldirection!=wxRIGHT)
    {
        if(m_align & wxALIGN_RIGHT)
            m_pos.x=m_field.GetWidth()-m_content_mo.GetWidth()-m_padRight;
        else if(m_align & wxALIGN_CENTER_HORIZONTAL)
            m_pos.x=(m_field.GetWidth()-m_content_mo.GetWidth())/2;
        else // wxALING_LEFT
            m_pos.x=m_padLeft;
    }
    else if(m_scrolldirection==wxLEFT)
        m_pos.x=m_field.GetWidth();
    else if(m_scrolldirection==wxRIGHT)
        m_pos.x=-m_content_mo.GetWidth();

    // vertical text pos
    if(m_scrolldirection!=wxUP && m_scrolldirection!=wxDOWN)
    {
        if(m_align & wxALIGN_BOTTOM)
            m_pos.y=m_field.GetHeight()-m_content_mo.GetHeight();
        else if(m_align & wxALIGN_CENTER_VERTICAL)
            m_pos.y=(m_field.GetHeight()-m_content_mo.GetHeight())/2;
        else // wxALIGN TOP
            m_pos.y=0;
    }
    else if(m_scrolldirection==wxUP)
        m_pos.y=m_field.GetHeight();
    else if(m_scrolldirection==wxDOWN)
        m_pos.y=-m_content_mo.GetHeight();
}

/** Prepares the backgroundimage, to optimze speed */
void wxLEDPanel::PrepareBackground()
{
    wxSize s=DoGetBestSize();
    wxBitmap bmpBG(s.GetWidth(),s.GetHeight());

    m_mdc_background.SelectObject(bmpBG);

    // clear the background
    m_mdc_background.SetBackground(this->GetBackgroundColour());
    m_mdc_background.Clear();

    if(m_invert || m_show_inactivs)
        DrawField(m_mdc_background, true);
}

// Red, Green, Blue, Yellow, Magenta, Cyan, Grey
const wxColour wxLEDPanel::s_colour[7]=
    {    wxColour(255,0,0), wxColour(0,255,0), wxColour(0,0,255),
        wxColour(255,255,0), wxColour(255,0,255), wxColour(0,255,255),
        wxColour(128,128,128) };

const wxColour wxLEDPanel::s_colour_dark[7]=
    {    wxColour(128,0,0), wxColour(0,128,0), wxColour(0,0,128),
        wxColour(128,128,0), wxColour(128,0,128), wxColour(0,128,128),
        wxColour(64,64,64) };

const wxColour wxLEDPanel::s_colour_verydark[7]=
    {    wxColour(64,0,0), wxColour(0,64,0), wxColour(0,0,64),
        wxColour(64,64,0), wxColour(64,0,64), wxColour(0,64,64),
        wxColour(32,32,32) };

const wxColour wxLEDPanel::s_colour_light[7]=
    {    wxColour(255,128,128), wxColour(128,255,128), wxColour(128,128,255),
        wxColour(255,255,128), wxColour(255,128,255), wxColour(128,255,255),
        wxColour(192,192,192) };
