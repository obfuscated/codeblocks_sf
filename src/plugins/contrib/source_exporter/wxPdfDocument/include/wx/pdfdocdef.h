///////////////////////////////////////////////////////////////////////////////
// Name:        pdfdocdef.h
// Purpose:     
// Author:      Ulrich Telle
// Modified by:
// Created:     2005-08-04
// Copyright:   (c) Ulrich Telle
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

/// \file pdfdocdef.h Compile time switches for the \b wxPdfDocument component

/** \mainpage wxPdfDocument

\section intro What is wxPdfDocument?

wxPdfDocument is a C++ class which allows wxWidgets applications to generate PDF files.
The code is a port of <a href="http://www.fpdf.org"><b>FPDF</b></a> - a free PHP class for
generating PDF files - to C++ using the <a href="http://www.wxwidgets.org"><b>wxWidgets</b></a>
library. wxPdfDocument does not make use of any libraries like
<a href="http://www.pdflib.com"><b>PDFlib</b></a> or
<a href="http://www.fastio.com"><b>ClibPDF</b></a> which require a fee at least for
commercial usage. wxPdfDocument is published under the <b>wxWidgets (formerly wxWindows)
license</b>. This means you may use it for any kind of usage and modify it to suit your needs.

wxPdfDocument offers all advantages of \b FPDF.  Several add-on PHP scripts found on the
FPDF web site are incorporated into wxPdfDocument. The main features are:

- Choice of measure unit, page format and margins 
- Page header and footer management 
- Automatic page break 
- Automatic line break and text justification 
- Image support (GIF, JPEG, PNG and WMF) 
- Colors 
- Links
- 14 Adobe standard fonts
- TrueType and Type1 fonts (with or without embedding) and encoding support
- TrueType Unicode and Type0 fonts (for Chinese, Japanese and Korean) support in the Unicode build
- Page compression 
- Graphics primitives for the creation of simple drawings
- Definition of clipping areas
- Bookmarks for outlining the document 
- Rotation
- Protecting the document by passwords and/or access permissions

The class can produce documents in many languages other than the Western European ones:
Central European, Cyrillic, Greek, Baltic and Thai, provided you own TrueType or Type1
fonts with the desired character set. In the Unicode build Chinese, Japanese and Korean
are supported, too.

A \ref overview showing all available methods in alphabetical order is provided.
A sample application including more than 10 examples demonstrates the different features.

wxPdfDocument is hosted as a component of <a href="http://wxcode.sourceforge.net"><b>wxCode</b></a>.
For any remark, question or problem, you can leave a message on the appropriate \b wxCode
tracker accessible from the <a href="http://wxcode.sourceforge.net/support.php"> wxCode support</a>
page. Or you can send a mail to me 
<a href="&#109;&#97;&#105;&#108;&#116;&#111;:&#117;&#108;&#114;&#105;&#99;&#104;&#46;&#116;&#101;&#108;&#108;&#101;&#64;&#103;&#109;&#120;&#46;&#100;&#101;">directly</a>.

\section version Version history

\todo All included font definition files were created manually. <i>Very inconvenient</i>. 
Therefore work on a \b MakeFont utility to create these from font metrics files
and code maps is in progress.

\todo Although all features were thoroughly tested individually, not all possible combinations were
verified to function properly. This means: wxPdfDocument still needs intensive testing. 
<b>If you find bugs please report them to the author!</b>

<dl>
<dt><b>0.5</b> - <i>September 2005</i></dt>
<dd>
First public release
</dd>

<dt><b>0.4</b> - <i>August 2005</i></dt>
<dd>
Support for embedding fonts
</dd>

<dt><b>0.3</b> - <i>July 2005</i></dt>
<dd>
Support for embedding images
</dd>

<dt><b>0.2</b> - <i>June 2005</i></dt>
<dd>
Several add-ons implemented
</dd>

<dt><b>0.1</b> - <i>May 2005</i></dt>
<dd>
Planning and basic PDF features implemented
</dd>
</dl>

\author Ulrich Telle (<a href="&#109;&#97;&#105;&#108;&#116;&#111;:&#117;&#108;&#114;&#105;&#99;&#104;&#46;&#116;&#101;&#108;&#108;&#101;&#64;&#103;&#109;&#120;&#46;&#100;&#101;">ulrich DOT telle AT gmx DOT de</a>)

\section acknowledgement Acknowledgement

Since wxPdfDocument is based on the great \b FPDF PHP class and several of the contributions to it
found on the <a href="http://www.fpdf.org"><b>FPDF website</b></a> I would like to thank 

- Olivier (FPDF, Bookmarks, Rotation),
- Andreas Würmser (Clipping)
- David Hernández Sanz (additional graphics primitives)
- Martin Hall-May (WMF images)
- Klemen Vodopivec (Protection)

*/

/** \page overview Reference Manual
The documentation of wxPdfDocument is created by Doxygen. To make it easier to locate the description
of a specific method the following alphabetical list shows all available methods:

\li wxPdfDocument::AcceptPageBreak - accept or not automatic page break
\li wxPdfDocument::AddFont - add a new font
\li wxPdfDocument::AddFontCJK - add a CJK (Chinese, Japanese or Korean) font
\li wxPdfDocument::AddLink - create an internal link
\li wxPdfDocument::AddPage - add a new page
\li wxPdfDocument::AliasNbPages - define an alias for number of pages

\li wxPdfDocument::Bookmark - add a bookmark

\li wxPdfDocument::Cell - print a cell
\li wxPdfDocument::Circle - draw a circle
\li wxPdfDocument::ClippingText - define text as clipping area
\li wxPdfDocument::ClippingRect - define rectangle as clipping area
\li wxPdfDocument::ClippingEllipse - define ellipse as clipping area
\li wxPdfDocument::ClippedCell - print a clipped cell
\li wxPdfDocument::Close - terminate the document
\li wxPdfDocument::Curve - draw a Bézier curve

\li wxPdfDocument::Ellipse - draw an ellipse

\li wxPdfDocument::Footer - page footer.

\li wxPdfDocument::GetBreakMargin - get the page break margin
\li wxPdfDocument::GetImageScale - get image scale
\li wxPdfDocument::GetPageWidth - get page width
\li wxPdfDocument::GetPageHeight - get page height
\li wxPdfDocument::GetScaleFactor - get scale factor
\li wxPdfDocument::GetStringWidth - compute string length
\li wxPdfDocument::GetX - get current x position
\li wxPdfDocument::GetY - get current y position

\li wxPdfDocument::Header - page header

\li wxPdfDocument::Image - output an image

\li wxPdfDocument::Line - draw a line
\li wxPdfDocument::Link - put a link
\li wxPdfDocument::Ln - line break

\li wxPdfDocument::MultiCell - print text with line breaks

\li wxPdfDocument::Open - start output to the PDF document

\li wxPdfDocument::PageNo - page number
\li wxPdfDocument::Polygon - draw a polygon

\li wxPdfDocument::Rect - draw a rectangle
\li wxPdfDocument::RegularPolygon -  draw a regular polygon
\li wxPdfDocument::Rotate - rotate around a given center
\li wxPdfDocument::RotatedImage - rotate image
\li wxPdfDocument::RotatedText - rotate text string
\li wxPdfDocument::RoundedRect - draw a rounded rectangle

\li wxPdfDocument::SaveAsFile - save the document to a file
\li wxPdfDocument::SetAuthor - set the document author
\li wxPdfDocument::SetAutoPageBreak - set the automatic page breaking mode
\li wxPdfDocument::SetCompression - turn compression on or off
\li wxPdfDocument::SetCreator - set document creator
\li wxPdfDocument::SetDisplayMode - set display mode
\li wxPdfDocument::SetDrawColor - set drawing color
\li wxPdfDocument::SetFillColor - set filling color
\li wxPdfDocument::SetFont - set font
\li wxPdfDocument::SetFontSize - set font size
\li wxPdfDocument::SetImageScale - set image scale
\li wxPdfDocument::SetKeywords - associate keywords with document

\li wxPdfDocument::SetLeftMargin - set left margin
\li wxPdfDocument::SetLineStyle - set line style
\li wxPdfDocument::SetLineWidth - set line width
\li wxPdfDocument::SetLink - set internal link destination
\li wxPdfDocument::SetMargins - set margins
\li wxPdfDocument::SetProtection - set permissions and/or passwords
\li wxPdfDocument::SetRightMargin - set right margin
\li wxPdfDocument::SetSubject - set document subject
\li wxPdfDocument::SetTextColor - set text color
\li wxPdfDocument::SetTitle - set document title
\li wxPdfDocument::SetTopMargin - set top margin
\li wxPdfDocument::SetX - set current x position
\li wxPdfDocument::SetXY - set current x and y positions
\li wxPdfDocument::SetY - set current y position
\li wxPdfDocument::StarPolygon - draw star polygon

\li wxPdfDocument::Text - print a string

\li wxPdfDocument::UnsetClipping - remove clipping area

\li wxPdfDocument::Write - print flowing text

\li wxPdfDocument::wxPdfDocument - constructor

*/

#ifndef _PDFDOC_DEF_H_
#define _PDFDOC_DEF_H_

#ifdef WXMAKINGDLL_WXPDFDOC
    #define WXDLLIMPEXP_PDFDOC WXEXPORT
#elif defined(WXUSINGDLL_WXPDFDOC)
    #define WXDLLIMPEXP_PDFDOC WXIMPORT
#else // not making nor using DLL
    #define WXDLLIMPEXP_PDFDOC
#endif

#endif // _PDFDOC_DEF_H_

