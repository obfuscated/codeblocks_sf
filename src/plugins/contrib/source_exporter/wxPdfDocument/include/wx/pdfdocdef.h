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
A separate detailed description is available for the \ref makefont.

wxPdfDocument is hosted as a component of <a href="http://wxcode.sourceforge.net"><b>wxCode</b></a>.
For any remark, question or problem, you can leave a message on the appropriate \b wxCode
tracker accessible from the <a href="http://wxcode.sourceforge.net/support.php"> wxCode support</a>
page. Or you can send a mail to me 
<a href="&#109;&#97;&#105;&#108;&#116;&#111;:&#117;&#108;&#114;&#105;&#99;&#104;&#46;&#116;&#101;&#108;&#108;&#101;&#64;&#103;&#109;&#120;&#46;&#100;&#101;">directly</a>.

\section version Version history

\todo Although all features were thoroughly tested individually, not all possible combinations were
verified to function properly. This means: wxPdfDocument still needs intensive testing. 
<b>If you find bugs please report them to the author!</b>

<dl>
<dt><b>0.6</b> - <i>November 2005</i></dt>
<dd>
Added features: gradients, transformations, barcodes<br>
MakeFont utility<br>
Changed API of graphics primitives: line style and fill color parameters deleted,
line style and fill color have to be set using SetLineStyle and SetFillColor.
</dd>

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

- Olivier Plathey (FPDF, Barcodes, Bookmarks, Rotation),
- Maxime Delorme (Sector)
- Martin Hall-May (WMF images)
- Emmanuel Havet (Code39 barcodes)
- Shailesh Humbad (POSTNET barcodes)
- Pierre Marletta (Diagrams)
- Matthias Lau (i25 barcodes)
- David Hernández Sanz (additional graphics primitives)
- Klemen Vodopivec (Protection)
- Moritz Wagner (Transformation)
- Andreas Würmser (Clipping, Gradients, Transformation)

*/

/** \page overview Reference Manual
The documentation of wxPdfDocument is created by Doxygen. To make it easier to locate the description
of a specific method the following alphabetical list shows all available methods:

\section refpdfdoc wxPdfDocument

\li wxPdfDocument::AcceptPageBreak - accept or not automatic page break
\li wxPdfDocument::AddFont - add a new font
\li wxPdfDocument::AddFontCJK - add a CJK (Chinese, Japanese or Korean) font
\li wxPdfDocument::AddLink - create an internal link
\li wxPdfDocument::AddPage - add a new page
\li wxPdfDocument::AliasNbPages - define an alias for number of pages
\li wxPdfDocument::Arrow - draw an arrow

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
\li wxPdfDocument::GetCellMargin - get the cell margin
\li wxPdfDocument::GetImageScale - get image scale
\li wxPdfDocument::GetLeftMargin - get the left margin
\li wxPdfDocument::GetPageWidth - get page width
\li wxPdfDocument::GetPageHeight - get page height
\li wxPdfDocument::GetRightMargin - get the right margin
\li wxPdfDocument::GetScaleFactor - get scale factor
\li wxPdfDocument::GetStringWidth - compute string length
\li wxPdfDocument::GetTopMargin - get the top margin
\li wxPdfDocument::GetX - get current x position
\li wxPdfDocument::GetY - get current y position

\li wxPdfDocument::Header - page header

\li wxPdfDocument::Image - output an image

\li wxPdfDocument::Line - draw a line
\li wxPdfDocument::LinearGradient - paint a linear gradient shading to rectangular area
\li wxPdfDocument::Link - put a link
\li wxPdfDocument::Ln - line break

\li wxPdfDocument::Marker - draw a marker symbol
\li wxPdfDocument::MirrorH - mirror horizontally
\li wxPdfDocument::MirrorV - mirror vertically
\li wxPdfDocument::MultiCell - print text with line breaks

\li wxPdfDocument::Open - start output to the PDF document

\li wxPdfDocument::PageNo - page number
\li wxPdfDocument::Polygon - draw a polygon

\li wxPdfDocument::RadialGradient - paint a radial gradient shading to rectangular area
\li wxPdfDocument::Rect - draw a rectangle
\li wxPdfDocument::RegularPolygon -  draw a regular polygon
\li wxPdfDocument::Rotate - rotate around a given center
\li wxPdfDocument::RotatedImage - rotate image
\li wxPdfDocument::RotatedText - rotate text string
\li wxPdfDocument::RoundedRect - draw a rounded rectangle

\li wxPdfDocument::SaveAsFile - save the document to a file
\li wxPdfDocument::Scale - scale in X and Y direction
\li wxPdfDocument::ScaleX - scale in X direction only
\li wxPdfDocument::ScaleXY - scale equally in X and Y direction
\li wxPdfDocument::ScaleY - scale in Y direction only
\li wxPdfDocument::Sector - draw a sector
\li wxPdfDocument::SetAuthor - set the document author
\li wxPdfDocument::SetAutoPageBreak - set the automatic page breaking mode
\li wxPdfDocument::SetCellMargin - set cell margin
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
\li wxPdfDocument::Skew - skew in X and Y direction
\li wxPdfDocument::SkewX - skew in Y direction only
\li wxPdfDocument::SkewY - skew in Y direction only
\li wxPdfDocument::StarPolygon - draw star polygon
\li wxPdfDocument::StartTransform - begin transformation
\li wxPdfDocument::StopTransform - end transformation

\li wxPdfDocument::Text - print a string
\li wxPdfDocument::Translate - move the origin 
\li wxPdfDocument::TranslateX - move the X origin only
\li wxPdfDocument::TranslateY - move the Y origin only

\li wxPdfDocument::UnsetClipping - remove clipping area

\li wxPdfDocument::Write - print flowing text
\li wxPdfDocument::WriteCell - print flowing text with cell attributes

\li wxPdfDocument::wxPdfDocument - constructor

\section refpdfbarcode wxPdfBarCodeCreator

\li wxPdfBarCodeCreator::Code39
\li wxPdfBarCodeCreator::EAN13
\li wxPdfBarCodeCreator::UPC_A
\li wxPdfBarCodeCreator::I25
\li wxPdfBarCodeCreator::PostNet


*/

/** \page makefont MakeFont Utility
\section mkfontadd Adding new fonts and encoding support

This section explains how to use \b TrueType or \b Type1 fonts so that you are not
limited to the standard fonts any more. The other interest is that you can
choose the font encoding, which allows you to use other languages than the
Western ones (the standard fonts having too few available characters). 
 
There are two ways to use a new font: embedding it in the PDF or not. When a
font is not embedded, it is sought in the system. The advantage is that the
PDF file is lighter; on the other hand, if it is not available, a substitution
font is used. So it is preferable to ensure that the needed font is installed
on the client systems. If the file is to be viewed by a large audience, it is
better to embed the fonts. 
 
Adding a new font requires three steps for \b TrueType fons: 

\li Generation of the metric file (.afm) 
\li Generation of the font definition file (.xml) 
\li Declaration of the font in the program 

For \b Type1, the first one is theoretically not necessary because the AFM file is
usually shipped with the font. In case you have only a metric file in PFM format,
it must be converted to AFM first. 

\section mkfontgen1 Generation of the metric file

The first step for a \b TrueType font consists in generating the AFM file (or UFM file in case of a 
<b>Unicode TrueType</b> font). A utility exists to do this task: <tt>ttf2ufm</tt> - a special version of
<tt>ttf2pt1</tt> - allowing to create AFM and/or UFM files. An archive containing the source
code of <tt>ttf2ufm</tt> and a Windows executable can be downloaded from
<a href="http://wxcode.sourceforge.net/docs/wxpdfdoc/ttf2ufm.zip">here</a>.
The command line to use is the following: 
 
<tt>ttf2ufm -a font.ttf font </tt>
 
For example, for Comic Sans MS Regular: 
 
<tt>ttf2ufm -a c:/windows/fonts/comic.ttf comic </tt>
 
Two files are created; the one we are interested in is comic.afm. 

\section mkfontgen2 Generation of the font definition file

The second step consists in generating a wxPdfDocument font metrics XML file containing
all the information needed by wxPdfDocument; in addition, the font file is compressed.
To do this, a utility program, \b makefont, is provided.

<tt>makefont {-a font.afm | -u font.ufm } [-f font.{ttf|pfb}] [-e encoding] [-p patch] [-t {ttf|t1}]</tt>

<table border=0>
<tr><td><tt>-a font.afm</tt></td><td>AFM font metric file for \b TrueType or \b Type1 fonts</td></tr>
<tr><td><tt>-u font.ufm</tt></td><td>UFM font metric file for <b>TrueType Unicode</b> fonts</td></tr>
<tr><td valign="top"><tt>-f font.{ttf|pfb}</tt></td><td>font file (<tt>.ttf</tt> = TrueType, <tt>.pfb</tt> = Type1).
<br>If you own a Type1 font in ASCII format (<tt>.pfa</tt>), you can convert it to binary format with
<a href="http://www.lcdf.org/~eddietwo/type/#t1utils">t1utils</a>.
<br>If you don't want to embed the font, omit this parameter. In this case, type is given by the type parameter. 
</td></tr>
<tr><td valign="top"><tt>-e encoding</tt></td><td>font encoding, i.e cp1252. Omit this parameter for a symbolic font.like <i>Symbol</i>
or <i>ZapfDingBats</i>.

The encoding defines the association between a code (from 0 to 255) and a character.
The first 128 are fixed and correspond to ASCII; the following are variable.
The encodings are stored in .map files. Those available are: 

\li cp1250 (Central Europe) 
\li cp1251 (Cyrillic) 
\li cp1252 (Western Europe) 
\li cp1253 (Greek) 
\li cp1254 (Turkish) 
\li cp1255 (Hebrew) 
\li cp1257 (Baltic) 
\li cp1258 (Vietnamese) 
\li cp874 (Thai) 
\li iso-8859-1 (Western Europe) 
\li iso-8859-2 (Central Europe) 
\li iso-8859-4 (Baltic) 
\li iso-8859-5 (Cyrillic) 
\li iso-8859-7 (Greek) 
\li iso-8859-9 (Turkish) 
\li iso-8859-11 (Thai) 
\li iso-8859-15 (Western Europe) 
\li iso-8859-16 (Central Europe) 
\li koi8-r (Russian) 
\li koi8-u (Ukrainian) 

Of course, the font must contain the characters corresponding to the chosen encoding. 
The encodings which begin with cp are those used by Windows; Linux systems usually use ISO. 
Remark: the standard fonts use cp1252. 

</td></tr>
<tr><td valign="top"><tt>-p patch</tt></td><td>patch file for individual encoding changes.
Use the same format as the <tt>.map</tt> files for encodings.
A patch file gives the possibility to alter the encoding.
Sometimes you may want to add some characters. For instance, ISO-8859-1 does not contain
the euro symbol. To add it at position 164, create a file containing the line
<p><tt>!A0 U+20AC Euro</tt>
<p>\b Note: The Unicode character id will not be interpreted.
</td></tr>
<tr><td><tt>-t {ttf|t1}</tt></td><td>font type (ttf = TrueType, t1 = Type1). Only needed if omitting the font file.</td></tr>
</table>

\b Note: in the case of a font with the same name as a standard one, for instance arial.ttf,
it is mandatory to embed. If you don't, Acrobat will use its own font. 

Executing <tt>makefont</tt> generates an .xml file, with the same name as the
<tt>.afm</tt> file resp. <tt>.ufm</tt> file. You may rename it if you wish. In case of
embedding the font file is compressed and gives a file with <tt>.z</tt> as extension.
For <b>Unicode TrueType</b> fonts a file with extension <tt>.ctg.z</tt> is created containing
the character to glyph mapping.You may rename these files, too, but in this case you have to
alter the file name(s) in the file tag in the <tt>.xml</tt> file accordingly.

You have to copy the generated file(s) to the font directory.

\section mkfontdecl Declaration of the font in the script

The last step is the most simple. You just need to call the AddFont() method. For instance: 
 
<tt>pdf.AddFont(_T("Comic"),_T(""),_T("comic.xml"));</tt>
  
or simply: 
 
<tt>pdf.AddFont(_T("Comic"));</tt>
  
And the font is now available (in regular and underlined styles), usable like the others.
If we had worked with Comic Sans MS Bold (comicbd.ttf), we would have put: 
 
<tt>pdf.AddFont(_T("Comic"),_T("B"),_T("comicbd.xml"));</tt>
  
\section mkfontreduce Reducing the size of TrueType fonts

Font files are often quite voluminous; this is due to the
fact that they contain the characters corresponding to many encodings. zlib compression
reduces them but they remain fairly big. A technique exists to reduce them further.
It consists in converting the font to the \b Type1 format with <tt>ttf2pt1</tt> by specifying the
encoding you are interested in; all other characters will be discarded. 
For instance, the arial.ttf font shipped with Windows 98 is 267KB (it contains 1296
characters). After compression it gives 147. Let's convert it to \b Type1 by keeping
only cp1250 characters: 
 
<tt>ttf2ufm -b -L cp1250.map c:/windows/fonts/arial.ttf arial </tt>
 
The <tt>.map</tt> files are located in the <tt>makefont</tt> directory.
The command produces arial.pfb and arial.afm. The arial.pfb file is only 35KB,
and 30KB after compression. 
 
It is possible to go even further. If you are interested only in a subset of the
encoding (you probably don't need all 217 characters), you can open the .map file
and remove the lines you are not interested in. This will reduce the file size
accordingly. 

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

