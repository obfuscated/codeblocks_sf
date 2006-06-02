///////////////////////////////////////////////////////////////////////////////
// Name:        pdfdoc.h
// Purpose:     
// Author:      Ulrich Telle
// Modified by:
// Created:     2005-08-04
// Copyright:   (c) Ulrich Telle
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

/// \file pdfdoc.h Interface of the wxPdfDocument class

#ifndef _PDFDOC_H_
#define _PDFDOC_H_

#if 0 // Use of pragma seems to cause problems
#if defined(__GNUG__) && !defined(__APPLE__)
    #pragma interface "pdfdoc.h"
#endif
#endif

#include <wx/dynarray.h>
#include <wx/mstream.h>

#include "wx/pdfdocdef.h"
#include "wx/pdfencrypt.h"
#include "wx/pdffont.h"
#include "wx/pdfimage.h"

#define wxPDF_PRODUCER       _T("wxPdfDocument 0.7.0")

#define wxPDF_EPSILON        1e-6

/// Border options
#define wxPDF_BORDER_NONE    0x0000
#define wxPDF_BORDER_LEFT    0x0001
#define wxPDF_BORDER_RIGHT   0x0002
#define wxPDF_BORDER_TOP     0x0004
#define wxPDF_BORDER_BOTTOM  0x0008
#define wxPDF_BORDER_FRAME   0x000F

/// Corner options
#define wxPDF_CORNER_NONE          0x0000
#define wxPDF_CORNER_TOP_LEFT      0x0001
#define wxPDF_CORNER_TOP_RIGHT     0x0002
#define wxPDF_CORNER_BOTTOM_LEFT   0x0004
#define wxPDF_CORNER_BOTTOM_RIGHT  0x0008
#define wxPDF_CORNER_ALL           0x000F

/// Style options
#define wxPDF_STYLE_DRAW      0x0001
#define wxPDF_STYLE_FILL      0x0002
#define wxPDF_STYLE_FILLDRAW  0x0003
#define wxPDF_STYLE_DRAWCLOSE 0x0004
#define wxPDF_STYLE_MASK      0x0007

/// Font decoration options
#define wxPDF_FONT_NORMAL     0x0000
#define wxPDF_FONT_UNDERLINE  0x0001
#define wxPDF_FONT_OVERLINE   0x0002
#define wxPDF_FONT_STRIKEOUT  0x0004
#define wxPDF_FONT_DECORATION 0x0007 // Mask all possible decorations


/// Permission options
#define wxPDF_PERMISSION_PRINT  0x0004  ///< Allow printing
#define wxPDF_PERMISSION_MODIFY 0x0008  ///< Allow modifying
#define wxPDF_PERMISSION_COPY   0x0010  ///< Allow text copying
#define wxPDF_PERMISSION_ANNOT  0x0020  ///< Allow annotations

/// Form field border styles
enum wxPdfBorderStyle
{
  wxPDF_BORDER_SOLID,
  wxPDF_BORDER_DASHED,
  wxPDF_BORDER_BEVELED,
  wxPDF_BORDER_INSET,
  wxPDF_BORDER_UNDERLINE
};

/// Alignment options
enum wxPdfAlignment
{
  wxPDF_ALIGN_LEFT,
  wxPDF_ALIGN_CENTER,
  wxPDF_ALIGN_RIGHT,
  wxPDF_ALIGN_JUSTIFY,
  wxPDF_ALIGN_TOP    = wxPDF_ALIGN_LEFT,
  wxPDF_ALIGN_MIDDLE = wxPDF_ALIGN_CENTER,
  wxPDF_ALIGN_BOTTOM = wxPDF_ALIGN_RIGHT
};

/// Zoom options
enum wxPdfZoom
{
  wxPDF_ZOOM_FULLPAGE,
  wxPDF_ZOOM_FULLWIDTH,
  wxPDF_ZOOM_REAL,
  wxPDF_ZOOM_DEFAULT,
  wxPDF_ZOOM_FACTOR
};

/// Layout options
enum wxPdfLayout
{
  wxPDF_LAYOUT_CONTINUOUS,
  wxPDF_LAYOUT_SINGLE,
  wxPDF_LAYOUT_TWO,
  wxPDF_LAYOUT_DEFAULT
};

/// Line Cap options
enum wxPdfLineCap
{
  wxPDF_LINECAP_NONE   = -1,
  wxPDF_LINECAP_BUTT   = 0,
  wxPDF_LINECAP_ROUND  = 1,
  wxPDF_LINECAP_SQUARE = 2
};

/// Line join options
enum wxPdfLineJoin
{
  wxPDF_LINEJOIN_NONE  = -1,
  wxPDF_LINEJOIN_MITER = 0,
  wxPDF_LINEJOIN_ROUND = 1,
  wxPDF_LINEJOIN_BEVEL = 2
};

/// Marker symbols
enum wxPdfMarker
{
  wxPDF_MARKER_CIRCLE,
  wxPDF_MARKER_SQUARE,
  wxPDF_MARKER_TRIANGLE_UP,
  wxPDF_MARKER_TRIANGLE_DOWN,
  wxPDF_MARKER_TRIANGLE_LEFT,
  wxPDF_MARKER_TRIANGLE_RIGHT,
  wxPDF_MARKER_DIAMOND,
  wxPDF_MARKER_PENTAGON_UP,
  wxPDF_MARKER_PENTAGON_DOWN,
  wxPDF_MARKER_PENTAGON_LEFT,
  wxPDF_MARKER_PENTAGON_RIGHT,
  wxPDF_MARKER_STAR,
  wxPDF_MARKER_STAR4,
  wxPDF_MARKER_PLUS,
  wxPDF_MARKER_CROSS,
  wxPDF_MARKER_SUN,
  wxPDF_MARKER_BOWTIE_HORIZONTAL,
  wxPDF_MARKER_BOWTIE_VERTICAL,
  wxPDF_MARKER_ASTERISK,
  wxPDF_MARKER_LAST  // Marks the last available marker symbol; do not use!
};

/// Class representing internal or external links.
class WXDLLIMPEXP_PDFDOC wxPdfLink
{
public:
  /// Constructor for internal link
  /**
  * Use this constructor to create an \b internal link reference.
  * \see wxPdfDocument::Link(), wxPdfDocument::Write(), wxPdfDocument::Cell(), wxPdfDocument::ClippedCell(), wxPdfDocument::Image(), wxPdfDocument::RotatedImage()
  */
  wxPdfLink(int linkRef);

  /// Constructor for external link
  /**
  * Use this constructor to create an \b external link reference.
  * \see wxPdfDocument::Link(), wxPdfDocument::Write(), wxPdfDocument::Cell(), wxPdfDocument::ClippedCell(), wxPdfDocument::Image(), wxPdfDocument::RotatedImage()
  */
  wxPdfLink(const wxString& linkURL);

  /// Copy constructor
  wxPdfLink(const wxPdfLink& pdfLink);

  /// Destructor
  virtual ~wxPdfLink();

  /// Check whether this instance is a valid link reference
  bool  IsValid() const { return m_isValid; }

  /// Check whether this instance is an internal reference
  bool  IsLinkRef() const { return m_isRef; }

  /// Get the internal link reference
  int   GetLinkRef() const { return m_linkRef; }

  /// Get the external link reference
  const wxString GetLinkURL() const { return m_linkURL; }

  /// Set page number and position on page
  void   SetLink(int page, double ypos) { m_page = page; m_ypos = ypos; }

  /// Get the page this link refers to
  int    GetPage() { return m_page; }

  /// Get the page position this link refers to
  double GetPosition() { return m_ypos; }

private:
  bool     m_isValid;   ///< Flag whether this instance is valid
  bool     m_isRef;     ///< Flag whether this is an internal link reference
  int      m_linkRef;   ///< Internal link reference
  wxString m_linkURL;   ///< External link reference
  int      m_page;      ///< Page number this link refers to
  double   m_ypos;      ///< Position on page this link refers to
};

/// Class representing the sensitive area of links referring to a page. (For internal use only)
class WXDLLIMPEXP_PDFDOC wxPdfPageLink : public wxPdfLink
{
public:
  /// Constructor
  wxPdfPageLink(double x, double y, double w, double h, const wxPdfLink& pdfLink);

  /// Destructor
  virtual ~wxPdfPageLink();

  /// Get the X offset
  double GetX() { return m_x; }

  /// Get the Y offset
  double GetY() { return m_y; }

  /// Get the width
  double GetWidth() { return m_w; }

  /// Get the height
  double GetHeight() { return m_h; }

private:
  double m_x;   ///< X offset of sensitive area
  double m_y;   ///< Y offset of sensitive area
  double m_w;   ///< Width of sensitive area
  double m_h;   ///< Height of sensitive area
};

/// Class representing text annotations.
class WXDLLIMPEXP_PDFDOC wxPdfAnnotation
{
public:
  /// Constructor for text annotation
  /**
  * Use this constructor to create a text annotation.
  * \param x X offset of the annotation
  * \param y Y offset of the annotation
  * \param text annotation text
  */
  wxPdfAnnotation(double x, double y, const wxString& text);

  /// Copy constructor
  wxPdfAnnotation(const wxPdfAnnotation& annotation);

  /// Destructor
  virtual ~wxPdfAnnotation() {}

  /// Get the X offset of the annotation
  double GetX() const { return m_x; }

  /// Get the Y offset of the annotation
  double GetY() const { return m_y; }

  /// Get the text of the annotation
  wxString GetText() const { return m_text; }

private:
  double   m_x;     ///< X offset of the annotation
  double   m_y;     ///< Y offset of the annotation
  wxString m_text;  ///< Annotation text
};

/// Class representing bookmarks for defining the document's outline. (For internal use only)
class WXDLLIMPEXP_PDFDOC wxPdfBookmark
{
public:
  /// Constructor
  wxPdfBookmark(const wxString& txt, int level, double y, int page);

  /// Destructor
  virtual ~wxPdfBookmark();

  /// Get the bookmark text
  wxString GetText() { return m_text; }

  /// Get the associated level
  int GetLevel() { return m_level; }

  /// Get the Y offset of the bookmark
  double GetY() { return m_y; }

  /// Get the page number of the bookmark
  int GetPage() { return m_page; }

  /// Set the parent of the bookmark
  void SetParent(int parent) { m_parent = parent; }

  /// Get the parent of the bookmark
  int GetParent() { return m_parent; }

  /// Set previous bookmark
  void SetPrev(int prev) { m_prev = prev; }

  /// Get previous bookmark
  int GetPrev() { return m_prev; }

  /// Set next bookmark
  void SetNext(int next) { m_next = next; }

  /// Get next bookmark
  int GetNext() { return m_next; }

  /// Set first bookmark
  void SetFirst(int first) { m_first = first; }

  /// Get first bookmark
  int GetFirst() { return m_first; }

  /// Set last bookmark
  void SetLast(int last) { m_last = last; }

  /// Get last bookmark
  int GetLast() { return m_last; }

private:
  wxString m_text;    ///< Text of bookmark
  int      m_level;   ///< Associated level
  double   m_y;       ///< Y offset
  int      m_page;    ///< Page number
  int      m_parent;  ///< Parent bookmark
  int      m_prev;    ///< Previous bookmark
  int      m_next;    ///< Next bookmark
  int      m_first;   ///< First bookmark
  int      m_last;    ///< Last bookmark
};

/// Class representing wxPdfDocument colors.
class WXDLLIMPEXP_PDFDOC wxPdfColour
{
public:
  /// Constructor for grayscale color
  /**
  * Defines a grayscale color
  * \param grayscale indicates the gray level. Value between 0 and 255
  */
  wxPdfColour(const unsigned char grayscale = 0);
  
  /// Constructor for wxColour color
  /**
  * Defines a wxColour color.
  * \param color defines a wxColour color composed of a red, green and blue component
  */
  wxPdfColour(const wxColour& color);

  /// Constructor for RGB color
  /**
  * Defines a RGB color.
  * \param red indicates the red level. Value between 0 and 255
  * \param green indicates the green level. Value between 0 and 255
  * \param blue indicates the blue level. Value between 0 and 255
  */
  wxPdfColour(const unsigned char red, const unsigned char green, const unsigned char blue);
  
  /// Constructor for CMYK color
  /**
  * Defines a CMYK color.
  * \param cyan indicates the cyan level. Value between 0 and 100
  * \param magenta indicates the magenta level. Value between 0 and 100
  * \param yellow indicates the yellow level. Value between 0 and 100
  * \param black indicates the black level. Value between 0 and 100
  */
  wxPdfColour(double cyan, double magenta, double yellow, double black);
  
  /// Constructor for named RGB color
  /**
  * Defines a named RGB color.
  * \param name is the name of the requested color. Use of HTML notation <b><tt>\#rrggbb</tt></b> as color name is also supported.
  */
  wxPdfColour(const wxString& name);
  
  /// Copy constructor
  wxPdfColour(const wxPdfColour& color);

  
  /// Set grayscale color
  /**
  * \param grayscale indicates the gray level. Value between 0 and 255. Default: 0 (Black).
  */
  void SetColor(const unsigned char grayscale = 0);
  
  /// Set wxColour color
  /**
  * \param color defines a wxColour color composed of a red, green and blue component
  */
  void SetColor(const wxColour& color);
  
  /// Set RGB color
  /**
  * \param red indicates the red level. Value between 0 and 255
  * \param green indicates the green level. Value between 0 and 255
  * \param blue indicates the blue level. Value between 0 and 255
  */
  void SetColor(const unsigned char red, const unsigned char green, const unsigned char blue);
  
  /// Set CMYK color
  /**
  * \param cyan indicates the cyan level. Value between 0 and 100
  * \param magenta indicates the magenta level. Value between 0 and 100
  * \param yellow indicates the yellow level. Value between 0 and 100
  * \param black indicates the black level. Value between 0 and 100
  */
  void SetColor(double cyan, double magenta, double yellow, double black);

  /// Set a named RGB color
  /**
  * \param name is the name of the requested color
  */
  void SetColor(const wxString& name);

  /// Get internal color string representation (for internal use only)
  /**
  * \param drawing flag specifying whether the color is used for drawing operations 
  */
  const wxString GetColor(bool drawing) const;

protected:
  /// Constructor for internal color string representation
  wxPdfColour(const wxString& color, bool WXUNUSED(internal));

  /// Get a color database
  static wxColourDatabase* GetColorDatabase();

private:
  wxString m_color;  ///< internal color string

  static wxColourDatabase* ms_colorDatabase;
};

/// Class representing spot colors.
class WXDLLIMPEXP_PDFDOC wxPdfSpotColour
{
public:
  /// Constructor for spot color
  wxPdfSpotColour(int index, double cyan, double magenta, double yellow, double black);

  /// Copy constructor
  wxPdfSpotColour(const wxPdfSpotColour& color);

  /// Set object index
  void SetObjIndex(int index) { m_objIndex = index; }

  /// Get object index
  int  GetObjIndex() { return m_objIndex; }

  /// Get spot color index
  int    GetIndex() { return m_index; }

  /// Get cyan level
  double GetCyan() { return m_cyan; }

  /// Get magenta level
  double GetMagenta() { return m_magenta; }

  /// Get yellow level
  double GetYellow() { return m_yellow; }

  /// Get black level
  double GetBlack() { return m_black; }

private:
  int    m_objIndex;   ///< object index
  int    m_index;      ///< color index
  double m_cyan;       ///< cyan level
  double m_magenta;    ///< magenta level
  double m_yellow;     ///< yellow level
  double m_black;      ///< black level
};

/// Class representing double arrays (no standard class in wxWidgets unfortunately)
WX_DEFINE_USER_EXPORTED_ARRAY_DOUBLE(double, wxPdfArrayDouble, class WXDLLIMPEXP_PDFDOC);

/// Class representing line styles.
class WXDLLIMPEXP_PDFDOC wxPdfLineStyle
{
public:
  /// Constructor
  /**
  * Creates a line style for use in graphics primitives.
  * \param[in] width Width of the line in user units.
  * \param[in] cap   Type of cap to put on the line (butt, round, square).
  *                  The difference between 'square' and 'butt' is that 'square'
  *                  projects a flat end past the end of the line.
  * \param[in] join  form of line joining: miter, round or bevel
  * \param[in] dash  pattern for dashed lines.Is an empty array (without dash) or
  *   array with series of length values, which are the lengths of the on and off dashes.
  *           For example: (2) represents 2 on, 2 off, 2 on , 2 off ...
  *                        (2,1) is 2 on, 1 off, 2 on, 1 off.. etc
  * \param[in] phase Modifier of the dash pattern which is used to shift the point at which the pattern starts
  * \param[in] color line color.
  * \see SetLineStyle(), Curve(), Line(), Circle(), Ellipse(), Rect(), RoundedRect(), Polygon(), RegularPolygon(), StarPolygon()
  */
  wxPdfLineStyle(double width = -1,
                 wxPdfLineCap cap = wxPDF_LINECAP_NONE, wxPdfLineJoin join = wxPDF_LINEJOIN_NONE,
                 const wxPdfArrayDouble& dash = wxPdfArrayDouble(), double phase = -1,
                 const wxPdfColour& color = wxPdfColour());

  /// Copy constructor
  wxPdfLineStyle(const wxPdfLineStyle& lineStyle);

  /// Assignment operator
  wxPdfLineStyle& operator= (const wxPdfLineStyle& lineStyle);

  /// Destructor
  virtual ~wxPdfLineStyle();

  /// Check whether the style is initialized.
  bool IsSet() const { return m_isSet; }

  /// Set the line width
  void SetWidth(double width) { m_width = width; }

  /// Get the line width
  double GetWidth() const { return m_width; }

  /// Set the line ending style
  void SetLineCap(const wxPdfLineCap cap) { m_cap = cap; }

  /// Get the line ending style
  wxPdfLineCap GetLineCap() const { return m_cap; }

  /// Set the line join style
  void SetLineJoin(const wxPdfLineJoin join) { m_join = join; }

  /// Get the line join style
  wxPdfLineJoin GetLineJoin() const { return m_join; }

  /// Set the dash pattern
  void SetDash(const wxPdfArrayDouble& dash) { m_dash = dash; }

  /// Get the dash pattern
  const wxPdfArrayDouble& GetDash() const { return m_dash; }

  /// Set the dash pattern phase
  void SetPhase(double phase) { m_phase = phase; }

  /// Get the dash pattern phase
  double GetPhase() const { return m_phase; }

  /// Set the line color
  void SetColour(const wxPdfColour& color) { m_color = color; };

  /// Get the line color
  const wxPdfColour& GetColour() const { return m_color; };

private:
  bool             m_isSet;   ///< Flag whether the style is initialized
  double           m_width;   ///< Line width
  wxPdfLineCap     m_cap;     ///< Line ending style
  wxPdfLineJoin    m_join;    ///< Line joining style
  wxPdfArrayDouble m_dash;    ///< Dash pattern
  double           m_phase;   ///< Dash pattern phase
  wxPdfColour      m_color;   ///< Line color
};

/// Class representing gradients.
class WXDLLIMPEXP_PDFDOC wxPdfGradient
{
public:
  /// Constructor
  wxPdfGradient(int type, const wxColour& color1, const wxColour& color2, double coords[]);

  /// Destructor
  virtual ~wxPdfGradient();

  /// Set gradient object index
  void SetObjIndex(int n) { m_n = n; }

  /// Get gradient object index
  int  GetObjIndex() { return m_n; }

  /// Get the gradient type
  const int GetType() const { return m_type; };

  /// Get the gradient color 1
  const wxColour& GetColor1() const { return m_color1; };

  /// Get the gradient color 2
  const wxColour& GetColor2() const { return m_color2; };

  /// Get the gradient coordinates
  const double* GetCoords() const { return m_coords; }

private:
  int              m_n;         ///< Gradient index
  int              m_type;      ///< Gradient type
  wxColour         m_color1;    ///< Gradient color 1
  wxColour         m_color2;    ///< Gradient color 2
  double           m_coords[5]; ///< Gradient coordinates
};

class wxPdfCellContext;
class wxPdfTable;
class wxPdfIndirectObject;
class wxPdfAnnotationWidget;

/// Hashmap class for offset values
WX_DECLARE_HASH_MAP(long, int, wxIntegerHash, wxIntegerEqual, wxPdfOffsetHashMap);

/// Hashmap class for document pages
WX_DECLARE_HASH_MAP(long, wxMemoryOutputStream*, wxIntegerHash, wxIntegerEqual, wxPdfPageHashMap);

/// Hashmap class for boolean values
WX_DECLARE_HASH_MAP(long, bool, wxIntegerHash, wxIntegerEqual, wxPdfBoolHashMap);

/// Hashmap class for double values
WX_DECLARE_HASH_MAP(long, double, wxIntegerHash, wxIntegerEqual, wxPdfDoubleHashMap);

/// Hashmap class for document links
WX_DECLARE_HASH_MAP(long, wxPdfLink*, wxIntegerHash, wxIntegerEqual, wxPdfLinkHashMap);

/// Hashmap class for page links
WX_DECLARE_HASH_MAP(long, wxArrayPtrVoid*, wxIntegerHash, wxIntegerEqual, wxPdfPageLinksMap);

/// Hashmap class for document annotations
WX_DECLARE_HASH_MAP(long, wxArrayPtrVoid*, wxIntegerHash, wxIntegerEqual, wxPdfAnnotationsMap);

/// Hashmap class for document annotations
WX_DECLARE_HASH_MAP(long, wxArrayPtrVoid*, wxIntegerHash, wxIntegerEqual, wxPdfFormAnnotsMap);

/// Hashmap class for form fields
WX_DECLARE_HASH_MAP(long, wxPdfIndirectObject*, wxIntegerHash, wxIntegerEqual, wxPdfFormFieldsMap);

/// Hashmap class for font encoding differences
WX_DECLARE_HASH_MAP(long, wxString*, wxIntegerHash, wxIntegerEqual, wxPdfDiffHashMap);

/// Hashmap class for gradients
WX_DECLARE_HASH_MAP(long, wxPdfGradient*, wxIntegerHash, wxIntegerEqual, wxPdfGradientMap);

/// Hashmap class for core fonts
WX_DECLARE_STRING_HASH_MAP(int, wxPdfCoreFontMap);

/// Hash map class for used/embedded fonts
WX_DECLARE_STRING_HASH_MAP(wxPdfFont*, wxPdfFontHashMap);

/// Hash map class for embedded images
WX_DECLARE_STRING_HASH_MAP(wxPdfImage*, wxPdfImageHashMap);

/// Hash map class for spot colors
WX_DECLARE_STRING_HASH_MAP(wxPdfSpotColour*, wxPdfSpotColourMap);

/// Hash map class for spot colors
WX_DECLARE_STRING_HASH_MAP(wxPdfIndirectObject*, wxPdfRadioGroupMap);

/// Class representing a PDF document.
class WXDLLIMPEXP_PDFDOC wxPdfDocument
{
public:
  /// Constructor
  /**
  * \param orientation Defines the default page orientation. Possible values are:
  *   \li wxPORTRAIT portrait layout (default)
  *   \li wxLANDSCAPE landscape layout
  *
  * \param unit Defines the user units. Possible values are:
  *   \li "mm" millimeter (1 mm = 0.0394 in = 2.833 pt = 0.1 cm) (default)
  *   \li "cm" centimeter (1 cm = 0.394 in = 28.33 pt = 10 mm)
  *   \li "pt" points (1 pt = 1/72 in = 0.0353 cm = 0.353 mm)
  *   \li "in" inch   (1 in = 72 pt = 2.54 cm = 25.4 mm)
  * \param format Defines the page format. All known wxWidgets paper types are allowed. (Default: wxPAPER_A4)
  */
  wxPdfDocument(int orientation = wxPORTRAIT, 
                const wxString& unit = wxString(_T("mm")), 
                wxPaperSize format = wxPAPER_A4);
// TODO: Constructor with explicit papersize

  virtual ~wxPdfDocument();

  /// Set permissions as well as user and owner passwords.
  /**
  * \param[in] permissions flag indicating permissions.
  *                        Flags from the following list may be combined as needed
  *                        If a value is present it means that the permission is granted
  * \param[in] userPassword user password if applicable.
  *                         If a user password is set, user will be prompted before document is opened
  * \param[in] ownerPassword owner password.if applicable
  *                          If an owner password is set, the document can be opened
  *                          in privilege mode with no restriction if that password is entered
  *                           
  */
  virtual void SetProtection(int permissions,
                             const wxString& userPassword = wxEmptyString,
                             const wxString& ownerPassword = wxEmptyString);

  /// Set the image scale.
  /**
  * \param[in] scale image scale.
  */
  virtual void SetImageScale(double scale);

  /// Returns the image scale.
  /**
  * \return image scale.
  */
  virtual double GetImageScale();

  /// Returns the page width in units.
  /**
  * \return int page width.
  */
  virtual double GetPageWidth();
  
  /// Returns the page height in units.
  /**
  * \return int page height.
  */
  virtual double GetPageHeight();
  
  /// Returns the page break margin.
  /**
  * \return int page break margin.
  */
  virtual double GetBreakMargin();
  
  /// Returns the scale factor (number of points in user unit).
  /**
  * \return int scale factor.
  */
  virtual double GetScaleFactor();

  /// Defines the left, top and right margins.
  /**
  * By default, they equal 1 cm.
  * Call this method to change them.
  * \param left Left margin.
  * \param top Top margin.
  * \param right Right margin. Default value is the left one.
  * \see SetLeftMargin(), SetTopMargin(), SetRightMargin(), SetAutoPageBreak()
  */
  virtual void SetMargins(double left, double top, double right = -1);
  
  /// Defines the left margin.
  /**
  * The method can be called before creating the first page.
  * If the current abscissa gets out of page, it is brought back to the margin.
  * \param margin The margin.
  * \see SetTopMargin(), SetRightMargin(), SetAutoPageBreak(), SetMargins()
  */
  virtual void SetLeftMargin(double margin);
  
  /// Returns the left margin.
  /**
  * \return double left margin.
  */
  virtual double GetLeftMargin();

  /// Defines the top margin.
  /**
  * The method can be called before creating the first page.
  * \param margin The margin.
  * \see SetLeftMargin(), SetRightMargin(), SetAutoPageBreak(), SetMargins()
  */
  virtual void SetTopMargin(double margin);
  
  /// Returns the top margin.
  /**
  * \return double top margin.
  */
  virtual double GetTopMargin();

  /// Defines the right margin.
  /**
  * The method can be called before creating the first page.
  * \param margin The margin.
  * \see SetLeftMargin(), SetTopMargin(), SetAutoPageBreak(), SetMargins()
  */
  virtual void SetRightMargin(double margin);

  /// Returns the right margin.
  /**
  * \return double right margin.
  */
  virtual double GetRightMargin();

  /// Defines the cell margin.
  /**
  * The method can be called before creating the first page.
  * \param margin The margin.
  */
  virtual void SetCellMargin(double margin);

  /// Returns the cell margin.
  /**
  * \return double cell margin.
  */
  virtual double GetCellMargin();

  /// Sets the height of a text line
  /**
  * \param height The line height.
  */
  virtual void SetLineHeight(double height);

  /// Returns the height of a text line
  /**
  * \return double line height
  */
  virtual double GetLineHeight();

  /// Enables or disables the automatic page breaking mode.
  /**
  * When enabling, the second parameter
  * is the distance from the bottom of the page that defines the triggering limit.
  * By default, the mode is on and the margin is 2 cm.
  * \param auto Boolean indicating if mode should be on or off.
  * \param margin Distance from the bottom of the page.
  * \see Cell(), MultiCell(), AcceptPageBreak()
  */
  virtual void SetAutoPageBreak(bool auto, double margin = 0);

  /// Defines the way the document is to be displayed by the viewer.
  /**
  * The zoom level can be set:pages can be displayed entirely on screen, occupy the full width
  * of the window, use real size, be scaled by a specific zooming factor or use viewer default
  * (configured in the Preferences menu of Acrobat). The page layout can be specified too:
  * single at once, continuous display, two columns or viewer default. By default, documents
  * use the full width mode with continuous display.
  * \param zoom The zoom to use. It can be one of the following string values or a number
  * indicating the zooming factor to use. 
  *   \li wxPDF_ZOOM_FULLPAGE: displays the entire page on screen
  *   \li wxPDF_ZOOM_FULLWIDTH: uses maximum width of window
  *   \li wxPDF_ZOOM_REAL: uses real size (equivalent to 100% zoom)
  *   \li wxPDF_ZOOM_DEFAULT: uses viewer default mode
  *   \li wxPDF_ZOOM_FACTOR: uses viewer default mode
  * \param layout The page layout. Possible values are:
  *   \li wxPDF_LAYOUT_SINGLE: displays one page at once
  *   \li wxPDF_LAYOUT_CONTINUOUS: displays pages continuously (default)
  *   \li wxPDF_LAYOUT_TWO: displays two pages on two columns
  *   \li wxPDF_LAYOUT_DEFAULT: uses viewer default mode
  * \param zoomFactor specifies the zoom factor in percent if layout is wxPDF_ZOOM_FACTOR
  */
  virtual void SetDisplayMode(wxPdfZoom zoom,
                              wxPdfLayout layout = wxPDF_LAYOUT_CONTINUOUS,
                              double zoomFactor = 100.);

  /// Activates or deactivates page compression.
  /**
  * When activated, the internal representation of each
  * page is compressed, which leads to a compression ratio of about 2 for the resulting document.
  * Compression is on by default.
  * \param compress Boolean indicating if compression must be enabled.
  */
  virtual void SetCompression(bool compress);

  /// Defines the title of the document.
  /**
  * \param title The title.
  * \see SetAuthor(), SetCreator(), SetKeywords(), SetSubject()
  */
  virtual void SetTitle(const wxString& title);
  
  /// Defines the subject of the document.
  /**
  * \param subject The subject.
  * \see SetAuthor(), SetCreator(), SetKeywords(), SetTitle()
  */
  virtual void SetSubject(const wxString& subject);
  
  /// Defines the author of the document.
  /**
  * \param author The name of the author.
  * \see SetCreator(), SetKeywords(), SetSubject(), SetTitle()
  */
  virtual void SetAuthor(const wxString& author);

  /// Associates keywords with the document,
  /**
  * Generally keywords are in the form 'keyword1 keyword2 ...'.
  * \param keywords The list of keywords.
  * \see SetAuthor(), SetCreator(), SetSubject(), SetTitle()
  */
  virtual void SetKeywords(const wxString& keywords);
  
  /// Defines the creator of the document.
  /**
  * This is typically the name of the application that generates the PDF.
  * \param creator The name of the creator.
  * \see SetAuthor(), SetKeywords(), SetSubject(), SetTitle()
  */
  virtual void SetCreator(const wxString& creator);
  
  /// Defines an alias for the total number of pages.
  /**
  * It will be substituted as the document is closed.
  * \param alias The alias. Default value: {nb}.
  * \see PageNo(), Footer()
  */
  virtual void AliasNbPages(const wxString& alias = wxString(_T("{nb}")));

  /// This method begins the generation of the PDF document.
  /**
  * It is not necessary to call it explicitly
  * because AddPage() does it automatically.
  * Note: no page is created by this method
  * \see AddPage(), Close()
  */
  virtual void Open();
  
  /// Terminates the PDF document.
  /**
  * It is not necessary to call this method explicitly because SaveAsFile()
  * does it automatically. If the document contains no page, AddPage() is called to prevent from getting
  * an invalid document.
  * \see Open(), SaveAsFile()
  */
  virtual void Close();
  
  /// Adds a new page to the document.
  /**
  * If a page is already present, the Footer() method is called first
  * to output the footer. Then the page is added, the current position set to the top-left corner according
  * to the left and top margins, and Header() is called to display the header.
  * The font which was set before calling is automatically restored. There is no need to call SetFont()
  * again if you want to continue with the same font. The same is true for colors and line width.
  * The origin of the coordinate system is at the top-left corner and increasing ordinates go downwards.
  * \param orientation Page orientation. Possible values are:
  *   \li wxPORTRAIT
  *   \li wxLANDSCAPE
  * The default value is the one passed to the constructor.
  * \see FPDF(), Header(), Footer(), SetMargins()
  */
  virtual void AddPage(int orientation = -1);

  /// This method is used to render the page header.
  /**
  * It is automatically called by AddPage() and should not be called directly by the application. 
  * The implementation in wxPdfDocument is empty, so you have to subclass it and override the method
  * if you want a specific processing.
  * \see Footer()
  */
  virtual void Header();
  
  /// This method is used to render the page footer.
  /**
  * It is automatically called by AddPage() and Close() and should not be called directly by
  * the application. The implementation in wxPdfDocument is empty, so you have to subclass it
  * and override the method if you want a specific processing.
  * \see Header()
  */
  virtual void Footer();
  
  /// Returns whether footer output is in progress
  /**
  * \return true if footer output is in progress, false otherwise
  * \see Header()
  */
  virtual bool IsInFooter();
  
  /// Returns the current page number.
  /**
  * \return page number
  * \see AliasNbPages()
  */
  virtual int  PageNo();
  
  /// Add spot color
  /**
  * Add a spot color which can be referenced in color setting methods
  * \param name the name of the spot color (case sensitive)
  * \param cyan indicates the cyan level. Value between 0 and 100
  * \param magenta indicates the magenta level. Value between 0 and 100
  * \param yellow indicates the yellow level. Value between 0 and 100
  * \param black indicates the black level. Value between 0 and 100
  * \see SetDrawColor(), SetFillColor(), SetTextColor(), Line(), Rect(), Cell(), MultiCell()
  */
  virtual void AddSpotColor(const wxString& name, double cyan, double magenta, double yellow, double black);

  /// Defines the color used for all drawing operations.
  /**
  * Affected drawing operations are: lines, rectangles and cell borders. It can be expressed in RGB
  * components or gray scale. The method can be called before the first page is created and the value
  * is retained from page to page.
  * \param grayscale indicates the gray level. Value between 0 and 255
  * \see SetFillColor(), SetTextColor(), Line(), Rect(), Cell(), MultiCell()
  */
  virtual void SetDrawColor(const unsigned char grayscale);
  
  /// Defines the color used for all drawing operations.
  /**
  * Affected drawing operations are: lines, rectangles and cell borders. It can be expressed in RGB
  * components or gray scale. The method can be called before the first page is created and the value
  * is retained from page to page.
  * \param color defines a color composed of a red, green and blue component
  * \see SetFillColor(), SetTextColor(), Line(), Rect(), Cell(), MultiCell()
  */
  virtual void SetDrawColor(const wxColour& color);
  
  /// Defines the color used for all drawing operations.
  /**
  * Affected drawing operations are: lines, rectangles and cell borders. It can be expressed in RGB
  * components or gray scale. The method can be called before the first page is created and the value
  * is retained from page to page.
  * \param color defines a color using the class wxPdfColour
  * \see SetFillColor(), SetTextColor(), Line(), Rect(), Cell(), MultiCell()
  */
  virtual void SetDrawColor(const wxPdfColour& color);
  
  /// Defines the color used for all drawing operations.
  /**
  * Affected drawing operations are: lines, rectangles and cell borders. It can be expressed in RGB
  * components or gray scale. The method can be called before the first page is created and the value
  * is retained from page to page.
  * \param red indicates the red level. Value between 0 and 255
  * \param green indicates the green level. Value between 0 and 255
  * \param blue indicates the blue level. Value between 0 and 255
  * \see SetFillColor(), SetTextColor(), Line(), Rect(), Cell(), MultiCell()
  */
  virtual void SetDrawColor(const unsigned char red, const unsigned char green, const unsigned char blue);

  /// Defines the color used for all drawing operations.
  /**
  * Affected drawing operations are: lines, rectangles and cell borders. It can be expressed in RGB
  * components or gray scale. The method can be called before the first page is created and the value
  * is retained from page to page.
  * \param cyan indicates the cyan level. Value between 0 and 100
  * \param magenta indicates the magenta level. Value between 0 and 100
  * \param yellow indicates the yellow level. Value between 0 and 100
  * \param black indicates the black level. Value between 0 and 100
  * \see SetFillColor(), SetTextColor(), Line(), Rect(), Cell(), MultiCell()
  */
  virtual void SetDrawColor(double cyan, double magenta, double yellow, double black);

  /// Defines the <b>spot color</b> used for all drawing operations.
  /**
  * Affected drawing operations are: lines, rectangles and cell borders. It can be expressed in RGB
  * components or gray scale. The method can be called before the first page is created and the value
  * is retained from page to page.
  * \param name the name of the spot color
  * \param tint indicates the tint level. Value between 0 and 100. Default: 100.
  * \see SetFillColor(), SetTextColor(), Line(), Rect(), Cell(), MultiCell()
  */
  virtual void SetDrawColor(const wxString& name, double tint = 100);

  /// Gets the color used for all drawing operations.
  /**
  * \see SetDrawColor()
  */
  virtual const wxPdfColour GetDrawColor();
  
  /// Defines the color used for all filling operations.
  /**
  * Affected filling operations are: filled rectangles and cell backgrounds. It can be expressed in RGB
  * components or gray scale. The method can be called before the first page is created and the value is
  * retained from page to page.
  * \param grayscale indicates the gray level. Value between 0 and 255
  * \see SetDrawColor(), SetTextColor(), Rect(), Cell(), MultiCell()
  */
  virtual void SetFillColor(const unsigned char grayscale);
  
  /// Defines the color used for all filling operations.
  /**
  * Affected filling operations are: filled rectangles and cell backgrounds. It can be expressed in RGB
  * components or gray scale. The method can be called before the first page is created and the value is
  * retained from page to page.
  * \param color defines a color composed of a red, green and blue component
  * \see SetDrawColor(), SetTextColor(), Rect(), Cell(), MultiCell()
  */
  virtual void SetFillColor(const wxColour& color);
  
  /// Defines the color used for all filling operations.
  /**
  * Affected filling operations are: filled rectangles and cell backgrounds. It can be expressed in RGB
  * components or gray scale. The method can be called before the first page is created and the value is
  * retained from page to page.
  * \param color defines a color using the class wxPdfColour
  * \see SetDrawColor(), SetTextColor(), Rect(), Cell(), MultiCell()
  */
  virtual void SetFillColor(const wxPdfColour& color);
  
  /// Defines the color used for all filling operations.
  /**
  * Affected filling operations are: filled rectangles and cell backgrounds. It can be expressed in RGB
  * components or gray scale. The method can be called before the first page is created and the value is
  * retained from page to page.
  * \param red indicates the red level. Value between 0 and 255
  * \param green indicates the green level. Value between 0 and 255
  * \param blue indicates the blue level. Value between 0 and 255
  * \see SetDrawColor(), SetTextColor(), Rect(), Cell(), MultiCell()
  */
  virtual void SetFillColor(const unsigned char red, const unsigned char green, const unsigned char blue);

  /// Defines the color used for all filling operations.
  /**
  * Affected filling operations are: filled rectangles and cell backgrounds. It can be expressed in RGB
  * components or gray scale. The method can be called before the first page is created and the value is
  * retained from page to page.
  * \param cyan indicates the cyan level. Value between 0 and 100
  * \param magenta indicates the magenta level. Value between 0 and 100
  * \param yellow indicates the yellow level. Value between 0 and 100
  * \param black indicates the black level. Value between 0 and 100
  * \see SetDrawColor(), SetTextColor(), Rect(), Cell(), MultiCell()
  */
  virtual void SetFillColor(double cyan, double magenta, double yellow, double black);

  /// Defines the <b>spot color</b> used for all filling operations.
  /**
  * Affected filling operations are: filled rectangles and cell backgrounds. It can be expressed in RGB
  * components or gray scale. The method can be called before the first page is created and the value is
  * retained from page to page.
  * \param name is the name of the spot color
  * \param tint indicates the tint level. Value between 0 and 100. Default: 100.
  * \see SetDrawColor(), SetTextColor(), Rect(), Cell(), MultiCell()
  */
  virtual void SetFillColor(const wxString& name, double tint = 100);

  /// Gets the color used for all filling operations.
  /**
  * \see SetFillColor()
  */
  virtual const wxPdfColour GetFillColor();
  
  /// Defines the color used for text.
  /**
  * It can be expressed in RGB components or gray scale. The method can be called before the first page is
  * created and the value is retained from page to page.
  * \param grayscale indicates the gray level. Value between 0 and 255
  * \see SetDrawColor(), SetFillColor(), Text(), Cell(), MultiCell()
  */
  virtual void SetTextColor(const unsigned char grayscale);
  
  /// Defines the color used for text.
  /**
  * It can be expressed in RGB components or gray scale. The method can be called before the first page is
  * created and the value is retained from page to page.
  * \param color defines a color composed of a red, green and blue component
  * \see SetDrawColor(), SetFillColor(), Text(), Cell(), MultiCell()
  */
  virtual void SetTextColor(const wxColour& color);
  
  /// Defines the color used for text.
  /**
  * It can be expressed in RGB components or gray scale. The method can be called before the first page is
  * created and the value is retained from page to page.
  * \param color defines a color using the class wxPdfColour
  * \see SetDrawColor(), SetFillColor(), Text(), Cell(), MultiCell()
  */
  virtual void SetTextColor(const wxPdfColour& color);
  
  /// Defines the color used for text.
  /**
  * It can be expressed in RGB components or gray scale. The method can be called before the first page is
  * created and the value is retained from page to page.
  * \param red indicates the red level. Value between 0 and 255
  * \param green indicates the green level. Value between 0 and 255
  * \param blue indicates the blue level. Value between 0 and 255
  * \see SetDrawColor(), SetFillColor(), Text(), Cell(), MultiCell()
  */
  virtual void SetTextColor(const unsigned char red, const unsigned char green, const unsigned char blue);

  /// Defines the color used for text.
  /**
  * It can be expressed in RGB components or gray scale. The method can be called before the first page is
  * created and the value is retained from page to page.
  * \param cyan indicates the cyan level. Value between 0 and 100
  * \param magenta indicates the magenta level. Value between 0 and 100
  * \param yellow indicates the yellow level. Value between 0 and 100
  * \param black indicates the black level. Value between 0 and 100
  * \see SetDrawColor(), SetFillColor(), Text(), Cell(), MultiCell()
  */
  virtual void SetTextColor(double cyan, double magenta, double yellow, double black);

  /// Defines the <b>spot color</b> used for text.
  /**
  * It can be expressed in RGB components or gray scale. The method can be called before the first page is
  * created and the value is retained from page to page.
  * \param name the name of the spot color
  * \param tint indicates the tint level. Value between 0 and 100. Default: 100.
  * \see SetDrawColor(), SetFillColor(), Text(), Cell(), MultiCell()
  */
  virtual void SetTextColor(const wxString& name, double tint = 100);

  /// Gets the color used for text output.
  /**
  * \see SetTextColor()
  */
  virtual const wxPdfColour GetTextColor();
  
  /// Returns the length of a string in user unit.
  /**
  * A font must be selected.
  * \param s The string whose length is to be computed
  * \return int
  */
  virtual double GetStringWidth(const wxString& s);

  /// Defines the line width.
  /**
  * By default, the value equals 0.2 mm. The method can be called before the first page is created
  * and the value is retained from page to page.
  * \param width The width.
  * \see Line(), Rect(), Cell(), MultiCell()
  */
  virtual void SetLineWidth(double width);

  /// Gets the current line width.
  /**
  * \return current line width
  * \see Line(), Rect(), Cell(), MultiCell()
  */
  virtual double GetLineWidth();

  /// Sets line style
  /**
  * \param linestyle: Line style. \see wxPdfLineStale
  */
  virtual void SetLineStyle(const wxPdfLineStyle& linestyle);

  /// Get current line style
  /**
  * \return current line style.
  */
  virtual const wxPdfLineStyle& GetLineStyle();

  /// Draws a line between two points.
  /**
  * \param x1 Abscissa of first point
  * \param y1 Ordinate of first point
  * \param x2 Abscissa of second point
  * \param y2 Ordinate of second point
  * \see SetLineWidth(), SetDrawColor()
  */
  virtual void Line(double x1, double y1, double x2, double y2);
  
  /// Draws an arrow line between two points.
  /**
  * \param x1 Abscissa of first point
  * \param y1 Ordinate of first point
  * \param x2 Abscissa of second point
  * \param y2 Ordinate of second point
  * \param linewidth line width
  * \param height height of the arrow head
  * \param width width of the arrow head
  * \see SetLineWidth(), SetDrawColor(), SetFillColor()
  */
  virtual void Arrow(double x1, double y1, double x2, double y2, double linewidth, double height, double width);

  /// Outputs a rectangle.
  /**
  * It can be drawn (border only), filled (with no border) or both.
  * \param x Abscissa of upper-left corner
  * \param y Ordinate of upper-left corner
  * \param w Width
  * \param h Height
  * \param style Style of rendering. Possible values are:
  *   \li wxPDF_STYLE_DRAW (default)
  *   \li wxPDF_STYLE_FILL: fill
  *   \li wxPDF_STYLE_FILLDRAW: draw and fill
  * \see SetLineWidth(), SetDrawColor(), SetFillColor()
  */
  virtual void Rect(double x, double y, double w, double h, int style = wxPDF_STYLE_DRAW);

  /// Draws a rounded rectangle
  /**
  * \param x Abscissa of upper-left corner
  * \param y Ordinate of upper-left corner
  * \param w Width
  * \param h Height
  * \param r: Radius of the rounded corners
  * \param roundCorner: Draws rounded corner or not.
  *   \li wxPDF_CORNER_NONE          no rounded corners
  *   \li wxPDF_CORNER_TOP_LEFT      top left corner
  *   \li wxPDF_CORNER_TOP_RIGHT     top right corner
  *   \li wxPDF_CORNER_BOTTOM_LEFT   bottom left corner
  *   \li wxPDF_CORNER_BOTTOM_RIGHT  bottom right corner
  *   \li wxPDF_CORNER_ALL           all corners
  * \param style: Style of rectangle (draw and/or fill)
  */
  virtual void RoundedRect(double x, double y, double w, double h,
                           double r, int roundCorner = wxPDF_CORNER_ALL, int style = wxPDF_STYLE_DRAW);

  /// Draws a Bézier curve
  /**
  * A Bézier curve is tangent to the line between the control points at either end of the curve.
  * \param x0: Abscissa of start point
  * \param y0: Ordinate of start point
  * \param x1: Abscissa of control point 1
  * \param y1: Ordinate of control point 1
  * \param x2: Abscissa of control point 2
  * \param y2: Ordinate of control point 2
  * \param x3: Abscissa of end point
  * \param y3: Ordinate of end point
  * \param style: Style of rectangle (draw and/or fill)
  */
  virtual void Curve(double x0, double y0, double x1, double y1,
                     double x2, double y2, double x3, double y3,
                     int style = wxPDF_STYLE_DRAW);

  /// Draws an ellipse
  /**
  * \param x0: Abscissa of Center point
  * \param y0: Ordinate of Center point
  * \param rx: Horizontal radius
  * \param ry: Vertical radius (if ry = 0, draws a circle)
  * \param angle: Orientation angle (anti-clockwise)
  * \param astart: Start angle
  * \param afinish: Finish angle
  * \param style: Style of rectangle (draw and/or fill)
  * \param nSeg: Ellipse is made up of nSeg Bézier curves
  */
  virtual void Ellipse(double x0, double y0, double rx, double ry = 0, 
                       double angle = 0, double astart = 0, double afinish = 360,
                       int style = wxPDF_STYLE_DRAW, int nSeg = 8);

  /// Draws a circle
  /**
  * \param x0: Abscissa of Center point
  * \param y0: Ordinate of Center point
  * \param r: Radius
  * \param astart: Start angle
  * \param afinish: Finish angle
  * \param style: Style of rectangle (draw and/or fill)
  * \param nSeg: Circle is made up of nSeg Bézier curves
  */
  virtual void Circle(double x0, double y0, double r, 
                      double astart = 0, double afinish = 360,
                      int style = wxPDF_STYLE_DRAW, int nSeg = 8);

  /// Draws a sector
  /**
  * \param x0: Abscissa of Center point
  * \param y0: Ordinate of Center point
  * \param r: Radius
  * \param astart: Start angle
  * \param afinish: Finish angle
  * \param style: Style of rectangle (draw and/or fill, default: fill&draw)
  * \param clockwise: indicates whether to go clockwise (default: true)
  * \param origin: origin of angles (0 for 3 o'clock, 90 for noon, 180 for 9 o'clock, 270 for 6 o'clock; default: 90)
  */
  virtual void Sector(double x0, double y0, double r, double astart, double afinish,
                      int style = wxPDF_STYLE_FILLDRAW, bool clockwise = true, double origin = 90.);

  /// Draws a polygon
  /**
  * \param x Array with abscissa values
  * \param y Array with ordinate values
  * \param style: Style of polygon (draw and/or fill)
  */
  virtual void Polygon(const wxPdfArrayDouble& x, const wxPdfArrayDouble& y,
                       int style = wxPDF_STYLE_DRAW);

   /// Draws a regular polygon
  /**
  * \param x0: Abscissa of Center point
  * \param y0: Ordinate of Center point
  * \param r: Radius of circumscribed circle
  * \param ns: Number of sides
  * \param angle: Orientation angle (anti-clockwise)
  * \param circle: Flag whether to draw circumscribed circle or not
  * \param style: Style of polygon (draw and/or fill)
  * \param circleStyle: Style of circumscribed circle (draw and/or fill) (if draw)
  * \param circleLineStyle: Line style for circumscribed circle. (if draw)
  * \param circleFillColor: Fill color for circumscribed circle. (if draw fill circle)
  */
  virtual void RegularPolygon(double x0, double y0, double r, int ns, double angle = 0, bool circle = false,
                              int style = wxPDF_STYLE_DRAW, 
                              int circleStyle = wxPDF_STYLE_DRAW,
                              const wxPdfLineStyle& circleLineStyle = wxPdfLineStyle(),
                              const wxPdfColour& circleFillColor = wxPdfColour());

  /// Draws a star polygon
  /**
  * \param x0: Abscissa of Center point
  * \param y0: Ordinate of Center point
  * \param r: Radius of circumscribed circle
  * \param nv: Number of vertices
  * \param ng: Number of gaps (ng % nv = 1 => regular polygon)
  * \param angle: Orientation angle (anti-clockwise)
  * \param circle: Flag whether to draw circumscribed circle or not
  * \param style: Style of polygon (draw and/or fill)
  * \param circleStyle: Style of circumscribed circle (draw and/or fill) (if draw)
  * \param circleLineStyle: Line style for circumscribed circle. (if draw)
  * \param circleFillColor: Fill color for circumscribed circle. (if draw fill circle)
  */
  virtual void StarPolygon(double x0, double y0, double r, int nv, int ng, double angle = 0, bool circle = false,
                           int style = wxPDF_STYLE_DRAW, 
                           int circleStyle = wxPDF_STYLE_DRAW,
                           const wxPdfLineStyle& circleLineStyle = wxPdfLineStyle(),
                           const wxPdfColour& circleFillColor = wxPdfColour());

  /// Performs a rotation around a given center.
  /**
  * \param angle angle in degrees.
  * \param x abscissa of the rotation center. Default value: current position.
  * \param y ordinate of the rotation center. Default value: current position.
  *
  * The rotation affects all elements which are printed after the method call
  * (with the exception of the clickable areas).
  *
  * Remarks:
  * \li Only the display is altered. The GetX() and GetY() methods are not affected,
  *  nor the automatic page break mechanism.
  * \li Rotation is not kept from page to page. Each page begins with a null rotation. 
  */
  virtual void Rotate(double angle, double x = -1, double y = -1);

  /// Imports a TrueType, TrueTypeUnicode or Type1 font and makes it available.
  /**
  * It is necessary to generate a font definition file first with the makefont utility.
  * The definition file (and the font file itself when embedding) must be present either
  * in the current directory or in the one indicated by WXPDF_FONTPATH if the constant is
  * defined.
  * \param family Font family. The name can be chosen arbitrarily. If it is a standard family name,
  * it will override the corresponding font.
  * \param style Font style. Possible values are (case insensitive):
  *   \li empty string: regular (default)
  *   \li B: bold
  *   \li I: italic
  *   \li BI or IB: bold italic
  * \param file The font definition file. By default, the name is built from the family and style,
  *  in lower case with no space.
  * \see SetFont()
  */
  virtual bool AddFont(const wxString& family, 
                       const wxString& style = wxEmptyString, 
                       const wxString& file = wxEmptyString);

  /// Imports a CJK (Chinese, Japanese or Korean) font and makes it available.
  /**
  * It is necessary to generate a font definition file first with the makefont utility.
  * The definition file (and the font file itself when embedding) must be present either
  * in the current directory or in the one indicated by WXPDF_FONTPATH if the constant is
  * defined.
  * \param family Font family. The name can be chosen arbitrarily. If it is a standard family name,
  * it will override the corresponding font.
  *
  * All font styles (regular, bold, italic and bold-italic) are made available
  * The font definition file name is built from the family in lower case with no space.
  * There are several predefined font definition files available:
  * \li BIG5 Chinese (traditional)
  * \li BIG5-HW  Chinese (traditional) half-width ASCII characters
  * \li GB Chinese (simplified)
  * \li GB-HW Chinese (simplified) half-width ASCII characters
  * \li SJIS Japanese
  * \li SJIS-HW Japanese, half-width ASCII characters
  * \li UHC Korean
  * \li UHC-HW Korean, half-width ASCII characters
  *
  * These fonts require that the Adobe CJK font support is installed
  * \see SetFont()
  */
#if wxUSE_UNICODE
  virtual bool AddFontCJK(const wxString& family);
#else
  virtual bool AddFontCJK(const wxString& WXUNUSED(family)) { return false; }
#endif
  
  /// Sets the font used to print character strings. 
  /**
  * It is mandatory to call this method at least once before printing text or the
  * resulting document would not be valid.
  * The font can be either a standard one or a font added via the AddFont() method.
  * Standard fonts use Windows encoding cp1252 (Western Europe).
  * The method can be called before the first page is created and the font is retained from page to page.
  * If you just wish to change the current font size, it is simpler to call SetFontSize().
  *
  * \param family Family font. It can be either a name defined by AddFont() or one of the standard
  * families (case insensitive):
  *  \li Courier (fixed-width)
  *  \li Helvetica or Arial (synonymous; sans serif)
  *  \li Times (serif)
  *  \li Symbol (symbolic)
  *  \li ZapfDingbats (symbolic)
  *
  * It is also possible to pass an empty string. In that case, the current family is retained.
  * \param style Font style. Possible values are (case insensitive):
  *   \li empty string: regular (default)
  *   \li B: bold
  *   \li I: italic
  *   \li BI or IB: bold italic
  *   \li U: underline
  * or any combination. The default value is regular. Bold and italic styles do not apply to Symbol and ZapfDingbats
  * \param size Font size in points. The default value is the current size. If no size has been
  * specified since the beginning of the document, the value taken is 12
  * \see AddFont(), SetFontSize(), Cell(), MultiCell(), Write()
  */
  virtual bool SetFont(const wxString& family,
                       const wxString& style = wxEmptyString,
                       double size = 0);
  
  /// Defines the size of the current font.
  /**
  * \param size The size (in points)
  * \see SetFont()
  */
  virtual void SetFontSize(double size);
  
  /// Gets the font family of the current font.
  /**
  * \return The font family of the current font
  * \see SetFont()
  */
  virtual const wxString GetFontFamily();

  /// Gets the style of the current font.
  /**
  * \return The style of the current font
  * \see SetFont()
  */
  virtual const wxString GetFontStyle();
  
  /// Gets the size of the current font.
  /**
  * \return The size (in points) of the current font
  * \see SetFont()
  */
  virtual double GetFontSize();

  /// Creates a new internal link and returns its identifier.
  /**
  * An internal link is a clickable area which directs to another place within the document.
  * The identifier can then be passed to Cell(), Write(), Image() or Link().
  * The destination is defined with SetLink().
  * \see Cell(), Write(), Image(), Link(), SetLink()
  */
  virtual int AddLink();
  
  /// Defines the page and position a link points to.
  /**
  * \param link The link identifier returned by AddLink()
  * \param y Ordinate of target position; -1 indicates the current position. The default value is 0 (top of page)
  * \param page Number of target page; -1 indicates the current page. This is the default value
  * \see AddLink()
  */
  virtual bool SetLink(int link, double y = 0., int page = -1);
  
  /// Puts a link on a rectangular area of the page.
  /**
  * Text or image links are generally put via Cell(), Write() or Image(), but this method can be useful
  * for instance to define a clickable area inside an image.
  * \param x Abscissa of the upper-left corner of the rectangle
  * \param y Ordinate of the upper-left corner of the rectangle
  * \param w Width of the rectangle
  * \param h Height of the rectangle
  * \param link URL or identifier returned by AddLink()
  * \see AddLink(), Cell(), Write(), Image()
  */
  virtual void Link(double x, double y, double w, double h, const wxPdfLink& link);

  /// Adds a bookmark to the document outline
  /**
  * \param txt: the bookmark title.
  * \param level: the bookmark level (0 is top level, 1 is just below, and so on).
  * \param y: the y position of the bookmark destination in the current page.
  *   -1 means the current position. Default value: 0.
  */
  virtual void Bookmark(const wxString& txt, int  level = 0, double y = 0);

  /// Prints a character string.
  /**
  * The origin is on the left of the first charcter, on the baseline.
  * This method allows to place a string precisely on the page, but it is usually easier to use Cell(),
  * MultiCell() or Write() which are the standard methods to print text.
  * \param x Abscissa of the origin
  * \param y Ordinate of the origin
  * \param txt String to print
  * \see SetFont(), SetTextColor(), Cell(), MultiCell(), Write()
  */
  virtual void Text(double x, double y, const wxString& txt);

  /// Prints a rotated text string
  /**
  * \param x: abscissa of the rotation center.
  * \param y: ordinate of the rotation center.
  * \param txt String to print
  * \param angle: angle in degrees.
  */
  virtual void RotatedText(double x, double y, const wxString& txt, double angle);

  /// Whenever a page break condition is met,
  /**
  * Whenever a page break condition is met, the method is called, and the break is issued or not
  * depending on the returned value. The default implementation returns a value according to the
  * mode selected by SetAutoPageBreak()
  *
  * This method is called automatically and should not be called directly by the application.
  * \return boolean
  * \see SetAutoPageBreak()
  */
  virtual bool AcceptPageBreak();
  
  /// Prints a cell (rectangular area) with optional borders, background color and character string.
  /**
  * The upper-left corner of the cell corresponds to the current position. The text can be aligned
  * or centered. After the call, the current position moves to the right or to the next line.
  * It is possible to put a link on the text.
  * If automatic page breaking is enabled and the cell goes beyond the limit, a page break is done
  * before outputting.
  * \param w Cell width. If 0, the cell extends up to the right margin.
  * \param h Cell height. Default value: 0.
  * \param txt String to print. Default value: empty string.
  * \param border Indicates if borders must be drawn around the cell. The value can be 
  *   \li wxPDF_BORDER_NONE no border
  *   \li wxPDF_BORDER_LEFT left border
  *   \li wxPDF_BORDER_RIGHT right border
  *   \li wxPDF_BORDER_TOP top border
  *   \li wxPDF_BORDER_BOTTOM bottom border
  *   \li wxPDF_BORDER_FRAME border on all sides
  * 
  * or a combination of them.
  * \param ln Indicates where the current position should go after the call. Possible values are:
  *   \li 0: to the right
  *   \li 1: to the beginning of the next line
  *   \li 2: below
  *
  * Putting 1 is equivalent to putting 0 and calling Ln() just after. Default value: 0.
  * \param align Allows to center or align the text. Possible values are:<ul><li>L or empty string: left align (default value)</li><li>C: center</li><li>R: right align</li></ul>
  *   \li wxPDF_ALIGN_LEFT align the text at the left margin
  *   \li wxPDF_ALIGN_CENTER center the text
  *   \li wxPDF_ALIGN_RIGHT align the text at the right margin
  *   \li wxPDF_ALIGN_JUSTIFY justify the text
  *
  * \param fill Indicates if the cell background must be painted (1) or transparent (0). Default value: 0.
  * \param link URL or identifier returned by AddLink().
  * \see SetFont(), SetDrawColor(), SetFillColor(), SetTextColor(), SetLineWidth(), AddLink(), Ln(), MultiCell(), Write(), SetAutoPageBreak()
  */
  virtual void Cell(double w, double h = 0., const wxString& txt = wxEmptyString,
                    int border = wxPDF_BORDER_NONE, int ln = 0, 
                    int align = wxPDF_ALIGN_LEFT, int fill = 0, 
                    const wxPdfLink& link = wxPdfLink(-1));

  /// This method allows printing text with line breaks.
  /**
  * They can be automatic (as soon as the text reaches the right border of the cell) or explicit
  * (via the \n character). As many cells as necessary are output, one below the other.
  * Text can be aligned, centered or justified. The cell block can be framed and the background painted.
  * \param w Width of cells. If 0, they extend up to the right margin of the page.
  * \param h Height of cells.
  * \param txt String to print
  * \param border Indicates if borders must be drawn around the cell. The value can be 
  *   \li wxPDF_BORDER_NONE no border
  *   \li wxPDF_BORDER_LEFT left border
  *   \li wxPDF_BORDER_RIGHT right border
  *   \li wxPDF_BORDER_TOP top border
  *   \li wxPDF_BORDER_BOTTOM bottom border
  *   \li wxPDF_BORDER_FRAME border on all sides
  * 
  * or a combination of them.
  * \param align Allows to center or align the text. Possible values are:
  *   \li wxPDF_ALIGN_LEFT align the text at the left margin
  *   \li wxPDF_ALIGN_CENTER center the text
  *   \li wxPDF_ALIGN_RIGHT align the text at the right margin
  *   \li wxPDF_ALIGN_JUSTIFY justify the text (default)
  *
  * \param fill Indicates if the cell background must be painted (1) or transparent (0). Default value: 0.
  * \param maxline Defines the maximum number of lines which should be printed.
  *        If maxline is 0 then the number of lines is not restricted. Default value: 0.
  * \return position in text string txt where output ended due to reaching the maximum number of lines
  * \see SetFont(), SetDrawColor(), SetFillColor(), SetTextColor(), SetLineWidth(), Cell(), Write(), SetAutoPageBreak()
  */
  virtual int MultiCell(double w, double h, const wxString& txt,
                        int border = 0, int align = wxPDF_ALIGN_JUSTIFY,
                        int fill = 0, int maxline = 0);

  /// This method counts the number of lines a text would occupy in respect to a given maximal width
  /**
  * \param w Width of cells. If 0, they extend up to the right margin of the page.
  * \param txt String for which the number of lines is to be counted
  * \return Number of lines this text would occupy
  */
  virtual int LineCount(double w, const wxString& txt);

  /// This method counts the number of lines a text will occupy in respect to a given maximal width
  /**
  * \param w Width of cells. If 0, they extend up to the right margin of the page.
  * \param h Height of cells.
  * \param txt String to print
  * \param halign Allows to center or align the text. Possible values are:
  *   \li wxPDF_ALIGN_LEFT align the text at the left margin
  *   \li wxPDF_ALIGN_CENTER center the text
  *   \li wxPDF_ALIGN_RIGHT align the text at the right margin
  *   \li wxPDF_ALIGN_JUSTIFY justify the text (default)
  *
  * \param valign Allows to vertical align the text. Possible values are:
  *   \li wxPDF_ALIGN_TOP align the text at the top of the box
  *   \li wxPDF_ALIGN_MIDDLE center the text vertically in the box
  *   \li wxPDF_ALIGN_BOTTOM align the text at the bottom of the box
  *
  * \param border Indicates if borders must be drawn around the text box. The value can be 
  *   \li wxPDF_BORDER_NONE no border
  *   \li wxPDF_BORDER_LEFT left border
  *   \li wxPDF_BORDER_RIGHT right border
  *   \li wxPDF_BORDER_TOP top border
  *   \li wxPDF_BORDER_BOTTOM bottom border
  *   \li wxPDF_BORDER_FRAME border on all sides
  * 
  * or a combination of them.
  * \param fill Indicates if the cell background must be painted (1) or transparent (0). Default value: 0.
  */
  virtual int TextBox(double w, double h, const wxString& txt,
                      int halign = wxPDF_ALIGN_JUSTIFY, int valign = wxPDF_ALIGN_TOP,
                      int border = 0, int fill = 0);

  /// This method prints text from the current position.
  /**
  * When the right margin is reached (or the \n character is met) a line break occurs and text continues
  * from the left margin. Upon method exit, the current position is left just at the end of the text.
  * It is possible to put a link on the text.
  * \param h Line height
  * \param txt String to print
  * \param link URL or identifier returned by AddLink()
  * \see SetFont(), SetTextColor(), AddLink(), MultiCell(), SetAutoPageBreak()
  */
  virtual void Write(double h, const wxString& txt, const wxPdfLink& link = wxPdfLink(-1));

  /// This method prints text with cell attributes from the current position.
  /**
  * When the right margin is reached (or the \n character is met) a line break occurs and text continues
  * from the left margin. Upon method exit, the current position is left just at the end of the text.
  * It is possible to put a link on the text.
  * \param h Line height
  * \param txt String to print
  * \param border Indicates if borders must be drawn around the cell. The value can be 
  *   \li wxPDF_BORDER_NONE no border
  *   \li wxPDF_BORDER_LEFT left border
  *   \li wxPDF_BORDER_RIGHT right border
  *   \li wxPDF_BORDER_TOP top border
  *   \li wxPDF_BORDER_BOTTOM bottom border
  *   \li wxPDF_BORDER_FRAME border on all sides
  * 
  * or a combination of them.
  * \param fill Indicates if the cell background must be painted (1) or transparent (0). Default value: 0.
  * \param link URL or identifier returned by AddLink()
  * \see SetFont(), SetTextColor(), AddLink(), MultiCell(), SetAutoPageBreak()
  */
  virtual void WriteCell(double h, const wxString& txt, int border = wxPDF_BORDER_NONE, int fill = 0, const wxPdfLink& link = wxPdfLink(-1));

  /// Puts an image in the page. 
  /**
  * The upper-left corner must be given. The dimensions can be specified in different ways:
  *   \li explicit width and height (expressed in user unit)
  *   \li one explicit dimension, the other being calculated automatically in order to keep the original proportions
  *   \li no explicit dimension, in which case the image is put at 72 dpi
  *
  * Supported formats are JPEG, PNG, GIF and WMF.
  * For JPEG, all flavors are allowed:
  *   \li gray scales,
  *   \li true colors (24 bits),
  *   \li CMYK (32 bits)
  *
  * For PNG, the following flavors are allowed:
  *   \li gray scales on at most 8 bits (256 levels)
  *   \li indexed colors
  *   \li true colors (24 bits)
  *
  * but the following options are not supported:
  *   \li Interlacing
  *   \li Alpha channel
  *
  * If a transparent color is defined, it will be taken into account (but will be only interpreted
  * by Acrobat 4 and above).
  *
  * For GIF, all flavors the wsWidgets GIF decoder is able to handle are supported
  *
  * For WMF: WMF files contain vector data described in terms of Windows Graphics Device Interface
  * (GDI) commands. There are approximately 80 different GDI commands allowed for in the WMF standard.
  * This method interprets only a small subset of these, but is sufficient to display most WMF images.
  * Please feel free to add further functionality.
  *
  * The format can be specified explicitly or inferred from the file extension.
  *
  * It is possible to put a link on the image.
  *
  * Remark: if an image is used several times, only one copy will be embedded in the file.
  *
  * \param file Name of the file containing the image.
  * \param x Abscissa of the upper-left corner.
  * \param y Ordinate of the upper-left corner.
  * \param w Width of the image in the page. If not specified or equal to zero, it is automatically calculated.
  * \param h Height of the image in the page. If not specified or equal to zero, it is automatically calculated.
  * \param type Image format. Possible values are (case insensitive): JPG, JPEG, PNG, GIF, WMF.
  * If not specified, the type is inferred from the file extension.
  * \param link URL or identifier returned by AddLink().
  * \see AddLink()
  */
  virtual bool Image(const wxString& file, double x, double y, double w = 0, double h = 0, 
                     const wxString& type = wxEmptyString,
                     const wxPdfLink& link = wxPdfLink(-1));

  /**
  * Puts an image in the page
  * The image is given by an wxImage-Object
  */
  virtual bool Image(const wxString& name, const wxImage& image,
                     double x, double y, double w = 0, double h = 0,
                     const wxPdfLink& link = wxPdfLink(-1));

  /// Puts a rotated image in the page. 
  /**
  * The upper-left corner must be given.
  *
  * The format can be specified explicitly or inferred from the file extension.
  *
  * It is possible to put a link on the image.
  *
  * Remark: if an image is used several times, only one copy will be embedded in the file.
  *
  * \param file Name of the file containing the image.
  * \param x Abscissa of the upper-left corner.
  * \param y Ordinate of the upper-left corner.
  * \param w Width of the image in the page. If not specified or equal to zero, it is automatically calculated.
  * \param h Height of the image in the page. If not specified or equal to zero, it is automatically calculated.
  * \param angle Angle of rotation
  * \param type Image format. Possible values are (case insensitive): JPG, JPEG, PNG, GIF, WMF.
  * If not specified, the type is inferred from the file extension.
  * \param link URL or identifier returned by AddLink().
  * \see Image(), AddLink()
  */
  virtual void RotatedImage(const wxString& file, double x, double y, double w, double h,
                            double angle,
                            const wxString& type = wxEmptyString,
                            const wxPdfLink& link = wxPdfLink(-1));

  /// Performs a line break.
  /**
  * The current abscissa goes back to the left margin and the ordinate increases by the amount passed in parameter.
  * \param h The height of the break. By default, the value equals the height of the last printed cell.
  * \see Cell()
  */
  virtual void Ln(double h = -1);

  /// Returns the abscissa of the current position.
  /**
  * \return float
  * \see SetX(), GetY(), SetY()
  */
  virtual double GetX();
  
  /// Defines the abscissa of the current position. 
  /**
  * If the passed value is negative, it is relative to the right of the page.
  * \param x The value of the abscissa.
  * \see GetX(), GetY(), SetY(), SetXY()
  */
  virtual void SetX(double x);
  
  /// Returns the ordinate of the current position.
  /**
  * \return float
  * \see SetY(), GetX(), SetX()
  */
  virtual double GetY();
  
  /// Moves the current abscissa back to the left margin and sets the ordinate. 
  /**
  * If the passed value is negative, it is relative to the bottom of the page.
  * \param y The value of the ordinate.
  * \see GetX(), GetY(), SetY(), SetXY()
  */
  virtual void SetY(double y);
  
  /// Defines the abscissa and ordinate of the current position. 
  /**
  * If the passed values are negative, they are relative respectively to the right and bottom of the page.
  * \param x The value of the abscissa
  * \param y The value of the ordinate
  * \see SetX(), SetY()
  */
  virtual void SetXY(double x, double y);
  
  /// Saves the document to a file on disk
  /**
  * The method first calls Close() if necessary to terminate the document.
  * \param name The name of the file. If not given, the document will be named 'doc.pdf'
  * \see Close()
  */
  virtual void SaveAsFile(const wxString& name = wxEmptyString);

  /// Define text as clipping area
  /**
  * A clipping area restricts the display and prevents any elements from showing outside of it.
  * \param x Abscissa of the origin
  * \param y Ordinate of the origin
  * \param txt String to print
  * \param outline Draw the outline or not.
  */
  virtual void ClippingText(double x, double y, const wxString& txt, bool outline = false);

  /// Define rectangle as clipping area
  /**
  * A clipping area restricts the display and prevents any elements from showing outside of it.
  * \param x Abscissa of the upper-left corner
  * \param y Ordinate of the upper-left corner
  * \param w Width of the rectangle
  * \param h Height of the rectangle
  * \param outline Draw the outline or not.
  */
  virtual void ClippingRect(double x, double y, double w, double h, bool outline = false);

  /// Define ellipse as clipping area
  /**
  * A clipping area restricts the display and prevents any elements from showing outside of it.
  * \param x Abscissa of the Center point
  * \param y Ordinate of the Center point
  * \param rx: Horizontal radius
  * \param ry: Vertical radius (if ry = 0, draws a circle)
  * \param outline Draw the outline or not. (Default false)
  */
  virtual void ClippingEllipse(double x, double y, double rx, double ry = 0, bool outline = false);

  /// Remove clipping area
  /**
  * Once you have finished using the clipping, you must remove it with UnsetClipping().
  */
  virtual void UnsetClipping();

  /// Prints a cell clipped to a rectangular area
  /**
  * The upper-left corner of the cell corresponds to the current position. The text can be aligned
  * or centered. After the call, the current position moves to the right or to the next line.
  * It is possible to put a link on the text.
  * If automatic page breaking is enabled and the cell goes beyond the limit, a page break is done
  * before outputting.
  * \param w Cell width.
  * \param h Cell height.
  * \param txt String to print. Default value: empty string.
  * \param border Indicates if borders must be drawn around the cell. The value can be 
  *   \li wxPDF_BORDER_NONE no border
  *   \li wxPDF_BORDER_LEFT left border
  *   \li wxPDF_BORDER_RIGHT right border
  *   \li wxPDF_BORDER_TOP top border
  *   \li wxPDF_BORDER_BOTTOM bottom border
  *   \li wxPDF_BORDER_FRAME border on all sides
  * 
  * or a combination of them.
  * \param ln Indicates where the current position should go after the call. Possible values are:
  *   \li 0: to the right
  *   \li 1: to the beginning of the next line
  *   \li 2: below
  *
  * Putting 1 is equivalent to putting 0 and calling Ln() just after. Default value: 0.
  * \param align Allows to center or align the text. Possible values are:<ul><li>L or empty string: left align (default value)</li><li>C: center</li><li>R: right align</li></ul>
  *   \li wxPDF_ALIGN_LEFT align the text at the left margin
  *   \li wxPDF_ALIGN_CENTER center the text
  *   \li wxPDF_ALIGN_RIGHT align the text at the right margin
  *   \li wxPDF_ALIGN_JUSTIFY justify the text
  *
  * \param fill Indicates if the cell background must be painted (1) or transparent (0). Default value: 0.
  * \param link URL or identifier returned by AddLink().
  * \see SetFont(), SetDrawColor(), SetFillColor(), SetTextColor(), SetLineWidth(), AddLink(), Ln(), MultiCell(), Write(), SetAutoPageBreak()
  */
  virtual void ClippedCell(double w, double h = 0., const wxString& txt = wxEmptyString,
                           int border = wxPDF_BORDER_NONE, int ln = 0, 
                           int align = wxPDF_ALIGN_LEFT, int fill = 0, 
                           const wxPdfLink& link = wxPdfLink(-1));

  /// Enters a transformation environment
  /**
  * Before applying any transformation this method should be invoked.
  * All transformation method invoke it implicitly if necessary.
  * All open transformation environments are closed implicitly on page end.
  */
  virtual void StartTransform();

  /// Performs scaling in X direction only
  /**
  * A scaling transformation is applied for the X direction.
  * \param sx: scaling factor for width as percent. 0 is not allowed.
  * \param x: abscissa of the scaling center. Default is current x position
  * \param y: ordinate of the scaling center. Default is current y position
  */
  virtual bool ScaleX(double sx, double x = -1, double y = -1);

  /// Performs scaling in Y direction only
  /**
  * A scaling transformation is applied for the Y direction.
  * \param sy: scaling factor for height as percent. 0 is not allowed.
  * \param x: abscissa of the scaling center. Default is current x position
  * \param y: ordinate of the scaling center. Default is current y position
  */
  virtual bool ScaleY(double sy, double x = -1, double y = -1);

  /// Performs equal scaling in X and Y direction
  /**
  * A scaling transformation is applied for both - X and Y - directions.
  * \param s: scaling factor for width and height as percent. 0 is not allowed.
  * \param x: abscissa of the scaling center. Default is current x position
  * \param y: ordinate of the scaling center. Default is current y position
  */
  virtual bool ScaleXY(double s, double x = -1, double y = -1);

  /// Performs scaling in X and Y direction
  /**
  * A scaling transformation is applied independently for X and Y direction.
  * \param sx: scaling factor for width in percent. 0 is not allowed.
  * \param sy: scaling factor for height in percent. 0 is not allowed.
  * \param x: abscissa of the scaling center. Default is current x position
  * \param y: ordinate of the scaling center. Default is current y position
  */
  virtual bool Scale(double sx, double sy, double x = -1, double y = -1);

  /// Performs a horizontal mirroring transformation
  /**
  * Alias for scaling -100% in x-direction
  * \param x: abscissa of the axis of reflection
  */
  virtual void MirrorH(double x = -1);

  /// Performs a vertical mirroring transformation
  /**
  * Alias for scaling -100% in y-direction
  * \param y: abscissa of the axis of reflection
  */
  virtual void MirrorV(double y = -1);

  /// Moves the X origin
  /**
  * \param tx: movement to the right
  */
  virtual void TranslateX(double tx);

  /// Moves the Y origin
  /**
  * \param ty: movement to the bottom
  */
  virtual void TranslateY(double ty);

  /// Moves the origin
  /**
  * \param tx: movement to the right
  * \param ty: movement to the bottom
  */
  virtual void Translate(double tx, double ty);

//  virtual void Rotate(double angle, double x = -1, double y = -1);

  /// Performs a skewing in both X direction only
  /**
  * \param xAngle: angle in degrees between -90 (skew to the left) and 90 (skew to the right)
  * \param x: abscissa of the skewing center. default is current x position
  * \param y: ordinate of the skewing center. default is current y position
  */
  virtual bool SkewX(double xAngle, double x = -1, double y = -1);

  /// Performs a skewing in Y direction only
  /**
  * \param yAngle: angle in degrees between -90 (skew to the bottom) and 90 (skew to the top)
  * \param x: abscissa of the skewing center. default is current x position
  * \param y: ordinate of the skewing center. default is current y position
  */
  virtual bool SkewY(double yAngle, double x = -1, double y = -1);

  /// Performs a skewing in both X and Y directions
  /**
  * \param xAngle: angle in degrees between -90 (skew to the left) and 90 (skew to the right)
  * \param yAngle: angle in degrees between -90 (skew to the bottom) and 90 (skew to the top)
  * \param x: abscissa of the skewing center. default is current x position
  * \param y: ordinate of the skewing center. default is current y position
  */
  virtual bool Skew(double xAngle, double yAngle, double x = -1, double y = -1);

  /// Leaves a transformation environment
  /**
  * This method should be invoked to cancel a transformation environment
  * opened by StartTransform.
  * All open transformation environments are closed implicitly on page end.
  */
  virtual void StopTransform();

  /// Paints a linear gradient shading to rectangular area
  /**
  * \param x abscissa of the top left corner of the rectangle.
  * \param y ordinate of the top left corner of the rectangle.
  * \param w width of the rectangle.
  * \param h height of the rectangle.
  * \param col1 first color (RGB components).
  * \param col2 second color (RGB components).
  */
  virtual void LinearGradient(double x, double y, double w, double h,
                              const wxColour& col1, const wxColour& col2);

  /// Paints a linear gradient shading to rectangular area
  /**
  * \param x abscissa of the top left corner of the rectangle.
  * \param y ordinate of the top left corner of the rectangle.
  * \param w width of the rectangle.
  * \param h height of the rectangle.
  * \param col1 first color (RGB components).
  * \param col2 second color (RGB components).
  * \param coords array of the form (x1, y1, x2, y2) which defines the gradient vector (see linear_gradient_coords.jpg).
  *               The default value is from left to right (x1=0, y1=0, x2=1, y2=0).
  */
  virtual void LinearGradient(double x, double y, double w, double h,
                              const wxColour& col1, const wxColour& col2,
                              double coords[4]);

  /// Paints a radial gradient shading to rectangular area
  /**
  * \param x abscissa of the top left corner of the rectangle.
  * \param y ordinate of the top left corner of the rectangle.
  * \param w width of the rectangle.
  * \param h height of the rectangle.
  * \param col1 first color (RGB components).
  * \param col2 second color (RGB components).
  */
  virtual void RadialGradient(double x, double y, double w, double h,
                              const wxColour& col1, const wxColour& col2);

  /// Paints a radial gradient shading to rectangular area
  /**
  * \param x abscissa of the top left corner of the rectangle.
  * \param y ordinate of the top left corner of the rectangle.
  * \param w width of the rectangle.
  * \param h height of the rectangle.
  * \param col1 first color (RGB components).
  * \param col2 second color (RGB components).
  * \param coords array of the form (fx, fy, cx, cy, r) where (fx, fy) is the starting point
  *               of the gradient with color1, (cx, cy) is the center of the circle with
  *               color2, and r is the radius of the circle (see radial_gradient_coords.jpg).
  *               (fx, fy) should be inside the circle, otherwise some areas will not be defined.
  */
  virtual void RadialGradient(double x, double y, double w, double h,
                              const wxColour& col1, const wxColour& col2, 
                              double coords[5]);

  /// Draws a graphical marker symbol
  /**
  * \param x abscissa of the marker's center 
  * \param y ordinate of the marker's center
  * \param markerType type of the marker 
  * \param size size of the marker
  */
  virtual void Marker(double x, double y, wxPdfMarker markerType, double size);

  /// Adds a text annotation
  /**
  * \param x abscissa of the annotation symbol
  * \param y ordinate of the annotation symbol
  * \param text annotation text
  */
  virtual void Annotate(double x, double y, const wxString& text);

  /// Appends Javascript
  /**
  * Allows to append Javascript code to a Javascript object at the document level.
  * \param javascript Javascript code to be appended
  */
  virtual void AppendJavascript(const wxString& javascript);

  /// Prints a string containing simple XML markup
  /**
  * Output starts at the current position.
  * \param str string containing text with simple XML markup
  * \see \ref writexml
  */
  void WriteXml(const wxString& str);

  /// Adds a check box field at the current position
  /**
  * Adds a check box to the list of form fields at the current position
  * \param name field name of the check box
  * \param width width of the check box
  * \param checked default value of the check box
  */
  void CheckBox(const wxString& name, double width, bool checked = false);

  /// Adds a check box field
  /**
  * Adds a check box to the list of form fields
  * \param name field name of the check box
  * \param x abscissa of the check box position
  * \param y ordinate of the check box position
  * \param width width of the check box
  * \param checked default value of the check box
  */
  void CheckBox(const wxString& name, double x, double y, double width, bool checked = false);
  
  /// Adds a combo box field at the current position
  /**
  * Adds a combo box to the list of form fields at the current position
  * \param name field name of the combo box
  * \param width width of the combo box
  * \param height height of the combo box
  * \param values array of option values of the combo box
  */
  void ComboBox(const wxString& name, double width, double height, const wxArrayString& values);

  /// Adds a combo box field
  /**
  * Adds a combo box to the list of form fields
  * \param name field name of the combo box
  * \param x abscissa of the combo box position
  * \param y ordinate of the combo box position
  * \param width width of the combo box
  * \param height height of the combo box
  * \param values array of option values of the combo box
  */
  void ComboBox(const wxString& name, 
                double x, double y, double width, double height, 
                const wxArrayString& values);
  
  /// Adds a push button at the current position
  /**
  * Adds a push button to the list of form fields at the current position
  * \param name field name of the push button
  * \param width width of the push button
  * \param height height of the push button
  * \param caption caption of the push button
  * \param action associated Javascript action
  */
  void PushButton(const wxString& name, double width, double height, 
                  const wxString& caption, const wxString& action);

  /// Adds a push button
  /**
  * Adds a push button to the list of form fields
  * \param name field name of the push button
  * \param x abscissa of the push button position
  * \param y ordinate of the push button position
  * \param width width of the push button
  * \param height height of the push button
  * \param caption caption of the push button
  * \param action associated Javascript action
  */
  void PushButton(const wxString& name, double x, double y, double width, double height, 
                  const wxString& caption, const wxString& action);
  
  /// Adds a radio button at the current position
  /**
  * Adds a radio button to the list of form fields at the current position
  * \param group name of the radio button group this radio button belongs to
  * \param name field name of the radio button
  * \param width width of the radio button
  */
  void RadioButton(const wxString& group, const wxString& name, double width);
  
  /// Adds a radio button
  /**
  * Adds a radio button to the list of form fields
  * \param group name of the radio button group this radio button belongs to
  * \param name field name of the radio button
  * \param x abscissa of the radio button position
  * \param y ordinate of the radio button position
  * \param width width of the radio button
  */
  void RadioButton(const wxString& group, const wxString& name, 
                   double x, double y, double width);

  /// Adds a text field at the current position
  /**
  * Adds a text field to the list of form fields at the current position
  * \param name field name of the text field
  * \param width width of the text field
  * \param height height of the text field
  * \param value default value of the text field
  * \param multiline flag whether the text field is a multiline field or not
  */
  void TextField(const wxString& name, double width, double height,
                 const wxString& value = wxEmptyString, bool multiline = false);

  /// Adds a text field
  /**
  * Adds a text field to the list of form fields
  * \param name field name of the text field
  * \param x abscissa of the text field position
  * \param y ordinate of the text field position
  * \param width width of the text field
  * \param height height of the text field
  * \param value default value of the text field
  * \param multiline flag whether the text field is a multiline field or not
  */
  void TextField(const wxString& name, 
                 double x, double y, double width, double height,
                 const wxString& value = wxEmptyString, bool multiline = false);

  /// Sets colors for form fields
  /**
  * Sets the border, background and text color to be used
  * for all subsequent form field additions until this method is called again
  * with different values.
  * \param borderColor color of the form field's border
  * \param backgroundColor color of the form field's background
  * \param textColor color of the form field's font
  */
  void SetFormColors(const wxPdfColour& borderColor = wxPdfColour(),
                     const wxPdfColour& backgroundColor = wxPdfColour(250),
                     const wxPdfColour& textColor = wxPdfColour());

  /// Sets the border style for form fields
  /**
  * Sets the border width and style to be used
  * for all subsequent form field additions until this method is called again
  * with different values.
  * \param borderStyle style of the form field's border
  *   \li wxPDF_BORDER_SOLID - solid border
  *   \li wxPDF_BORDER_DASHED - dashed border
  *   \li wxPDF_BORDER_BEVELED - beveled border
  *   \li wxPDF_BORDER_INSET - inset border
  *   \li wxPDF_BORDER_UNDERLINE - border on the bottom side only
  * \param borderWidth width of the form field's border
  */
  void SetFormBorderStyle(wxPdfBorderStyle borderStyle = wxPDF_BORDER_SOLID,
                          double borderWidth = -1);

  /// Convert a wxColour to the corresponding PDF specification
  static wxString RGB2String(const wxColour& color);

  /// Format a floating point number with a fixed precision
  static wxString Double2String(double value, int precision = 0);

  /// Parse a floating point number
  static double String2Double(const wxString& str);

  /// Convert integer number to roman number
  static wxString Convert2Roman(int value);

  /// Force a floating point number into a fixed range
  static double ForceRange(double value, double minValue, double maxValue);

protected:

  /// Return fonts path
  virtual wxString GetFontPath();

  /// Select font
  virtual bool SelectFont(const wxString& family,
                          const wxString& style = wxEmptyString,
                          double size = 0, bool setFont = true);

  /// Start document
  virtual void BeginPage(int orientation);
  
  /// End of page contents
  virtual void EndPage();
  
  /// End dociment
  virtual void EndDoc();

  /// Add header
  virtual void PutHeader();
  
  /// Add pages.
  virtual void PutPages();

  /// Replace page number aliases
  virtual void ReplaceNbPagesAlias();
  
  /// Add resources
  virtual void PutResources();

  /// Add bookmarks
  virtual void PutBookmarks();

  /// Add shaders
  virtual void PutShaders();

  /// Adds fonts
  virtual void PutFonts();
  
  /// Add images
  virtual void PutImages();

  /// Add spot colors
  virtual void PutSpotColors();

  /// Add Javascript (document level)
  virtual void PutJavaScript();

  /// Add resource dictionary
  virtual void PutResourceDict();
  
  /// Add encryption info.
  virtual void PutEncryption();

  /// Add form fields
  virtual void PutFormFields();

  /// Add info.
  virtual void PutInfo();
  
  /// Addcatalog
  virtual void PutCatalog();
  
  /// Add object dictionary
  virtual void PutXObjectDict();
  
  /// Add trailer
  virtual void PutTrailer();
  
  /// Get new object id
  int GetNewObjId();

  /// Begin a new object
  void NewObj();
  
  /// Decorate text
  wxString DoDecoration(double x, double y, const wxString& txt);

  /// Format a text string
  void TextEscape(const wxString& s, bool newline = true);

  /// Add byte stream
  void PutStream(wxMemoryOutputStream& s);
  
  /// Add a text string to the document
  void OutTextstring(const wxString& s, bool newline = true);

  /// Add an ASCII text string to the document
  void OutAsciiTextstring(const wxString& s, bool newline = true);
  
  /// Add \ before \, ( and )
  void OutEscape(const char* s, int len);

  /// Add ASCII string
  void OutAscii(const wxString& s, bool newline = true);

  /// Add character string
  void Out(const char* s, bool newline = true);

  /// Add len characters
  void Out(const char* s, int len, bool newline = true);

  /// Sets a draw point
  void OutPoint(double x, double y);

  /// Sets a draw point relative to current position
  void OutPointRelative(double dx, double dy);

  /// Draws a line from last draw point
  void OutLine(double x, double y);

  /// Draws a line relative from last draw point
  void OutLineRelative(double dx, double dy);

  /// Draws a Bézier curve from last draw point
  void OutCurve(double x1, double y1, double x2, double y2, double x3, double y3);

  /// Perform transformation
  void Transform(double tm[6]);

  /// Paints a linear or radial gradient shading to a rectangular area
  void Gradient(int type, const wxColour& col1, const wxColour& col2, double coords[]);

  /// Adds a form field to the document
  void AddFormField(wxPdfAnnotationWidget* field, bool setFormField = true);

  /// Add an indirect object to the document
  void OutIndirectObject(wxPdfIndirectObject* object);

  /// Add an image object to the document
  void OutImage(wxPdfImage* currentImage,
                double x, double y, double w, double h, const wxPdfLink& link);

  /// Prepare an XML cell for output
  void PrepareXmlCell(wxXmlNode* node, wxPdfCellContext& context);

  /// Output a prepared XML cell
  void WriteXmlCell(wxXmlNode* node, wxPdfCellContext& context);

  /// Take alignment of an XML cell into account
  void DoXmlAlign(wxPdfCellContext& context);

  /// Prepare an XML table for output
  void PrepareXmlTable(wxXmlNode* node, wxPdfCellContext& context);

  /// Output a prepared XML table
  void WriteXmlTable(wxPdfCellContext& context);

  /// Initialize the core fonts
  void InitializeCoreFonts();

  /// Create a unique ID
  static wxString GetUniqueId(const wxString& prefix = wxEmptyString);

private:
  int                  m_page;                ///< current page number
  int                  m_n;                   ///< current object number
  int                  m_firstPageId;         ///< object id of the first page
 
  wxPdfOffsetHashMap*  m_offsets;             ///< array of object offsets

  wxMemoryOutputStream m_buffer;              ///< buffer holding in-memory PDF
  wxPdfPageHashMap*    m_pages;               ///< array containing pages
  int                  m_state;               ///< current document state

  bool                 m_compress;            ///< compression flag
  int                  m_defOrientation;      ///< default orientation
  int                  m_curOrientation;      ///< current orientation
  wxPdfBoolHashMap*    m_orientationChanges;  ///< array indicating orientation changes

  double               m_k;                   ///< scale factor (number of points in user unit)
  double               m_fwPt;                ///< width of page format in points
  double               m_fhPt;                ///< height of page format in points
  double               m_fw;                  ///< width of page format in user unit
  double               m_fh;                  ///< height of page format in user unit
  double               m_wPt;                 ///< current width of page in points
  double               m_hPt;                 ///< current height of page in points
  double               m_w;                   ///< current width of page in user unit
  double               m_h;                   ///< current height of page in user unit
  double               m_imgscale;            ///< image scale factor

  double               m_tMargin;             ///< top margin
  double               m_bMargin;             ///< page break margin
  double               m_lMargin;             ///< left margin
  double               m_rMargin;             ///< right margin
  double               m_cMargin;             ///< cell margin

  double               m_x;                   ///< current x position in user unit for cell positioning
  double               m_y;                   ///< current y position in user unit for cell positioning
  double               m_angle;               ///< current rotation angle
  double               m_lasth;               ///< height of last cell printed
  double               m_lineWidth;           ///< line width in user units
  wxPdfLineStyle       m_lineStyle;           ///< current line style

  int                  m_inTransform;         ///< flag for transformation state

  wxPdfCoreFontMap*    m_coreFonts;           ///< array of standard font names (and character widths)
  wxPdfFontHashMap*    m_fonts;               ///< array of used fonts
  wxPdfDiffHashMap*    m_diffs;               ///> array of encoding differences
  wxPdfImageHashMap*   m_images;              ///< array of used images
  wxPdfPageLinksMap*   m_pageLinks;           ///< array of links in pages
  wxPdfLinkHashMap*    m_links;               ///< array of internal links
  wxPdfGradientMap*    m_gradients;           ///< array of gradients
  wxPdfSpotColourMap*  m_spotColors;          ///< array of spot colors

  wxPdfAnnotationsMap* m_annotations;         ///< array of text annotations
  
  wxArrayPtrVoid       m_outlines;            ///< array of bookmarks
  int                  m_outlineRoot;         ///< number of root node
  int                  m_maxOutlineLevel;     ///< max. occuring outline level

  wxString             m_fontFamily;          ///< current font family
  wxString             m_fontStyle;           ///< current font style
  int                  m_decoration;          ///< font decoration flags

  wxPdfFont*           m_currentFont;         ///< current font info

  double               m_fontSizePt;          ///< current font size in points
  double               m_fontSize;            ///< current font size in user unit
  wxString             m_drawColor;           ///< commands for drawing color
  wxString             m_fillColor;           ///< commands for filling color
  wxString             m_textColor;           ///< commands for text color
  bool                 m_colorFlag;           ///< indicates whether fill and text colors are different
  double               m_ws;                  ///< word spacing

  bool                 m_autoPageBreak;       ///< automatic page breaking
  double               m_pageBreakTrigger;    ///< threshold used to trigger page breaks
  bool                 m_inFooter;            ///< flag set when processing footer
  wxPdfZoom            m_zoomMode;            ///< zoom display mode
  double               m_zoomFactor;          ///< zoom factor
  wxPdfLayout          m_layoutMode;          ///< layout display mode

  wxString             m_title;               ///< title
  wxString             m_subject;             ///< subject
  wxString             m_author;              ///< author
  wxString             m_keywords;            ///< keywords
  wxString             m_creator;             ///< creator
  wxString             m_aliasNbPages;        ///< alias for total number of pages
  wxString             m_PDFVersion;          ///< PDF version number

  double               m_img_rb_x;            ///< right-bottom corner X coordinate of inserted image
  double               m_img_rb_y;            ///< right-bottom corner Y coordinate of inserted image

  // Encryption
  bool                 m_encrypted;           ///< flag whether document is protected
  wxPdfEncrypt*        m_encryptor;           ///< encryptor instance
  int                  m_encObjId;            ///< encrypted object id

  // Javascript
  int                  m_nJS;                 ///< Javascript object number
  wxString             m_javascript;          ///< Javascript string

  // Forms
  int                  m_zapfdingbats;        ///< index of font ZapfDingBats
  wxPdfFormFieldsMap*  m_formFields;          ///< array of form fields
  wxPdfFormAnnotsMap*  m_formAnnotations;     ///< array of form field annotations
  wxPdfRadioGroupMap*  m_radioGroups;         ///< array of radio button groups
  wxString             m_formBorderColor;     ///< form field border color
  wxString             m_formBackgroundColor; ///< form field background color
  wxString             m_formTextColor;       ///< form field text color
  wxString             m_formBorderStyle;     ///< form field border style
  double               m_formBorderWidth;     ///< form field border width

  static bool          ms_seeded;             ///< flag whether random number generator is seeded
  static int           ms_s1;                 ///< Random number generator seed 1
  static int           ms_s2;                 ///< Random number generator seed 2

  friend class wxPdfImage;
  friend class wxPdfTable;
};

#endif
