/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "helper.h"

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

bool nsHeaderFixUp::IsInsideMultilineComment(wxString& Line)
{
  int EndCommentPos = Line.Find(_T("*/"));
  bool OutsideMultilineComment = false;

  if ( EndCommentPos == wxNOT_FOUND )
    Line.Clear(); // skip line
  else
  {
    Line.Remove(0,EndCommentPos+2);
    OutsideMultilineComment = true; // END Multiline comment
  }

  return !OutsideMultilineComment;
}

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

bool nsHeaderFixUp::IsInsideString(wxString& Line)
{
  int EndStringPos = Line.Find(_T('\"'));
  bool OutsideString = false;

  if ( EndStringPos == wxNOT_FOUND )
    Line.Clear(); // Multi-line string -> skip line
  else if ( EndStringPos > 0 )
  {
    if ( Line.GetChar(EndStringPos-1) == '\\' )
      // Something like "\"cbMessageBox\"" -> remove \"
      Line.Remove(0,EndStringPos+1);
    else
      OutsideString = true; // EndStringPos > 0
  }
  else
      OutsideString = true; // EndStringPos = 0

  if ( OutsideString ) // END Inside string
    Line.Remove(0,EndStringPos+1);

  return !OutsideString;
}

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

bool nsHeaderFixUp::IsNextChar(const wxChar&   NextCharInLine,
                               const wxChar&   ThisChar,
                               const wxString& RemainingLine)
{
  wxString s_ChNext = NextCharInLine; // conversion for using Trim() and IsSameAs()
  wxString s_ChComp = ThisChar;       // conversion for using Trim() and IsSameAs()

  // in case of in-equality AND if NextCharInLine is a space, trim spaces and try again...
  // (but NOT, if we are actually looking for a space!)
  if ( !s_ChNext.IsSameAs(s_ChComp) && !s_ChNext.Trim().IsEmpty() )
  {
    // remove leading spaces from remaining line so we get the
    // next proper character not being space
    wxString TrimmedLine(RemainingLine); TrimmedLine.Trim(false);
    if ( !TrimmedLine.IsEmpty() )
      s_ChNext = TrimmedLine.GetChar(0); // first non-space
  }

  return ( s_ChNext.IsSameAs(s_ChComp) );
}
