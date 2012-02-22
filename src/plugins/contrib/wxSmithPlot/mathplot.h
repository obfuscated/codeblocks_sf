/////////////////////////////////////////////////////////////////////////////
// Name:            mathplot.cpp
// Purpose:         Framework for plotting in wxWindows
// Original Author: David Schalig
// Maintainer:      Davide Rondini
// Contributors:    Jose Luis Blanco, Val Greene
// Created:         21/07/2003
// Last edit:       09/09/2007
// Copyright:       (c) David Schalig, Davide Rondini
// Licence:         wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _MP_MATHPLOT_H_
#define _MP_MATHPLOT_H_

/** @file mathplot.h */
/** @mainpage wxMathPlot
    wxMathPlot is a framework for mathematical graph plotting in wxWindows.

    The framework is designed for convenience and ease of use.

    @section screenshots Screenshots
    <a href="screenshots.html">Go to the screenshots page.</a>

    @section overview Overview
    The heart of wxMathPlot is mpWindow, which is a 2D canvas for plot layers.
    mpWindow can be embedded as subwindow in a wxPane, a wxFrame, or any other wxWindow.
    mpWindow provides a zoomable and moveable view of the layers. The current view can
    be controlled with the mouse, the scrollbars, and a context menu.

    Plot layers are implementations of the abstract base class mpLayer. Those can
    be function plots, scale rulers, or any other vector data visualisation. wxMathPlot provides two mpLayer implementations for plotting horizontal and vertical rulers: mpScaleX and mpScaleY.
    For convenient function plotting three more abstract base classes derived from mpLayer are provided: mpFX, mpFY and mpFXY. These base classes already come with plot code, own functions can be implemented by overiding just one member for retrieving a function value.
    Another class, mpFXYVector, is provided since version 0.03. This non-virtual class
    renders 2D graphs from a pair of std::vector's. One of the utility of this class is that you can use it directly in GUI designers such as wxSmith within Code::Blocks.

    From version 0.03, the mpWindow has built-in support for mouse-based pan and zoom through intuitive combinations of buttons and the mouse wheel. It also incorporate an optional double buffering mechanism to avoid flicker.

    @section coding Coding conventions
    wxMathPlot sticks to wxWindow's coding conventions. All entities defined by wxMathPlot
    have the prefix <i>mp</i>.

    @section author Author and license
    wxMathPlot is published under the terms of the wxWindow license.
    The original author is David Schalig <mrhill@users.sourceforge.net>.
    From June 2007 the project is maintained by Davide Rondini. <cdron@users.sourceforge.net>.
    Authors can be contacted via the wxMathPlot's homepage at
    http://sourceforge.net/projects/wxmathplot.
    Contributors:
    Jose Luis Blanco <jlblanco@ctima.uma.es>, Val Greene.
*/

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "mathplot.h"
#endif

#include <vector>

#include "wx/defs.h"
#include "wx/menu.h"
#include "wx/scrolwin.h"
#include "wx/event.h"
#include "wx/dynarray.h"
#include "wx/pen.h"
#include "wx/dcmemory.h"


//-----------------------------------------------------------------------------
// we do not always want to use a DLL

#ifndef     BUILD_MATHPLOT_DLL
#ifdef      WXDLLEXPORT
#undef      WXDLLEXPORT
#endif

#define     WXDLLEXPORT
#endif




//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class WXDLLEXPORT mpLayer;
class WXDLLEXPORT mpFX;
class WXDLLEXPORT mpFY;
class WXDLLEXPORT mpFXY;
class WXDLLEXPORT mpFXYVector;
class WXDLLEXPORT mpScaleX;
class WXDLLEXPORT mpScaleY;
class WXDLLEXPORT mpWindow;
class WXDLLEXPORT mpText;

/** Command IDs used by mpWindow */
enum
{
    mpID_FIT = 2000,    //!< Fit view to match bounding box of all layers
    mpID_ZOOM_IN,       //!< Zoom into view at clickposition / window center
    mpID_ZOOM_OUT,      //!< Zoom out
    mpID_CENTER,        //!< Center view on click position
    mpID_LOCKASPECT,    //!< Lock x/y scaling aspect
    mpID_HELP_MOUSE,    //!< Shows information about the mouse commands
};

//-----------------------------------------------------------------------------
// mpLayer
//-----------------------------------------------------------------------------

/** Plot layer, abstract base class.
    Any number of mpLayer implementations can be attached to mpWindow.
    Examples for mpLayer implementations are function graphs, or scale rulers.

    For convenience mpLayer defines a name, a font (wxFont), a pen (wxPen),
    and a continuity property (bool) as class members.
    The default values at constructor are the default font, a black pen, and
     continuity set to false (draw separate points).
    These may or may not be used by implementations.
*/
class WXDLLEXPORT mpLayer : public wxObject
{
public:
    mpLayer();

    virtual ~mpLayer() {};

    /** Check whether this layer has a bounding box.
        The default implementation returns \a TRUE. Overide and return
        FALSE if your mpLayer implementation should be ignored by the calculation
        of the global bounding box for all layers in a mpWindow.
        @retval TRUE Has bounding box
        @retval FALSE Has not bounding box
    */
    virtual bool   HasBBox() { return TRUE; }

    /** Get inclusive left border of bounding box.
        @return Value
    */
    virtual double GetMinX() { return -1.0; }

    /** Get inclusive right border of bounding box.
        @return Value
    */
    virtual double GetMaxX() { return  1.0; }

    /** Get inclusive bottom border of bounding box.
        @return Value
    */
    virtual double GetMinY() { return -1.0; }

    /** Get inclusive top border of bounding box.
        @return Value
    */
    virtual double GetMaxY() { return  1.0; }

    /** Plot given view of layer to the given device context.
        An implementation of this function has to transform layer coordinates to
        wxDC coordinates based on the view parameters retrievable from the mpWindow
        passed in \a w. The passed device context \a dc has its coordinate origin set
        to the center of the visible area. The coordinate orientation is as show in the
        following picture:
        <pre>
        +--------------------------------------------------+
        |                                                  |
        |                                                  |
        |                (wxDC origin 0,0)                 |
        |                       x-------------> acending X |
        |                       |                          |
        |                       |                          |
        |                       V ascending Y              |
        +--------------------------------------------------+
        </pre>
        Note that Y ascends in downward direction, whereas the usual vertical orientation
        for mathematical plots is vice versa. Thus Y-orientation will be swapped usually,
        when transforming between wxDC and mpLayer coordinates.

        <b> Rules for transformation between mpLayer and wxDC coordinates </b>
        @code
        dc_X = (layer_X - mpWindow::GetPosX()) * mpWindow::GetScaleX()
        dc_Y = (mpWindow::GetPosY() - layer_Y) * mpWindow::GetScaleY() // swapping Y-orientation

        layer_X = (dc_X / mpWindow::GetScaleX()) + mpWindow::GetPosX() // scale guaranted to be not 0
        layer_Y = mpWindow::GetPosY() - (dc_Y / mpWindow::GetScaleY()) // swapping Y-orientation
        @endcode

        @param dc Device context to plot to.
        @param w  View to plot. The visible area can be retrieved from this object.
    */
    virtual void   Plot(wxDC & dc, mpWindow & w) = 0;

    /** Get layer name.
        @return Name
    */
    wxString       GetName() const { return m_name; }

    /** Get font set for this layer.
        @return Font
    */
    const wxFont&  GetFont() const { return m_font; }

    /** Get pen set for this layer.
        @return Pen
    */
    const wxPen&   GetPen()  const { return m_pen;  }

    /** Set the 'continuity' property of the layer (true:draws a continuous line, false:draws separate points).
      * \sa GetContinuity
      */
    void SetContinuity(bool continuity) {m_continuous = continuity;}

    /** Gets the 'continuity' property of the layer.
      * \sa SetContinuity
      */
    bool GetContinuity() const {return m_continuous;}

    /** Set layer name
        @param name Name, will be copied to internal class member
    */
    void SetName(wxString name) { m_name = name; }

    /** Set layer font
        @param font Font, will be copied to internal class member
    */
    void SetFont(wxFont& font)  { m_font = font; }

    /** Set layer pen
        @param pen Pen, will be copied to internal class member
    */
    void SetPen(wxPen& pen)     { m_pen  = pen;  }

protected:
    wxFont   m_font;    //!< Layer's font
    wxPen    m_pen;     //!< Layer's pen
    wxString m_name;    //!< Layer's name
    bool     m_continuous; //!< Specify if the layer will be plotted as a continuous line or a set of points.

    DECLARE_CLASS(mpLayer)
};

//-----------------------------------------------------------------------------
// mpLayer implementations - functions
//-----------------------------------------------------------------------------

/** @name Label alignment constants
@{*/

/** @internal */
#define mpALIGNMASK    0x03
/** Aligns label to the right. For use with mpFX. */
#define mpALIGN_RIGHT  0x00
/** Aligns label to the center. For use with mpFX and mpFY. */
#define mpALIGN_CENTER 0x01
/** Aligns label to the left. For use with mpFX. */
#define mpALIGN_LEFT   0x02
/** Aligns label to the top. For use with mpFY. */
#define mpALIGN_TOP    mpALIGN_RIGHT
/** Aligns label to the bottom. For use with mpFY. */
#define mpALIGN_BOTTOM mpALIGN_LEFT
/** Aligns X axis to bottom border. For mpScaleX */
#define mpALIGN_BORDER_BOTTOM  0x04
/** Aligns X axis to top border. For mpScaleX */
#define mpALIGN_BORDER_TOP  0x05
/** Aligns Y axis to left border. For mpScaleY */
#define mpALIGN_BORDER_LEFT mpALIGN_BORDER_BOTTOM
/** Aligns Y axis to right border. For mpScaleY */
#define mpALIGN_BORDER_RIGHT mpALIGN_BORDER_TOP
/** Aligns label to north-east. For use with mpFXY. */
#define mpALIGN_NE     0x00
/** Aligns label to north-west. For use with mpFXY. */
#define mpALIGN_NW     0x01
/** Aligns label to south-west. For use with mpFXY. */
#define mpALIGN_SW     0x02
/** Aligns label to south-east. For use with mpFXY. */
#define mpALIGN_SE     0x03

/*@}*/

/** @name mpLayer implementations - functions
@{*/

/** Abstract base class providing plot and labeling functionality for functions F:X->Y.
    Override mpFX::GetY to implement a function.
    Optionally implement a constructor and pass a name (label) and a label alignment
    to the constructor mpFX::mpFX. If the layer name is empty, no label will be plotted.
*/
class WXDLLEXPORT mpFX : public mpLayer
{
public:
    /** @param name  Label
        @param flags Label alignment, pass one of #mpALIGN_RIGHT, #mpALIGN_CENTER, #mpALIGN_LEFT.
    */
    mpFX(wxString name = wxEmptyString, int flags = mpALIGN_RIGHT);

    /** Get function value for argument.
        Override this function in your implementation.
        @param x Argument
        @return Function value
    */
    virtual double GetY( double x ) = 0;

    /** Layer plot handler.
        This implementation will plot the function in the visible area and
        put a label according to the aligment specified.
    */
    virtual void Plot(wxDC & dc, mpWindow & w);

protected:
    int m_flags; //!< Holds label alignment

    DECLARE_CLASS(mpFX)
};

/** Abstract base class providing plot and labeling functionality for functions F:Y->X.
    Override mpFY::GetX to implement a function.
    Optionally implement a constructor and pass a name (label) and a label alignment
    to the constructor mpFY::mpFY. If the layer name is empty, no label will be plotted.
*/
class WXDLLEXPORT mpFY : public mpLayer
{
public:
    /** @param name  Label
        @param flags Label alignment, pass one of #mpALIGN_BOTTOM, #mpALIGN_CENTER, #mpALIGN_TOP.
    */
    mpFY(wxString name = wxEmptyString, int flags = mpALIGN_TOP);

    /** Get function value for argument.
        Override this function in your implementation.
        @param y Argument
        @return Function value
    */
    virtual double GetX( double y ) = 0;

    /** Layer plot handler.
        This implementation will plot the function in the visible area and
        put a label according to the aligment specified.
    */
    virtual void Plot(wxDC & dc, mpWindow & w);

protected:
    int m_flags; //!< Holds label alignment

    DECLARE_CLASS(mpFY)
};

/** Abstract base class providing plot and labeling functionality for a locus plot F:N->X,Y.
    Locus argument N is assumed to be in range 0 .. MAX_N, and implicitely derived by enumrating
    all locus values. Override mpFXY::Rewind and mpFXY::GetNextXY to implement a locus.
    Optionally implement a constructor and pass a name (label) and a label alignment
    to the constructor mpFXY::mpFXY. If the layer name is empty, no label will be plotted.
*/
class WXDLLEXPORT mpFXY : public mpLayer
{
public:
    /** @param name  Label
        @param flags Label alignment, pass one of #mpALIGN_NE, #mpALIGN_NW, #mpALIGN_SW, #mpALIGN_SE.
    */
    mpFXY(wxString name = wxEmptyString, int flags = mpALIGN_NE);

    /** Rewind value enumeration with mpFXY::GetNextXY.
        Override this function in your implementation.
    */
    virtual void Rewind() = 0;

    /** Get locus value for next N.
        Override this function in your implementation.
        @param x Returns X value
        @param y Returns Y value
    */
    virtual bool GetNextXY(double & x, double & y) = 0;

    /** Layer plot handler.
        This implementation will plot the locus in the visible area and
        put a label according to the aligment specified.
    */
    virtual void Plot(wxDC & dc, mpWindow & w);

protected:
    int m_flags; //!< Holds label alignment

    DECLARE_CLASS(mpFXY)
};

/** Abstract base class providing plot and labeling functionality for functions F:Y->X.
    Override mpProfile::GetX to implement a function.
    This class if similar to mpFY, but the Plot method is different. The plot is in fact represented by lines instead of points, which gives best rendering of rapidly-varying functions, and in general, data which are not so close one to another.
    Optionally implement a constructor and pass a name (label) and a label alignment
    to the constructor mpProfile::mpProfile. If the layer name is empty, no label will be plotted.
*/
class WXDLLEXPORT mpProfile : public mpLayer
{
public:
    /** @param name  Label
        @param flags Label alignment, pass one of #mpALIGN_BOTTOM, #mpALIGN_CENTER, #mpALIGN_TOP.
    */
    mpProfile(wxString name = wxEmptyString, int flags = mpALIGN_TOP);

    /** Get function value for argument.
        Override this function in your implementation.
        @param x Argument
        @return Function value
    */
    virtual double GetY( double x ) = 0;

    /** Layer plot handler.
        This implementation will plot the function in the visible area and
        put a label according to the aligment specified.
    */
    virtual void Plot(wxDC & dc, mpWindow & w);

protected:
    int m_flags; //!< Holds label alignment

    DECLARE_CLASS(mpProfile)
};

/*@}*/

//-----------------------------------------------------------------------------
// mpLayer implementations - furniture (scales, ...)
//-----------------------------------------------------------------------------

/** @name mpLayer implementations - furniture (scales, ...)
@{*/

/** Plot layer implementing a x-scale ruler.
    The ruler is fixed at Y=0 in the coordinate system. A label is plotted at
    the bottom-right hand of the ruler. The scale numbering automatically
    adjusts to view and zoom factor.
*/
class WXDLLEXPORT mpScaleX : public mpLayer
{
public:
    /** @param name Label to plot by the ruler
        @param flags Set position of the scale respect to the window.
        @param ticks Select ticks or grid. Give TRUE for drawing axis ticks, FALSe for drawing the grid */
    mpScaleX(wxString name = wxT("X"), int flags = mpALIGN_CENTER, bool ticks = true);

    /** Layer plot handler.
        This implementation will plot the ruler adjusted to the visible area.
    */
    virtual void Plot(wxDC & dc, mpWindow & w);

    /** Check whether this layer has a bounding box.
        This implementation returns \a FALSE thus making the ruler invisible
        to the plot layer bounding box calculation by mpWindow.
    */
    virtual bool HasBBox() { return FALSE; }

    /** Set X axis alignment.
        @param align alignment (choose between mpALIGN_BORDER_BOTTOM, mpALIGN_BOTTOM, mpALIGN_CENTER, mpALIGN_TOP, mpALIGN_BORDER_TOP */
    void SetAlign(int align) { m_flags = align; };

    /** Set X axis ticks or grid
        @param ticks TRUE to plot axis ticks, FALSE to plot grid. */
    void SetTicks(bool ticks) { m_ticks = ticks; };

    /** Get X axis ticks or grid
        @return TRUE if plot is drawing axis ticks, FALSE if the grid is active. */
    bool GetTicks() { return m_ticks; };

protected:
    int m_flags; //!< Flag for axis alignment
    bool m_ticks; //!< Flag to toggle between ticks or grid

    DECLARE_CLASS(mpScaleX)
};

/** Plot layer implementing a y-scale ruler.
    If align is set to mpALIGN_CENTER, the ruler is fixed at X=0 in the coordinate system. If the align is set to mpALIGN_TOP or mpALIGN_BOTTOM, the axis is always drawn respectively at top or bottom of the window. A label is plotted at
    the top-right hand of the ruler. The scale numbering automatically
    adjusts to view and zoom factor.
*/
class WXDLLEXPORT mpScaleY : public mpLayer
{
public:
    /** @param name Label to plot by the ruler
        @param flags Set position of the scale respect to the window.
        @param ticks Select ticks or grid. Give TRUE for drawing axis ticks, FALSe for drawing the grid */
    mpScaleY(wxString name = wxT("Y"), int flags = mpALIGN_CENTER, bool ticks = true);

    /** Layer plot handler.
        This implementation will plot the ruler adjusted to the visible area.
    */
    virtual void Plot(wxDC & dc, mpWindow & w);

    /** Check whether this layer has a bounding box.
        This implementation returns \a FALSE thus making the ruler invisible
        to the plot layer bounding box calculation by mpWindow.
    */
    virtual bool HasBBox() { return FALSE; }

    /** Set Y axis alignment.
        @param align alignment (choose between mpALIGN_BORDER_LEFT, mpALIGN_LEFT, mpALIGN_CENTER, mpALIGN_RIGHT, mpALIGN_BORDER_RIGHT) */
    void SetAlign(int align) { m_flags = align; };

    /** Set Y axis ticks or grid
        @param ticks TRUE to plot axis ticks, FALSE to plot grid. */
    void SetTicks(bool ticks) { m_ticks = ticks; };

    /** Get Y axis ticks or grid
        @return TRUE if plot is drawing axis ticks, FALSE if the grid is active. */
    bool GetTicks() { return m_ticks; };

protected:
    int m_flags; //!< Flag for axis alignment
    bool m_ticks; //!< Flag to toggle between ticks or grid

    DECLARE_CLASS(mpScaleY)
};

//-----------------------------------------------------------------------------
// mpWindow
//-----------------------------------------------------------------------------

/** @name Constants defining mouse modes for mpWindow
@{*/

/** Mouse panning drags the view. Mouse mode for mpWindow. */
#define mpMOUSEMODE_DRAG    0
/** Mouse panning creates a zoom box. Mouse mode for mpWindow. */
#define mpMOUSEMODE_ZOOMBOX 1

/*@}*/
/** Define the hash map for managing the layer list inside mpWindow */
WX_DECLARE_HASH_MAP( int, mpLayer*, wxIntegerHash, wxIntegerEqual, wxLayerList );

/** Canvas for plotting mpLayer implementations.

    This class defines a zoomable and moveable 2D plot canvas. Any number
    of mpLayer implementations (scale rulers, function plots, ...) can be
    attached using mpWindow::AddLayer.

    The canvas window provides a context menu with actions for navigating the view.
    The context menu can be retrieved with mpWindow::GetPopupMenu, e.g. for extending it
    externally.

    Since wxMathPlot version 0.03, the mpWindow incorporate the following features:
        - DoubleBuffering (Default=disabled): Can be set with EnableDoubleBuffer
        - Mouse based pan/zoom (Default=enable): Can be set with EnableMousePanZoom.

    The mouse commands can be visualized by the user through the popup menu, and are:
        - Mouse Move+CTRL: Pan (Move)
        - Mouse Wheel: Vertical scroll
        - Mouse Wheel+SHIFT: Horizontal scroll
        - Mouse Wheel UP+CTRL: Zoom in
        - Mouse Wheel DOWN+CTRL: Zoom out

*/
class WXDLLEXPORT mpWindow : public wxScrolledWindow
{
public:
    mpWindow() {}
    mpWindow( wxWindow *parent, wxWindowID id,
                     const wxPoint &pos = wxDefaultPosition,
                     const wxSize &size = wxDefaultSize,
                     int flags = 0);
    ~mpWindow();

    /** Get reference to context menu of the plot canvas.
        @return Pointer to menu. The menu can be modified.
    */
    wxMenu* GetPopupMenu() { return &m_popmenu; }

    /** Add a plot layer to the canvas.
        @param layer Pointer to layer. The mpLayer object will get under control of mpWindow,
                     i.e. it will be delete'd on mpWindow destruction
        @retval TRUE Success
        @retval FALSE Failure due to out of memory.
    */
    bool AddLayer( mpLayer* layer);

    /** Remove a plot layer from the canvas.
        @param layer Pointer to layer. The mpLayer object will be destructed using delete.
        @return true if layer is deleted correctly

        N.B. Only the layer reference in the mpWindow is deleted, the layer object still exists!
    */
    bool DelLayer( mpLayer* layer);

    /*! Get the layer in list position indicated.
        N.B. You <i>must</i> know the index of the layer inside the list!
        @param position position of the layer in the layers list
        @return pointer to mpLayer
    */
    mpLayer* GetLayer(int position);


    /** Get current view's X scale.
        See @ref mpLayer::Plot "rules for coordinate transformation"
        @return Scale
    */
    double GetXscl() { return m_scaleX; }
    double GetScaleX(void) const{ return m_scaleX; }; // Schaling's method: maybe another method esists with the same name

    /** Get current view's Y scale.
        See @ref mpLayer::Plot "rules for coordinate transformation"
        @return Scale
    */
    double GetYscl() const { return m_scaleY; }
    double GetScaleY(void) const { return m_scaleY; } // Schaling's method: maybe another method esists with the same name

    /** Get current view's X position.
        See @ref mpLayer::Plot "rules for coordinate transformation"
        @return X Position in layer coordinate system, that corresponds to the center point of the view.
    */
    double GetXpos() const { return m_posX; }
    double GetPosX(void) const { return m_posX; }

    /** Get current view's Y position.
        See @ref mpLayer::Plot "rules for coordinate transformation"
        @return Y Position in layer coordinate system, that corresponds to the center point of the view.
    */
    double GetYpos() const { return m_posY; }
    double GetPosY(void) const { return m_posY; }

    /** Get current view's X dimension in device context units.
        Usually this is equal to wxDC::GetSize, but it might differ thus mpLayer
        implementations should rely on the value returned by the function.
        See @ref mpLayer::Plot "rules for coordinate transformation"
        @return X dimension.
    */
    int GetScrX(void) const { return m_scrX; }

    /** Get current view's Y dimension in device context units.
        Usually this is equal to wxDC::GetSize, but it might differ thus mpLayer
        implementations should rely on the value returned by the function.
        See @ref mpLayer::Plot "rules for coordinate transformation"
        @return Y dimension.
    */
    int GetScrY(void) const { return m_scrY; }

    /** Set current view's X scale and refresh display.
        @param scaleX New scale, must not be 0.
    */
    void SetScaleX(double scaleX);

    /** Set current view's Y scale and refresh display.
        @param scaleY New scale, must not be 0.
    */
    void SetScaleY(double scaleY) { if (scaleY!=0) m_scaleY=scaleY; UpdateAll(); }

    /** Set current view's X position and refresh display.
        @param posX New position that corresponds to the center point of the view.
    */
    void SetPosX(double posX) { m_posX=posX; UpdateAll(); }

    /** Set current view's Y position and refresh display.
        @param posY New position that corresponds to the center point of the view.
    */
    void SetPosY(double posY) { m_posY=posY; UpdateAll(); }

    /** Set current view's X and Y position and refresh display.
        @param posX New position that corresponds to the center point of the view.
        @param posY New position that corresponds to the center point of the view.
    */
    void SetPos( double posX, double posY) { m_posX=posX; m_posY=posY; UpdateAll(); }

    /** Enable/disable the double-buffering of the window, eliminating the flicker (default=disabled).
     */
    void EnableDoubleBuffer( bool enabled ) { m_enableDoubleBuffer = enabled; }

    /** Enable/disable the feature of pan/zoom with the mouse (default=enabled)
     */
    void EnableMousePanZoom( bool enabled ) { m_enableMouseNavigation = enabled; }

    /** Enable or disable X/Y scale aspect locking for the view.
        @note Explicit calls to mpWindow::SetScaleX and mpWindow::SetScaleY will set
              an unlocked apect, but any other action changing the view scale will
              lock the aspect again.
    */
    void LockAspect(bool enable = TRUE);

    /** Checks whether the X/Y scale aspect is locked.
        @retval TRUE Locked
        @retval FALSE Unlocked
    */
    inline bool IsAspectLocked() { return m_lockaspect; }

    /** Set view to fit global bounding box of all plot layers and refresh display.
        Scale and position will be set to a show all attached mpLayers.
        The X/Y scale aspect lock is taken into account.
    */
    void Fit();

    /** Zoom into current view and refresh display */
    void ZoomIn();

    /** Zoom out current view and refresh display */
    void ZoomOut();

    /** Refresh display */
    void UpdateAll();

    // Added methods by Davide Rondini

    /** Counts the number of plot layers, axis excluded.
    	\return The number of profiles plotted.
    */
    unsigned int CountLayers();

protected:
    void OnPaint         (wxPaintEvent     &event); //!< Paint handler, will plot all attached layers
    void OnSize          (wxSizeEvent      &event); //!< Size handler, will update scroll bar sizes
    void OnScroll2       (wxScrollWinEvent &event); //!< Scroll handler, will move canvas
    void OnShowPopupMenu (wxMouseEvent     &event); //!< Mouse handler, will show context menu
    void OnMouseRightDown(wxMouseEvent     &event); //!< Mouse handler, for detecting when the user drag with the right button or just "clicks" for the menu
    void OnCenter        (wxCommandEvent   &event); //!< Context menu handler
    void OnFit           (wxCommandEvent   &event); //!< Context menu handler
    void OnZoomIn        (wxCommandEvent   &event); //!< Context menu handler
    void OnZoomOut       (wxCommandEvent   &event); //!< Context menu handler
    void OnLockAspect    (wxCommandEvent   &event); //!< Context menu handler
    void OnMouseHelp     (wxCommandEvent   &event); //!< Context menu handler
    void OnMouseWheel    (wxMouseEvent     &event); //!< Mouse handler for the wheel
    void OnMouseMove     (wxMouseEvent     &event); //!< Mouse handler for mouse motion (for pan)

    /** Recalculate global layer bounding box, and save it in m_minX,...
      * \return true if there is any valid BBox information.
      */
    bool UpdateBBox(); //!<

    //wxList m_layers;    //!< List of attached plot layers
    wxLayerList m_layers; //!< List of attached plot layers
    wxMenu m_popmenu;   //!< Canvas' context menu
    bool   m_lockaspect;//!< Scale aspect is locked or not

    double m_minX;      //!< Global layer bounding box, left border incl.
    double m_maxX;      //!< Global layer bounding box, right border incl.
    double m_minY;      //!< Global layer bounding box, bottom border incl.
    double m_maxY;      //!< Global layer bounding box, top border incl.
    double m_scaleX;    //!< Current view's X scale
    double m_scaleY;    //!< Current view's Y scale
    double m_posX;      //!< Current view's X position
    double m_posY;      //!< Current view's Y position
    int    m_scrX;      //!< Current view's X dimension
    int    m_scrY;      //!< Current view's Y dimension
    int    m_clickedX;  //!< Last mouse click X position, for centering and zooming the view
    int    m_clickedY;  //!< Last mouse click Y position, for centering and zooming the view

    int          m_last_lx,m_last_ly;   //!< For double buffering
    wxMemoryDC   m_buff_dc;             //!< For double buffering
    wxBitmap     *m_buff_bmp;            //!< For double buffering
    bool          m_enableDoubleBuffer;  //!< For double buffering
    bool          m_enableMouseNavigation;  //!< For pan/zoom with the mouse.
    bool          m_mouseMovedAfterRightClick;
    long          m_mouseRClick_X,m_mouseRClick_Y; //!< For the right button "drag" feature

    DECLARE_CLASS(mpWindow)
    DECLARE_EVENT_TABLE()
};

//-----------------------------------------------------------------------------
// mpFXYVector - provided by Jose Luis Blanco
//-----------------------------------------------------------------------------

/** A class providing graphs functionality for a 2D plot (either continuous or a set of points), from vectors of data.
     This class can be used directly, the user does not need to derive any new class. Simply pass the data as two vectors
     with the same length containing the X and Y coordinates to the method SetData.

     To generate a graph with a set of points, call
     \code
     layerVar->SetContinuity(false)
     \endcode

     or

     \code
     layerVar->SetContinuity(true)
     \endcode

     to render the sequence of coordinates as a continuous line.

     (Added: Jose Luis Blanco, AGO-2007)
*/
class WXDLLEXPORT mpFXYVector : public mpFXY
{
public:
    /** @param name  Label
        @param flags Label alignment, pass one of #mpALIGN_NE, #mpALIGN_NW, #mpALIGN_SW, #mpALIGN_SE.
    */
    mpFXYVector(wxString name = wxEmptyString, int flags = mpALIGN_NE);

    /** Changes the internal data: the set of points to draw.
        Both vectors MUST be of the same length. This method DOES NOT refresh the mpWindow, do it manually.
      * \sa Clear
    */
    void SetData( const std::vector<float> &xs,const std::vector<float> &ys);

    /** Clears all the data, leaving the layer empty.
      * \sa SetData
      */
    void Clear();

protected:
    /** The internal copy of the set of data to draw.
      */
    std::vector<float>  m_xs,m_ys;

    /** The internal counter for the "GetNextXY" interface
      */
    size_t              m_index;

    /** Loaded at SetData
      */
    double              m_minX,m_maxX,m_minY,m_maxY;

    /** Rewind value enumeration with mpFXY::GetNextXY.
        Overrided in this implementation.
    */
    void Rewind();

    /** Get locus value for next N.
        Overrided in this implementation.
        @param x Returns X value
        @param y Returns Y value
    */
    bool GetNextXY(double & x, double & y);

    /** Returns the actual minimum X data (loaded in SetData).
      */
    double GetMinX() { return m_minX; }

    /** Returns the actual minimum Y data (loaded in SetData).
      */
    double GetMinY() { return m_minY; }

    /** Returns the actual maximum X data (loaded in SetData).
      */
    double GetMaxX() { return m_maxX; }

    /** Returns the actual maximum  Y data (loaded in SetData).
      */
    double GetMaxY() { return m_maxY; }

    int     m_flags; //!< Holds label alignment

    DECLARE_CLASS(mpFXYVector)
};

//-----------------------------------------------------------------------------
// mpText - provided by Val Greene
//-----------------------------------------------------------------------------

/** Plot layer implementing a text string.
The text is plotted using a percentage system 0-100%, so the actual
coordinates for the location are not required, and the text stays
on the plot reguardless of the other layers location and scaling
factors.
*/
class WXDLLEXPORT mpText : public mpLayer
{
public:
    /** @param name text to be drawn in the plot
        @param offsetx holds offset for the X location in percentage (0-100)
        @param offsety holds offset for the Y location in percentage (0-100) */
    mpText(wxString name = wxT("Title"), int offsetx = 5, int offsety = 50);

    /** Text Layer plot handler.
        This implementation will plot text adjusted to the visible area. */
    virtual void Plot(wxDC & dc, mpWindow & w);

    /** mpText should not be used for scaling decisions. */
    virtual bool HasBBox() { return FALSE; }

protected:
    int m_offsetx; //!< Holds offset for X in percentage
    int m_offsety; //!< Holds offset for Y in percentage

    DECLARE_CLASS(mpText)
};


/*@}*/

#endif // _MP_MATHPLOT_H_
