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
* 26-Feb-2010
* 
*/



=============
wxMathPlot
=============

-------------------
General Description
-------------------

wxMathPlot is a framework for mathematical graph plotting in wxWindows.

The framework is designed for convenience and ease of use.

The heart of wxMathPlot is mpWindow, which is a 2D canvas for plot layers.
mpWindow can be embedded as subwindow in a wxPane, a wxFrame, or any other 
wxWindow.  mpWindow provides a zoomable and moveable view of the layers. The 
current view can be controlled with the mouse, the scrollbars, and a context 
menu.

Plot layers are implementations of the abstract base class mpLayer. Those can
be function plots, scale rulers, or any other vector data visualisation. 
wxMathPlot provides two mpLayer implementations for plotting horizontal and 
vertical rulers: mpScaleX and mpScaleY.  For convenient function plotting three 
more abstract base classes derived from mpLayer are provided: mpFX, mpFY and 
mpFXY. These base classes already come with plot code, own functions can be 
implemented by overiding just one member for retrieving a function value.
Another class, mpFXYVector, is provided since version 0.03. This non-virtual 
class renders 2D graphs from a pair of std::vector's. One of the utility of this 
class is that you can use it directly in GUI designers such as wxSmith within 
Code::Blocks.

From version 0.03, the mpWindow has built-in support for mouse-based pan and 
zoom through intuitive combinations of buttons and the mouse wheel. It also 
incorporate an optional double buffering mechanism to avoid flicker.

wxMathPlot sticks to wxWindow's coding conventions. All entities defined by 
wxMathPlot have the prefix "mp".

wxMathPlot is published under the terms of the wxWindow license.
The original author is David Schalig <mrhill@users.sourceforge.net>.
From June 2007 the project is maintained by Davide Rondini. 
<cdron@users.sourceforge.net>.
Authors can be contacted via the wxMathPlot's homepage at
http://sourceforge.net/projects/wxmathplot.
Contributors:
Jose Luis Blanco <jlblanco@ctima.uma.es>, Val Greene.



The wxMathPlot build directory has the following structure:
MathPlot\
    bin\                // this dir contains the binary file output of the 
                        // Code::Blocks projects; files include "TestPlot.exe",
                        // "wxsSmithPlot.dll",  "wxsSmithPlot.zip", and 
                        // "CopySmithPlot.bat"    
    images\             // a set of usefull XPM images for buttons         
    include\            // the include file "wxMathPlot.h" is here
    lib\                // the library "wxMathPlot.a" is here
    obj\                // intermediate files for all compiles
    project\            // this is the Code::Bocks project to create the
                        // wxMathPlot library; it creates the file
                        // "wxMathPlot.a" and places the library file and
                        // the include file in the proper directories 
    src\                // source files for component wxMathPlot
    test\               // this is the Code::Blocks project to create a test
                        // program for wxMathPlot; the output file
                        // "TestPlot.exe" is placed in the ..\bin\ directory
        wxsmith\        // used by Code::Blocks and wxSmith
    wxs\                // this is the Code::Blocks project to create the
                        // plugin for Code::Blocks and wxSmith, to put the
                        // component wxMathPlot on the wxSmith palette.
                        // the output files "wxsSmithPlot.dll" and
                        // and "wxsSmithPlot.zip" are placed in the ..\bin\
                        // directory; they must be manually copied to the
                        // plugin directory of your Code::Blocks installation.
                        // see the batch file "CopySmithPlot.bat" to
                        // make this easier  
        images\         // images used on the wxSmith palette 
        src\            // source code for the property pages used by
                        // wxSmith for the wxMathPlot component

The Code::Blocks projects used here were created and tested under Windows XP,
using Code::Blocks SVN version 5866, and wxWidgets version 2.8.  The correct 
order to build these projects is:

1) Build the library (project found in the "project\" directory)
2) Build the wxSmith plugin (project found in the "wxs\" directory)
3) Exit Code::Blocks and install the plugins
   copy "bin\wxsSmithPlot.dll" to "<Code::Blocks>\share\CodeBlocks\plugins"
   copy "bin\wxsSmithPlot.zip" to "<Code::Blocks>\share\CodeBlocks"
4) Re-start Code::Blocks and build the test program (project found in the
   "test\" directory)

(For Windows XP, all of these projects and programs are already compiled; you
only need to install the plugin files into your own Code::Blocks installation
directory).


 

----------------------   
Properties Description
----------------------

WxMathPlot has a plugin for Code::Blocks, that works with the wxSmith GUI
designer.  This plugin makes 4 components available on the wxSmith component
pallette, found on the "MathPlot" tab: mpWindow, mpAxis, mpVector, and
mpMarker.

mpWindow
--------
This is an implementation of the "mpWindow" described in "mathplot.h".  The
properties tab allows you to preset all the attributes normally associated
with any widget component (size, position, background colour, etc).  An
mpWindow simply provides a place to draw lines and curves.

The mpWindow also handles almost all of the "detail" work associated with
displaying a large set of numerical data; details such as setting scale,
aligning the axis and tic-marks, and fitting the data to the display area
are all taken care of by mpWindow.

In addition, mpWindow provides a popup menu (right-click on the mouse) that
allows the user to zoom in, zoom out, and move the center of the plot.

mpAxis
------
An axis displays a line with (optional) tic-marks, and provides a scale with
which to measure the relative amplitude of values plotted upon the mpWindow.

When a mpAxis is selected and placed upon an mpWindow, a text-label widget is
displayed on the GUI designer with the name of the mpAxis.  This text-label
is simply a "place holder", and is used only for designer purposes.  It does
not appear on the final (compiled) display.  By clicking on the place-holder,
the properties for the axis appear in the property tab, and allows the user to
select attibutes such as type (X-axis or Y-axis), axis label, and colour.

mpVector
--------
A vector is an implementation of mpFXYVector() (described in "mathplot.h").
This component allows the user to define numerical data as a set of X,Y data
points and draw them as a single curve on the mpWindow.  (Other vector types
such as mpFX() or mpFY() may only be implemented programattically by the user,
as they return X or Y data only on demand).

When a vector is selected and placed upon an mpWindow, a text-label widget is
displayed on the GUI designer with the name of the mpVector.  This text-label
is simply a "place holder", and is used only for designer purposes.  It does
not appear on the final (compiled) display.  By clicking on the place-holder,
the properties for the vector appear in the property tab, and allows the user to
select attibutes such as vector label, continuous line, and colour.  It also
allows the user the option to pre-load data for the vector.

When pre-loading data for a vector, define the data as a set of X,Y points; 
this is, 2 floating-point numbers seperated by a comma.  Blank lines and lines
starting with "!" are ignored when mpVector reads the data.  Also, since the
data is stored as a wxArrayString, there is a limit as to how many lines may
be entered (currently about 16 000 lines maximum).

mpMarker
--------
A marker is simply a text label that appears at a given location on the
mpWindow, located in graph units.  While other text labels are positioned
relative to the edges of the mpWindow (such as north-east corner or along
the bottom edge) a marker is positioned just as if it were another numerical
datum (for example, at position [0.0, 0.0] or position [-5.0, 11.5]).  The
text is centered upon the specified point. 
      

