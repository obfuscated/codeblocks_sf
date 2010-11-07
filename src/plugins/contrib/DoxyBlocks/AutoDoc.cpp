/**************************************************************************//**
 * \file		AutoDoc.cpp
 * \author	Gary Harris
 * \date		02-03-2010
 *
 * DoxyBlocks - doxygen integration for Code::Blocks.					\n
 * Copyright (C) 2010 Gary Harris.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *****************************************************************************/

#include <sdk.h> // Code::Blocks SDK
#ifndef CB_PRECOMP
#include <configmanager.h>
#include <editormanager.h>
#include <cbeditor.h>
#include <logmanager.h>
#endif
#include <wx/tokenzr.h>
#include "DoxyBlocks.h"
#include "Expressions.h"


/*! \brief Insert a comment block at the current line.
 */
void DoxyBlocks::BlockComment(void)
{
	if(!IsProjectOpen()){
		return;
	}

    AppendToLog(_("Writing block comment..."));
    int iBlockComment = m_pConfig->GetBlockComment();
	bool bUseAtInTags = m_pConfig->GetUseAtInTags();
    cbEditor *cbEd = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if(!cbEd){
        return;
    }
    cbStyledTextCtrl *control = cbEd->GetControl();

    // Get the position of the beginning of the current line and insert.
    wxInt32 line = control->GetCurrentLine();
    int iPos = control->PositionFromLine(line);
    int iIndent = control->GetLineIndentation(line);
	wxString sIndent(wxT(' '), iIndent);

    // Comment strings.
    wxString sTagChar;
    sTagChar = bUseAtInTags ? wxT("@") : wxT("\\");
    wxString sSpace(wxT(" "));
    wxString sTagBrief(sSpace + sTagChar + wxT("brief"));
    wxString sTagParam(sSpace + sTagChar + wxT("param"));
    wxString sTagReturn(sSpace + sTagChar + wxT("return"));
    wxString sStartComment;
    wxString sMidComment;
    wxString sEndComment;

	GetBlockCommentStrings(iBlockComment, sStartComment, sMidComment, sEndComment);
	wxString sLine = control->GetLine(control->GetCurrentLine());

    // Make the changes undoable in one step.
    control->BeginUndoAction();

    if(reClass.Matches(sLine)){													// Class MyClass : public ParentClass
		StartComment(control, iPos, iBlockComment, sStartComment, sMidComment, sTagBrief, sIndent);
		AddCommentLine(control, iPos, sIndent + sEndComment);
	}
    else if(reStruct.Matches(sLine)){											// struct
		StartComment(control, iPos, iBlockComment, sStartComment, sMidComment, sTagBrief, sIndent);
		AddCommentLine(control, iPos, sIndent + sEndComment);
	}
    else if(reTypedef.Matches(sLine)){										// typedef
		StartComment(control, iPos, iBlockComment, sStartComment, sMidComment, sTagBrief, sIndent);
		AddCommentLine(control, iPos, sIndent + sEndComment);
	}
    else if(reEnum.Matches(sLine)){										// enum
		StartComment(control, iPos, iBlockComment, sStartComment, sMidComment, sTagBrief, sIndent);
		AddCommentLine(control, iPos, sIndent + sEndComment);
	}
    else if(reClassFunctionNoRet.Matches(sLine)){				// class::function(type param, ...) statement
		// THIS SHOULD BE CHECKED BEFORE THE OTHER FUNCTION COMPARISONS.
		StartComment(control, iPos, iBlockComment, sStartComment, sMidComment, sTagBrief, sIndent);
		wxString sParams = reClassFunctionNoRet.GetMatch(sLine, 3);
		CommentFunction(control, iPos, sMidComment, sTagParam, sTagReturn, sIndent, sParams, wxEmptyString, wxEmptyString);
		if(!sParams.IsEmpty()){
			AddCommentLine(control, iPos, sIndent + sMidComment);
		}
		AddCommentLine(control, iPos, sIndent + sEndComment);
    }
    else if(reClassFunction.Matches(sLine)){							// ret class::function(type param, ...) statement
		StartComment(control, iPos, iBlockComment, sStartComment, sMidComment, sTagBrief, sIndent);
		CommentFunction(control, iPos, sMidComment, sTagParam, sTagReturn, sIndent, reClassFunction.GetMatch(sLine, 6), reClassFunction.GetMatch(sLine, 1), reClassFunction.GetMatch(sLine, 4));
		AddCommentLine(control, iPos, sIndent + sMidComment);
		AddCommentLine(control, iPos, sIndent + sEndComment);
	}
    else if(reFunction.Matches(sLine)){									// ret function(type param, ...) statement
		StartComment(control, iPos, iBlockComment, sStartComment, sMidComment, sTagBrief, sIndent);
		CommentFunction(control, iPos, sMidComment, sTagParam, sTagReturn, sIndent, reFunction.GetMatch(sLine, 5), reFunction.GetMatch(sLine, 1), reFunction.GetMatch(sLine, 4));
		AddCommentLine(control, iPos, sIndent + sMidComment);
		AddCommentLine(control, iPos, sIndent + sEndComment);
    }
	else{																			// Unmatched, use default block.
		StartComment(control, iPos, iBlockComment, sStartComment, sMidComment, sTagBrief, sIndent);
		AddCommentLine(control, iPos, sIndent + sMidComment);
		AddCommentLine(control, iPos, sIndent + sMidComment + sTagParam + sSpace);
		AddCommentLine(control, iPos, sIndent + sMidComment + sTagParam + sSpace);
		AddCommentLine(control, iPos, sIndent + sMidComment + sTagReturn + sSpace);
		AddCommentLine(control, iPos, sIndent + sMidComment);
		AddCommentLine(control, iPos, sIndent + sEndComment);
	}

    // Position the cursor at the first text entry position.
	// Skip the first line of visible-style comments.
	if(iBlockComment == 4 || iBlockComment == 5){
		line += 1;
	}
    iPos = control->GetLineEndPosition(line);
    control->GotoPos(iPos);
    control->EndUndoAction();
}

/*! \brief Insert an inline comment at the current cursor position.
 */
void DoxyBlocks::LineComment(void)
{
	if(!IsProjectOpen()){
		return;
	}

    AppendToLog(_("Writing line comment..."));
	int iLineComment = m_pConfig->GetLineComment();
    cbEditor *cbEd = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if(!cbEd){
        return;
    }
    cbStyledTextCtrl *control = cbEd->GetControl();
    int iPos = control->GetCurrentPos();

    wxString sComment;
    switch(iLineComment){
        case 0:			//C/Javadoc.
            sComment = wxT("/**<  */");
            break;
        case 1:			// C++ exclamation.
            sComment = wxT("//!< ");
            break;
        case 2:			// C++ slash.
            sComment = wxT("///< ");
            break;
        case 3:			// Qt.
            sComment = wxT("/*!<  */");
            break;
    }

    // Make the changes undoable in one step.
    control->BeginUndoAction();

    control->InsertText(iPos, sComment);
    // Position the cursor at the text entry position.
    int i = 0;
    while(i < 5){
        control->CharRight();
        i++;
    }
    control->EndUndoAction();
}

/*! \brief Initialise the comment structure strings to the selected style.
 *
 * \param 	iBlockComment	int 			The selected comment style.
 * \param 	sStartComment	wxString 	The comment tag that starts a block.
 * \param 	sMidComment	wxString 	The comment tag that starts each line in a block.
 * \param 	sEndComment	wxString&	The comment tag that ends a block.
 * \return 	void
 *
 */
void DoxyBlocks::GetBlockCommentStrings(int iBlockComment, wxString &sStartComment, wxString &sMidComment, wxString &sEndComment)
{
    switch(iBlockComment){
        case 0:			//C/Javadoc.
            sStartComment = wxT("/**");
            sMidComment = wxT(" *");
            sEndComment = wxT(" */");
            break;
        case 1:			// C++ exclamation.
            sStartComment = wxT("//!");
            sMidComment = wxT("//!");
            sEndComment = wxT("//!");
            break;
        case 2:			// C++ slash.
            sStartComment = wxT("///");
            sMidComment = wxT("///");
            sEndComment = wxT("///");
            break;
        case 3:			// Qt.
            sStartComment = wxT("/*!");
            sMidComment = wxT(" *");
            sEndComment = wxT(" */");
            break;
        case 4:			// Visible C.
            sStartComment = wxT("/********************************************//**");
            sMidComment = wxT(" *");
            sEndComment = wxT(" ***********************************************/");
            break;
        case 5:			// Visible C++.
            sStartComment = wxT("/////////////////////////////////////////////////");
            sMidComment = wxT("///");
            sEndComment = wxT("/////////////////////////////////////////////////");
            break;
    }
}

/*! \brief Insert the tags that begin a comment block in the selected style.
 *
 * \param 	control				cbStyledTextCtrl*	The editor's wxStyledTextControl.
 * \param 	iPos					int&						The current editor position.
 * \param 	iBlockComment	int 						The selected comment style.
 * \param 	sStartComment	wxString 				The comment tag that starts a block.
 * \param 	sMidComment	wxString 				The comment tag that starts each line in a block.
 * \param 	sTagBrief			wxString 				The doxygen tag for a brief description.
 * \param 	sIndent				wxString 				A string of spaces matching the function's indent level.
 * \return 	void
 *
 */
void DoxyBlocks::StartComment(cbStyledTextCtrl *control, int &iPos, int iBlockComment, wxString sStartComment, wxString sMidComment, wxString sTagBrief, wxString sIndent)
{
    wxString sSpace(wxT(" "));

	control->GotoPos(iPos);
	// Doing this first prevents me ending up with the function declaration indented i.e. I work in the blank line.
	control->NewLine();
	control->LineUp();
	if(iBlockComment == 4 || iBlockComment == 5){
		control->AddText(sIndent + sStartComment);
		control->NewLine();
		iPos = control->PositionFromLine(control->GetCurrentLine());
		control->AddText(sIndent + sMidComment + sTagBrief + sSpace);
	}
	else{
		control->AddText(sIndent + sStartComment + sTagBrief + sSpace);
	}
}

/*! \brief	Add comment lines for a function's parameters and return type in the selected style.
 *
 * \param 	control				cbStyledTextCtrl*	The editor's wxStyledTextControl.
 * \param 	iPos					int&						The current editor position.
 * \param 	sMidComment	wxString 				The comment tag that starts each line in a block.
 * \param 	sTagParam		wxString 				The doxygen tag for a parameter description.
 * \param 	sTagReturn		wxString 				The doxygen tag for a return value description.
 * \param 	sIndent				wxString 				A string of spaces matching the function's indent level.
 * \param 	sFunction			wxString 				A string containing the the name of the function being documented.
 * \param 	sParams			wxString 				A string containing the parameters of the function being documented.
 * \param 	sReturn				wxString 				A string containing the return type of the function being documented.
 * \return 	void
 *
 */
void DoxyBlocks::CommentFunction(cbStyledTextCtrl * control, int &iPos, wxString sMidComment, wxString sTagParam, wxString sTagReturn, wxString sIndent, wxString sParams, wxString sReturn, wxString sFunction)
{
    wxString sSpace(wxT(" "));
	// Write an empty comment line to terminate "\brief" if we have params or a return value.
	if(!sParams.IsEmpty() || !sReturn.IsEmpty()){
		AddCommentLine(control, iPos, sIndent + sMidComment);
	}

	// Extract the functions parameters from the parameter string and write a "\param" line for each.
	wxStringTokenizer tokenizer(sParams, wxT(","));
	while(tokenizer.HasMoreTokens()){
		wxString sParam = tokenizer.GetNextToken();
		sParam.Trim();
		sParam.Trim(false);

		// Split on spaces.
		wxStringTokenizer tokenizerParam(sParam, wxT(" "));
		wxArrayString arElements;
		while(tokenizerParam.HasMoreTokens()){
			wxString sElement = tokenizerParam.GetNextToken();
			arElements.Add(sElement);
		}
		// Reorder the elements.
		if(arElements.GetCount() == 4){
				// "const type * name" or "const type & name".
				sParam = arElements[3] + sSpace + arElements[0] + sSpace + arElements[1] + arElements[2];
		}
		else if(arElements.GetCount() == 3){
			if(arElements[1] == wxT("*") || arElements[1] == wxT("&") || arElements[1].StartsWith(wxT("**"))){
				// "type * name" or "type ** name" or "type & name".
				sParam = arElements[2] + sSpace + arElements[0] + arElements[1];
			}
			else{
				// "const type name" or "const type *name" or "const type* name".
				wxString sRightFirst = arElements[2].Left(1);
				wxString sLeftLast = arElements[1].Right(1);
				// "type *name" or "type **name" or "type &name".
				if(sRightFirst == wxT("*") || sRightFirst  == wxT("&")){
					if(arElements[2].StartsWith(wxT("**"))){
						arElements[2].Remove(0, 2);
						sParam = arElements[2] + sSpace + arElements[0] + sSpace + arElements[1] + wxT("**");
					}
					else{
						arElements[2].Remove(0, 1);
						sParam = arElements[2] + sSpace + arElements[0] + sSpace + arElements[1] + sRightFirst;
					}
				}
				// "type* name" or "type& name".
				else if(sLeftLast == wxT("*") || sLeftLast  == wxT("&")){
					arElements[1].RemoveLast();
					sParam = arElements[2] + sSpace + arElements[0] + sSpace + arElements[1] + sLeftLast;
				}
				else{
					sParam = arElements[2] + sSpace + arElements[0] + sSpace + arElements[1];
				}
			}
		}
		else if(arElements.GetCount() == 2){
			wxString sRightFirst = arElements[1].Left(1);
			wxString sLeftLast = arElements[0].Right(1);
			// "type *name" or "type **name" or "type &name".
			if(sRightFirst == wxT("*") || sRightFirst  == wxT("&")){
				if(arElements[1].StartsWith(wxT("**"))){
					arElements[1].Remove(0, 2);
					sParam = arElements[1] + sSpace + arElements[0] + wxT("**");
				}
				else{
					arElements[1].Remove(0, 1);
					sParam = arElements[1] + sSpace + arElements[0] + sRightFirst;
				}
			}
			// "type* name" or "type& name".
			else if(sLeftLast == wxT("*") || sLeftLast  == wxT("&")){
				arElements[0].RemoveLast();
				sParam = arElements[1] + sSpace + arElements[0] + sLeftLast;
			}
			else{
				sParam = arElements[1] + sSpace + arElements[0];
			}
		}
		// The regex ensures that we don't have any other arrangements of params.

		AddCommentLine(control, iPos, sIndent + sMidComment + sTagParam + sSpace + sParam);
	}

	sReturn = ProcessReturnString(sReturn, sFunction);
	// Don't write a "\return" line if there is no return value.
	if(!sReturn.IsEmpty()){
		AddCommentLine(control, iPos, sIndent + sMidComment + sTagReturn + sSpace + sReturn);
	}
}

/*! \brief Add a line to a comment block in the selected style.
 *
 * \param 	control		cbStyledTextCtrl*	The editor's wxStyledTextControl.
 * \param 	iPos			int&						The current editor position.
 * \param 	sText		wxString 				The text to add to the comment block.
 * \return 	void
 *
 */
void DoxyBlocks::AddCommentLine(cbStyledTextCtrl *control, int& iPos, wxString sText)
{
	// Use NewLine() to get the correct line ending chars.
	control->NewLine();
	iPos = control->PositionFromLine(control->GetCurrentLine());
	control->GotoPos(iPos);
	control->AddText(sText);
}

/** \brief
 *
 * \param sReturn 		wxString	Return statement string.
 * \param sFunction 	wxString	Function name string.
 * \return wxString		The processed return string.
 *
 * This function strips the keywords "static" and "inline" from the return statement and formats
 * the rest, processing *, ** and & symbols so that the symbol follows the type name.
 */
wxString DoxyBlocks::ProcessReturnString(wxString sReturn, wxString sFunction)
{
	// Do some preprocessing. Remove static and inline keywords and any leftover space
	// so that the return field won't be written if there is no return value.
	if(sReturn.Contains(wxT("static"))){
		sReturn.Replace(wxT("static"), wxT(""));
	}
	if(sReturn.Contains(wxT("inline"))){
		sReturn.Replace(wxT("inline"), wxT(""));
	}
	sReturn.Trim(false);

	if(!sReturn.IsEmpty()){
		// Handle pointers and references. Symbols in sReturn are already in the right place.
		wxString sFuncFirst = sFunction.Left(1);
		if(sFuncFirst == wxT("*") || sFuncFirst == wxT("&")){
			// "ret *func" or "ret **func" or "ret &func".
			if(sFunction.StartsWith(wxT("**"))){
				sReturn += wxT("**");
				sFunction.Remove(0, 2);
			}
			else{
				sReturn += sFuncFirst;
				sFunction.Remove(0, 1);
			}
		}

		// Remove unnecessary spaces.
		sReturn.Trim();
		int l = sReturn.Length();
		int i = sReturn.rfind(' ', l);
		if(i == l - 2 || i == l - 3){
			// The last space appears before *, ** or &, so remove it.
			wxString sTemp = sReturn.BeforeLast(' ') + sReturn.AfterLast(' ');
			sReturn = sTemp;
		}
	}

	return sReturn;
}
