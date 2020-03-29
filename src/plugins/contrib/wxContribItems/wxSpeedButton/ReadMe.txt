/*
* This file is part of a wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2007  Bartlomiej Swiecki
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* wxSmith and this file is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
* 
* Ron Collins
* rcoll@theriver.com
* 4-Feb-2010
* 
*/



=============
wxSpeedButton
=============

-------------------
General Description
-------------------

wxSpeedButton is a toggle button, that acts similar to TSpeedButton used in
the Delphi language.

wxSpeedButton displays both a text label and a bitmap, and is capable of
acting as a simple (generic) push-button, a toggle-button, or a grouped
toggle button.  (In a grouped toggle button, only one button may be "down" at
one time; when a button is pressed down, the previous "down" button goes "up").
When acting as a grouped toggle button, the buttons may be grouped by their
parent (for example, all wxSpeedButton's on a panel), or by an index number.
This grouping (by index number) allows wxToggleButton's to be placed anywhere
on a form (or dialog), and still used as a single group of buttons.  And, of
course, multiple groups may be defined simply by giving them different group
index numbers.

When defining a bitmap image (or "glyph") for a wxToggleButton, use an image
that may contain multiple sub-images.  (This is similar to the Delphi method
of putting the "active" image and the "disabled" image into the same file).
The button image may contain 1, 2, or 3 sub-images.  (It may, in fact,
contain more sub-images, but only the first 3 will be used).  

If the image contains only a single image, then that image will be used for
the button "up" and "down" states, and an internally-generated grey-scale
image will be used for the "disabled" button.  If the image contains two 
sub-images, the first sub-image will be used for the "up" and "down" states,
and the second sub-image will be used for the "disabled" button.  If the
image contains all three sub-images (or more) then the first sub-image is 
displayed on the button when in the "up" state; the second sub-image will be 
used when the button is in the "down" state, and the third sub-image will be 
used when the button is disabled.  

When defined in this way, the images displayed on the button are drawn in a
"transparent" manner.  If the image contains a transparency mask, then it will
be used; if not, then a transparency mask will be generated using the
bottom-left pixel of the image to determine the transparent color (this is the
same method used in Delphi).  If, however, a seperate image is specified for
a button state (using method calls SetGlyphUp() or SetGlyphDown()) then the
transparency mask must be explicitly defined within the image itself.

The wxSpeedButton build directory has the following structure:
SpeedButton\
    bin\                // this dir contains the binary file output of the 
                        // Code::Blocks projects; files include "SpeedTest.exe",
                        // "wxsSmithButton.dll",  "wxsSmithButton.zip", and 
                        // "CopySmithButton.bat"    
    images\             // a set of usefull XPM images for buttons         
    include\            // the include file "wxSpeedButton.h" is here
    lib\                // the library "wxSpeedButton.a" is here
    obj\                // intermediate files for all compiles
    project\            // this is the Code::Bocks project to create the
                        // wxSpeedButton library; it creates the file
                        // "wxSpeedButton.a" and places the library file and
                        // the include file in the proper directories 
    src\                // source files for component wxSpeedButton
    test\               // this is the Code::Blocks project to create a test
                        // program for wxSpeedButton; the output file
                        // "SpeedTest.exe" is placed in the ..\bin\ directory
        wxsmith\        // used by Code::Blocks and wxSmith
    wxs\                // this is the Code::Blocks project to create the
                        // plugin for Code::Blocks and wxSmith, to put the
                        // component wxSpeedButton on the wxSmith palette.
                        // the output files "wxsSmithButton.dll" and
                        // and "wxsSmithButton.zip" are placed in the ..\bin\
                        // directory; they must be manually copied to the
                        // plugin directory of your Code::Blocks installation.
                        // see the batch file "CopySmithButton.bat" to
                        // make this easier  
        images\         // images used on the wxSmith palette 
        src\            // source code for the property pages used by
                        // wxSmith for the wxSpeedButton component

The Code::Blocks projects used here were created and tested under Windows XP,
using Code::Blocks SVN version 5866, and wxWidgets version 2.8.  The correct 
order to build these projects is:

1) Build the library (project found in the "project\" directory)
2) Build the wxSmith plugin (project found in the "wxs\" directory)
3) Exit Code::Blocks and install the plugins
   copy "bin\wxsSmithButton.dll" to "<Code::Blocks>\share\CodeBlocks\plugins"
   copy "bin\wxsSmithButton.zip" to "<Code::Blocks>\share\CodeBlocks"
4) Re-start Code::Blocks and build the test program (project found in the
   "test\" directory)

(For Windows XP, all of these projects and programs are already compiled; you
only need to install the plugin files into your own Code::Blocks installation
directory).

---------------------   
Component Description
---------------------

The wxSpeedButton component is a completely new widget, written from scratch,
and descended from wxControl.  It uses wxNativeRenderer to draw the button
face, giving it the same look and feel of the built-in wxButton, regardless
of platform.

The members of the wxSpeedButton component are:

Constructors
------------

wxSpeedButton(  wxWindow       *inParent,                       // parent window
        wxWindowID      inID,                                   // id of this button
        const wxString &inLabel         = _(""),                // button text
        const wxBitmap &inGlyph         = wxNullBitmap,         // bitmaps displayed on button
        int             inGlyphCount    = 0,                    // number of images in inGlyph
        int             inMargin        = 2,                    // area around image and text
        int             inGroupIndex    = 0,                    // ident of a group of buttons
        bool            inAllowAllUp    = true,                 // allow all buttons up
        const wxPoint  &inPos           = wxDefaultPosition,    // button position
        const wxSize   &inSize          = wxDefaultSize,        // button size
        long            inStyle         = 0,                    // border styles
        const wxValidator &inVal        = wxDefaultValidator,   // validator
        const wxString &inName          = _("SpeedButton"));    // name of button


wxSpeedButton( wxWindow        *inParent,                       // parent window
        wxWindowID      inID,                                   // id of this button
        const wxString &inLabel         = _(""),                // button text
        const wxString &inGlyphFile     = _(""),                // bitmaps displayed on button
        int             inGlyphCount    = 0,                    // number of images in inGlyph
        int             inMargin        = 2,                    // area around image and tex
        int             inGroupIndex    = 0,                    // ident of a group of buttons
        bool            inAllowAllUp    = true,                 // allow all buttons up
        const wxPoint  &inPos           = wxDefaultPosition,    // button position
        const wxSize   &inSize          = wxDefaultSize,        // button size
        long            inStyle         = 0,                    // border styles
        const wxValidator &inVal        = wxDefaultValidator,   // validator
        const wxString &inName          = _("SpeedButton"));    // name of button


Member Functions
----------------

void        SetLabel(wxString &inLabel);
wxString    GetLabel(void);
  Use these functions to get or set the text-label drawn on the button face.
  
void        SetGlyphUp(wxBitmap &inBitmap);
wxBitmap   &GetGlyphUp(void);
void        SetGlyphDown(wxBitmap &inBitmap);
wxBitmap   &GetGlyphDown(void);
void        SetGlyphDisabled(wxBitmap &inBitmap);
wxBitmap   &GetGlyphDisabled(void);
  Use these functions to get or set the image drawn on the buton face,
  depending upon the state of the button (up, down, or disabled).  Unlike the
  images defined in the button constructor, we do not automatically add in
  a transparency mask to these images.  When setting an image using one of
  these functions, a transparancy mask must be explicitly defined within
  the image itself.

void        SetAlign(int inAlign);
int         GetAlign(void);
  Get or set the image and text alignment; "inAlign" must be one of
  wxBU_LEFT, wxBU_RIGHT, wxBU_TOP, or wxBU_BOTTOM.  The alignment is normally
  set as part of the Window "style" value, but these functions give you the
  option to get or set it dynamically.

void        SetMargin(int inMargin);
int         GetMargin(void);
  The margin is the distance (in pixels) between the button edge and the
  image, and between the image and the label text.
  
void        SetGroupIndex(bool inIndex);
int         GetGroupIndex(void);
  Change the group index of a button, in effect moving the button to a different
  group.  There are 3 special values used here:
  - A value of 0 makes the button a "simple" button; all you can do is click it,
    it always appears in the UP state;
  - A value of -1 makes the button a "toggle" button; it is not associated with
    any group of buttons, and each time you click it will change it's state
    from UP to DOWN to UP again.
  - A value of -2 places the button in a group defined by it's immediate parent.
    Buttons with a group index of -2 and residing on the same parent (for
    example, a wxPanel) act as a single group, where only one button can be
    set DOWN at a time.
  - Any other integer value (positive or negative) will place those buttons
    with the same group index value into a group.  Within a group, only one
    button may be DOWN at a time, although it is possible for them all to
    be UP at the same time (see "Allow All Up").
    
void        SetDown(bool inDown);
bool        GetDown(void);
void        SetValue(bool inDown);
bool        GetValue(void);
  The "value" of a button is the same as it's DOWN state.  These functions
  allow you to programatically get or set the DOWN state of a button.  If you
  set a button DOWN, and it is part of a button group, then the button that is
  currently DOWN in that group will be set UP.
  
void        SetAllowAllUp(bool inAllUp);
bool        GetAllowAllUp(void);
  For a group of buttons, only one button is allowed to be DOWN at a time.  If
  you set a button to be DOWN (either by clicking it or by calling SetDown()),
  then the previous DOWN button will be set to UP.
  However, you may choose to allow all buttons to be UP, or force one button
  to always be down.  By setting "Allow All Up" to true, then all buttons in
  a group may be set to the UP state; if set to false, then the group must
  always have at least one button in the DOWN state.
  
void        SetUserData(long inData);
long        GetUserData(void);
  This is simply a convenience, and allows the user to associate any "long int"
  data with the button.  On many platforms, a "long int" may also hold a
  pointer value (but this is completely platform dependant).

void        DoClick(bool inLeft);
  Calling this function simulates a mouse-click event, for either the LEFT
  or RIGHT mouse button.
 

----------------------   
Properties Description
----------------------

wxSpeedButton properties may be modified within the wxSmith GUI editor.  These
properties define the initial button declaration, layout, and function.  After
placing a button on your form (or panel, or whatever) and editing the items
that show up in the property panel, you may see the actual generated code by
viewing the *.cpp and *.h files associated with that GUI.

Thee are many properties that are common to all controls (for example, size
or position).  These "common" properties are not covered here, but only those
items which are unique or special to wxSpeedButton.

The following properties may be defined with wxSmith:

Label
type: wxString
  This is the text string that will appear on the button.

Glyph
type; image
  This is the image that will appear on the button.  The image selected may
  be one of the wxWidgets-provided images (from wxArtProvider) or from an
  external file.  If from a file, the image may contain 1, 2, or 3 sub-images;
  a sub-image each for the button UP, DOWN or disabled.

  If the image contains only a single image, then that image will be used for
  the button "up" and "down" states, and an internally-generated grey-scale
  image will be used for the "disabled" button.  If the image contains two 
  sub-images, the first sub-image will be used for the "up" and "down" states,
  and the second sub-image will be used for the "disabled" button.  If the
  image contains all three sub-images (or more) then the first sub-image is 
  displayed on the button when in the "up" state; the second sub-image will be 
  used when the button is in the "down" state, and the third sub-image will be 
  used when the button is disabled.

  When defined in this way, the images displayed on the button are drawn in a
  "transparent" manner.  If the image contains a transparency mask, then it will
  be used; if not, then a transparency mask will be generated using the
  bottom-left pixel of the image to determine the transparent color (this is the
  same method used in Delphi).  If, however, a seperate image is specified for
  a button state (using method calls SetGlyphUp() or SetGlyphDown()) then the
  transparency mask must be explicitly defined within the image itself.
  
GlyphCount
type: integer
  When the Glyph is specified as a file, this value specifies the numbber
  of sub-images contained in that file (normally 1, 2, or 3).  If this value
  is 0, then wxSpeedButton will attempt to determine the number of sub-images
  from the size of the dominant image, assuming that each sub-image is
  square-shaped (for example, 16 x 16, or 32 x 32).  On those occasiions
  where the sub-images are not computed correctly, you can explicitly set the
  number of sub-images to extract here.

Use XPM As #include
type: boolean
  If the Glyph is specified as a file, and that file is an XPM image, you
  have the option of loading the file at run-time or of including the file
  at compile-time (avoiding the problem of distributing image files with
  your application).  Note that if the file is #included'd at compile-time,
  the name of the data within the XPM file must be unique.  (The typical use
  is to name the data block inside the file similar to the name of the file;
  since XPM file are text files, naming the data block is trivial to do with
  any text editor).

Margin Space
type: integer
  This is the distance (in pixels) to be used to separate the glyph from the
  text label.

Button DOWN
type: boolean
  If TRUE, the button will start in the DOWN state, else it will start in 
  the UP state.

Button Type
type: multiple-choice
  This value will modify the group index value actully used in the declaration
  of the button.  A button may be a "simple button", a "toggle button", a
  "group button by parent" or a "group button by index".

Group Index
type: integer
  When the button type is "group button by index", this value specifies the
  group the button belongs to.  Multiple button groups may be defined; within
  a group the button work together so that only one button may be DOWN at any
  time, but different groups all work independantly of each other.

Allow All Up
type: boolean
  This is only used for a group button.  Within a group, only one button
  may be DOWN at any time, and normally there must be one button down at
  all times.  By setting this value TRUE, you may allow all buttons to be
  UP at the same time (although only still allow only one DOWN at any time).

User Data
type: integer
  Anything you want.  This is just a handy place to store extra data for
  that button.

Var Name
type: identifier
  This is the name (within your program) of the button.  By default, it is
  filled in automatically by wxSmith, but you can change it to something more
  descriptive of the button's function if you want.

Style
type: property sub-panel
  This property is included by default on all window-based components, 
  including buttons.  However, some extra items have been included in this
  component: wxBU_LEFT, wxBU_RIGHT, wxBU_TOP, and wxBU_BOTTOM, which 
  correspond to the type of image and label alignment to use.  By default,
  the image and label are aligned to the left of the button.


