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

bool nsHeaderFixUp::IsNextChar(const wxString& ThisChar,
                               const wxChar&   NextCharInLine,
                               const wxString& RemainingLine)
{
  wxString s_Ch = NextCharInLine;
  if ( !s_Ch.IsSameAs(ThisChar) && !s_Ch.Trim().IsEmpty() )
  {
    wxString TrimmedLine(RemainingLine);
    TrimmedLine.Trim(false);
    if ( !TrimmedLine.IsEmpty() )
      wxString s_Ch = TrimmedLine.GetChar(0);
  }

  if ( s_Ch.IsSameAs(ThisChar) )
    return true;

  return false;
}
