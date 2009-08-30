/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *   Copyright (C) 2006-2009 by Jim Pattee <jimp03@email.com>
 *   Copyright (C) 1998-2002 by Tal Davidson
 *   <http://www.gnu.org/licenses/lgpl-3.0.html>
 *
 *   This file is a part of Artistic Style - an indentation and
 *   reformatting tool for C, C++, C# and Java source files.
 *   <http://astyle.sourceforge.net>
 *
 *   Artistic Style is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published
 *   by the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Artistic Style is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public License
 *   along with Artistic Style.  If not, see <http://www.gnu.org/licenses/>.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

#include "astyle.h"

#include <algorithm>
#include <fstream>
#include <iostream>


namespace astyle
{
// static member variables
int ASFormatter::formatterFileType = 9;		// initialized with an invalid type
vector<const string*> ASFormatter::headers;
vector<const string*> ASFormatter::nonParenHeaders;
vector<const string*> ASFormatter::preDefinitionHeaders;
vector<const string*> ASFormatter::preCommandHeaders;
vector<const string*> ASFormatter::operators;
vector<const string*> ASFormatter::assignmentOperators;
vector<const string*> ASFormatter::castOperators;

/**
 * Constructor of ASFormatter
 */
ASFormatter::ASFormatter()
{
	sourceIterator = NULL;
	enhancer = new ASEnhancer;
	preBracketHeaderStack = NULL;
	bracketTypeStack = NULL;
	parenStack = NULL;
	lineCommentNoIndent = false;
	formattingStyle = STYLE_NONE;
	bracketFormatMode = NONE_MODE;
	pointerAlignment = ALIGN_NONE;
	isModeManuallySet = false;
	isIndentManuallySet = false;
	shouldPadOperators = false;
	shouldPadParensOutside = false;
	shouldPadParensInside = false;
	shouldPadHeader = false;
	shouldUnPadParens = false;
	shouldBreakOneLineBlocks = true;
	shouldBreakOneLineStatements = true;
	shouldConvertTabs = false;
	shouldBreakBlocks = false;
	shouldBreakClosingHeaderBlocks = false;
	shouldBreakClosingHeaderBrackets = false;
	shouldDeleteEmptyLines = false;
	shouldBreakElseIfs = false;
	shouldAddBrackets = false;
	shouldAddOneLineBrackets = false;
	// the following prevents warning messages with cppcheck
	// it will NOT compile if activated
//	init();
}

/**
 * Destructor of ASFormatter
 */
ASFormatter::~ASFormatter()
{
	// delete ASFormatter stack vectors
	deleteContainer(preBracketHeaderStack);
	deleteContainer(parenStack);
	deleteContainer(bracketTypeStack);

	// delete static member vectors using swap to eliminate memory leak reporting
	// delete ASFormatter static member vectors
	formatterFileType = 9;		// reset to an invalid type
	vector<const string*> headersClear;
	headers.swap(headersClear);
	vector<const string*> nonParenHeadersClear;
	nonParenHeaders.swap(nonParenHeadersClear);
	vector<const string*> preDefinitionHeadersClear;
	preDefinitionHeaders.swap(preDefinitionHeadersClear);
	vector<const string*> preCommandHeadersClear;
	preCommandHeaders.swap(preCommandHeadersClear);
	vector<const string*> operatorsClear;
	operators.swap(operatorsClear);
	vector<const string*> assignmentOperatorsClear;
	assignmentOperators.swap(assignmentOperatorsClear);
	vector<const string*> castOperatorsClear;
	castOperators.swap(castOperatorsClear);

	// delete ASBeautifier static member vectors
	// must be done when the ASFormatter object is deleted (not ASBeautifier)
	ASBeautifier::deleteStaticVectors();

	delete enhancer;
}

/**
 * build vectors for each programing language
 * depending on the file extension.
 */
void ASFormatter::buildLanguageVectors()
{
	if (getFileType() == formatterFileType)  // don't build unless necessary
		return;

	formatterFileType = getFileType();

	headers.clear();
	nonParenHeaders.clear();
	preDefinitionHeaders.clear();
	preCommandHeaders.clear();
	operators.clear();
	assignmentOperators.clear();
	castOperators.clear();

	ASResource::buildHeaders(headers, getFileType());
	ASResource::buildNonParenHeaders(nonParenHeaders, getFileType());
	ASResource::buildPreDefinitionHeaders(preDefinitionHeaders, getFileType());
	ASResource::buildPreCommandHeaders(preCommandHeaders, getFileType());
	if (operators.size() == 0)
		ASResource::buildOperators(operators);
	if (assignmentOperators.size() == 0)
		ASResource::buildAssignmentOperators(assignmentOperators);
	if (castOperators.size() == 0)
		ASResource::buildCastOperators(castOperators);
}

/**
 * set the variables for each preefined style.
 * this will override any previous settings.
 */
void ASFormatter::fixOptionVariableConflicts()
{
	if (formattingStyle == STYLE_ALLMAN)
	{
		setBracketFormatMode(BREAK_MODE);
		setBlockIndent(false);
		setBracketIndent(false);
	}
	else if (formattingStyle == STYLE_JAVA)
	{
		setBracketFormatMode(ATTACH_MODE);
		setBlockIndent(false);
		setBracketIndent(false);
	}
	else if (formattingStyle == STYLE_KandR)
	{
		setBracketFormatMode(LINUX_MODE);
		setBlockIndent(false);
		setBracketIndent(false);
	}
	else if (formattingStyle == STYLE_STROUSTRUP)
	{
		setBracketFormatMode(STROUSTRUP_MODE);
		setBlockIndent(false);
		setBracketIndent(false);
		if (!getIndentManuallySet())
		{
			if (getIndentString() == "\t")
				setTabIndentation(5, getForceTabIndentation());
			else
				setSpaceIndentation(5);
		}
	}
	else if (formattingStyle == STYLE_WHITESMITH)
	{
		setBracketFormatMode(BREAK_MODE);
		setBlockIndent(false);
		setBracketIndent(true);
		setClassIndent(true);
		setSwitchIndent(true);
	}
	else if (formattingStyle == STYLE_BANNER)
	{
		setBracketFormatMode(ATTACH_MODE);
		setBlockIndent(false);
		setBracketIndent(true);
		setClassIndent(true);
		setSwitchIndent(true);
	}
	else if (formattingStyle == STYLE_GNU)
	{
		setBracketFormatMode(BREAK_MODE);
		setBlockIndent(true);
		setBracketIndent(false);
		if (!getIndentManuallySet())
		{
			if (getIndentString() == "\t")
				setTabIndentation(2, getForceTabIndentation());
			else
				setSpaceIndentation(2);
		}
	}
	else if (formattingStyle == STYLE_LINUX)
	{
		setBracketFormatMode(LINUX_MODE);
		setBlockIndent(false);
		setBracketIndent(false);
		if (!getIndentManuallySet())
		{
			if (getIndentString() == "\t")
				setTabIndentation(8, getForceTabIndentation());
			else
				setSpaceIndentation(8);
		}
	}
	else if (formattingStyle == STYLE_HORSTMANN)
	{
		setBracketFormatMode(HORSTMANN_MODE);
		setBlockIndent(false);
		setBracketIndent(false);
		setSwitchIndent(true);
		if (!getIndentManuallySet())
		{
			if (getIndentString() == "\t")
				setTabIndentation(3, getForceTabIndentation());
			else
				setSpaceIndentation(3);
		}
	}
	else if (formattingStyle == STYLE_1TBS)
	{
		setBracketFormatMode(LINUX_MODE);
		setBlockIndent(false);
		setBracketIndent(false);
		setAddBracketsMode(true);
	}

	// add-one-line-brackets implies keep-one-line-blocks
	if (shouldAddOneLineBrackets)
		setBreakOneLineBlocksMode(false);
	// cannot have both indent-blocks and indent-brackets, default to indent-blocks
	if (getBlockIndent())
		setBracketIndent(false);
	// cannot have indent-brackets with horstmann brackets
	if (bracketFormatMode == HORSTMANN_MODE)
		setBracketIndent(false);
}

/**
 * initialize the ASFormatter.
 *
 * init() should be called every time a ASFormatter object is to start
 * formatting a NEW source file.
 * init() recieves a pointer to a DYNAMICALLY CREATED ASSourceIterator object
 * that will be used to iterate through the source code. This object will be
 * deleted during the ASFormatter's destruction, and thus should not be
 * deleted elsewhere.
 *
 * @param iter     a pointer to the DYNAMICALLY CREATED ASSourceIterator object.
 */
void ASFormatter::init(ASSourceIterator *si)
{
	buildLanguageVectors();
	fixOptionVariableConflicts();

	ASBeautifier::init(si);
	enhancer->init(getFileType(),
	               getIndentLength(),
	               getIndentString(),
	               getCaseIndent(),
	               getEmptyLineFill());
	sourceIterator = si;

	initContainer(preBracketHeaderStack, new vector<const string*>);
	initContainer(parenStack, new vector<int>);
	parenStack->push_back(0);               // parenStack must contain this default entry
	initContainer(bracketTypeStack, new vector<BracketType>);
	bracketTypeStack->push_back(NULL_TYPE);

	currentHeader = NULL;
	currentLine = string("");
	readyFormattedLine = string("");
	formattedLine = "";
	currentChar = ' ';
	previousChar = ' ';
	previousCommandChar = ' ';
	previousNonWSChar = ' ';
	quoteChar = '"';
	charNum = 0;
	leadingSpaces = 0;
	preprocBracketTypeStackSize = 0;
	spacePadNum = 0;
	nextLineSpacePadNum = 0;
	currentLineBracketNum = string::npos;
	previousReadyFormattedLineLength = string::npos;
	templateDepth = 0;
	traceLineNumber = 0;
	horstmannIndentChars = 0;
	tabIncrementIn = 0;
	previousBracketType = NULL_TYPE;
	previousOperator = NULL;

	isVirgin = true;
	isInLineComment = false;
	isInComment = false;
	noTrimCommentContinuation = false;
	isInPreprocessor = false;
	doesLineStartComment = false;
	lineEndsInCommentOnly = false;
	lineIsLineCommentOnly = false;
	lineIsEmpty = false;
	isImmediatelyPostCommentOnly = false;
	isImmediatelyPostEmptyLine = false;
	isInQuote = false;
	isInVerbatimQuote = false;
	haveLineContinuationChar = false;
	isInQuoteContinuation = false;
	isSpecialChar = false;
	isNonParenHeader = false;
	foundNamespaceHeader = false;
	foundClassHeader = false;
	foundStructHeader = false;
	foundInterfaceHeader = false;
	foundPreDefinitionHeader = false;
	foundPreCommandHeader = false;
	foundCastOperator = false;
	foundQuestionMark = false;
	isInLineBreak = false;
	endOfCodeReached = false;
	isLineReady = false;
	isPreviousBracketBlockRelated = false;
	isInPotentialCalculation = false;
	shouldReparseCurrentChar = false;
	needHeaderOpeningBracket = false;
	shouldBreakLineAtNextChar = false;
	passedSemicolon = false;
	passedColon = false;
	clearNonInStatement = false;
	isInTemplate = false;
	isInBlParen = false;
	isImmediatelyPostComment = false;
	isImmediatelyPostLineComment = false;
	isImmediatelyPostEmptyBlock = false;
	isImmediatelyPostPreprocessor = false;
	isImmediatelyPostReturn = false;
	isImmediatelyPostOperator = false;
	isCharImmediatelyPostReturn = false;
	isCharImmediatelyPostOperator = false;
	isCharImmediatelyPostComment = false;
	isPreviousCharPostComment = false;
	isCharImmediatelyPostLineComment = false;
	isCharImmediatelyPostOpenBlock = false;
	isCharImmediatelyPostCloseBlock = false;
	isCharImmediatelyPostTemplate = false;
//	previousBracketIsBroken = false;
	breakCurrentOneLineBlock = false;
	isInHorstmannRunIn = false;
	currentLineBeginsWithBracket = false;
	isPrependPostBlockEmptyLineRequested = false;
	isAppendPostBlockEmptyLineRequested = false;
	prependEmptyLine = false;
	appendOpeningBracket = false;
	foundClosingHeader = false;
	isImmediatelyPostHeader = false;
	isInHeader = false;
	isInCase = false;
	isJavaStaticConstructor = false;
}

/**
 * get the next formatted line.
 *
 * @return    formatted line.
 */

string ASFormatter::nextLine()
{
	const string *newHeader;
	bool isInVirginLine = isVirgin;
	isCharImmediatelyPostComment = false;
	isPreviousCharPostComment = false;
	isCharImmediatelyPostLineComment = false;
	isCharImmediatelyPostOpenBlock = false;
	isCharImmediatelyPostCloseBlock = false;
	isCharImmediatelyPostTemplate = false;
	traceLineNumber++;

	while (!isLineReady)
	{
		if (shouldReparseCurrentChar)
			shouldReparseCurrentChar = false;
		else if (!getNextChar())
		{
			breakLine();
			return beautify(readyFormattedLine);
		}
		else // stuff to do when reading a new character...
		{
			// make sure that a virgin '{' at the begining of the file will be treated as a block...
			if (isInVirginLine && currentChar == '{' && currentLineBeginsWithBracket)	// lineBeginsWith('{')
				previousCommandChar = '{';
			if (isInHorstmannRunIn)
				isInLineBreak = false;
			if (!isWhiteSpace(currentChar))
				isInHorstmannRunIn = false;
			isPreviousCharPostComment = isCharImmediatelyPostComment;
			isCharImmediatelyPostComment = false;
			isCharImmediatelyPostTemplate = false;
			isCharImmediatelyPostReturn = false;
			isCharImmediatelyPostOperator = false;
			isCharImmediatelyPostOpenBlock = false;
			isCharImmediatelyPostCloseBlock = false;
		}

//		if (inLineNumber >= 7)
//			int x = 1;

		if (shouldBreakLineAtNextChar && !isWhiteSpace(currentChar))
		{
			isInLineBreak = true;
			shouldBreakLineAtNextChar = false;
		}

		if (isInLineComment)
		{
			formatLineCommentBody();
			continue;
		}
		else if (isInComment)
		{
			formatCommentBody();
			continue;
		}

		// not in line comment or comment

		else if (isInQuote)
		{
			formatQuoteBody();
			continue;
		}

		if (isSequenceReached("//"))
		{
			formatLineCommentOpener();
			continue;
		}
		else if (isSequenceReached("/*"))
		{
			formatCommentOpener();
			continue;
		}
		else if (currentChar == '"' || currentChar == '\'')
		{
			formatQuoteOpener();
			continue;
		}
		// treat these preprocessor statements as a line comment
		else if (currentChar =='#')
		{
			if (isSequenceReached("#region")
			        || isSequenceReached("#endregion")
			        || isSequenceReached("#error")
			        || isSequenceReached("#warning"))
			{
				// check for horstmann run-in
				if (formattedLine[0] == '{')
				{
					isInLineBreak = true;
					isInHorstmannRunIn = false;
				}
				isInLineComment = true;
				appendCurrentChar();
				continue;
			}
		}

		if (isInPreprocessor)
		{
			appendCurrentChar();
			continue;
		}

		// handle white space - needed to simplify the rest.
		if (isWhiteSpace(currentChar))
		{
			appendCurrentChar();
			continue;
		}

		/* not in MIDDLE of quote or comment or white-space of any type ... */

		// check if in preprocessor
		// ** isInPreprocessor will be automatically reset at the begining
		//    of a new line in getnextChar()
		if (currentChar == '#')
		{
			isInPreprocessor = true;
			// check for horstmann run-in
			if (formattedLine[0] == '{')
			{
				isInLineBreak = true;
				isInHorstmannRunIn = false;
			}
			processPreprocessor();
			//  need to fall thru here to reset the variables
		}

		/* not in preprocessor ... */

		if (isImmediatelyPostComment)
		{
			isImmediatelyPostComment = false;
			isCharImmediatelyPostComment = true;
		}

		if (isImmediatelyPostLineComment)
		{
			isImmediatelyPostLineComment = false;
			isCharImmediatelyPostLineComment = true;
		}

		if (isImmediatelyPostReturn)
		{
			isImmediatelyPostReturn = false;
			isCharImmediatelyPostReturn = true;
		}

		if (isImmediatelyPostOperator)
		{
			isImmediatelyPostOperator = false;
			isCharImmediatelyPostOperator = true;
		}

		// reset isImmediatelyPostHeader information
		if (isImmediatelyPostHeader)
		{
			if (currentChar != '{' && shouldAddBrackets)
			{
				addBracketsToStatement();
				if (!shouldAddOneLineBrackets)
					breakCurrentOneLineBlock = true;
			}

			isImmediatelyPostHeader = false;

			// Make sure headers are broken from their succeeding blocks
			// (e.g.
			//     if (isFoo) DoBar();
			//  should become
			//     if (isFoo)
			//         DoBar;
			// )
			// But treat else if() as a special case which should not be broken!
			if (shouldBreakOneLineStatements
			        && isOkToBreakBlock(bracketTypeStack->back()))
			{
				// if may break 'else if()'s, then simply break the line
				if (shouldBreakElseIfs)
					isInLineBreak = true;
			}
		}

		if (passedSemicolon)    // need to break the formattedLine
		{
			passedSemicolon = false;
			if (parenStack->back() == 0 && currentChar != ';') // allow ;;
			{
				// does a one-line statement have ending comments?
				if (isBracketType(bracketTypeStack->back(), SINGLE_LINE_TYPE))
				{
					size_t blockEnd = currentLine.rfind(AS_CLOSE_BRACKET);
					assert(blockEnd != string::npos);
					// move ending comments to this formattedLine
					if (isBeforeAnyLineEndComment(blockEnd))
					{
						size_t commentStart = currentLine.find_first_not_of(" \t", blockEnd + 1);
						assert(commentStart != string::npos);
						assert((currentLine.compare(commentStart, 2, "//") == 0)
						       || (currentLine.compare(commentStart, 2, "/*") == 0));
						size_t commentLength = currentLine.length() - commentStart;
						int tabCount = getIndentLength();
						appendSpacePad();
						for (int i=1; i<tabCount; i++)
							formattedLine.append(1, ' ');
						formattedLine.append(currentLine, commentStart, commentLength);
						currentLine.erase(commentStart, commentLength);
					}
				}
				shouldReparseCurrentChar = true;
				isInLineBreak = true;
				if (needHeaderOpeningBracket)
				{
					isCharImmediatelyPostCloseBlock = true;
					needHeaderOpeningBracket = false;
				}
				continue;
			}
		}

		if (passedColon)
		{
			passedColon = false;
			if (parenStack->back() == 0 && !isBeforeAnyComment())
			{
				shouldReparseCurrentChar = true;
				isInLineBreak = true;
				continue;
			}
		}

		// Check if in template declaration, e.g. foo<bar> or foo<bar,fig>
		// If so, set isInTemplate to true
		if (!isInTemplate && currentChar == '<')
		{
			int maxTemplateDepth = 0;
			templateDepth = 0;
			for (size_t i = charNum;
			        i < currentLine.length();
			        i++)
			{
				char currentChar = currentLine[i];

				if (currentChar == '<')
				{
					templateDepth++;
					maxTemplateDepth++;
				}
				else if (currentChar == '>')
				{
					templateDepth--;
					if (templateDepth == 0)
					{
						// this is a template!
						isInTemplate = true;
						templateDepth = maxTemplateDepth;
						break;
					}
				}
				else if (currentLine.compare(i, 2, "&&") == 0
				         || currentLine.compare(i, 2, "||") == 0)
				{
					// this is not a template -> leave...
					isInTemplate = false;
					break;
				}
				else if (currentChar == ','       // comma,     e.g. A<int, char>
				         || currentChar == '&'    // reference, e.g. A<int&>
				         || currentChar == '*'    // pointer,   e.g. A<int*>
				         || currentChar == ':'    // ::,        e.g. std::string
				         || currentChar == '['    // []         e.g. string[]
				         || currentChar == ']'    // []         e.g. string[]
				         || currentChar == '('    // (...)      e.g. function definition
				         || currentChar == ')')   // (...)      e.g. function definition
				{
					continue;
				}
				else if (!isLegalNameChar(currentChar) && !isWhiteSpace(currentChar))
				{
					// this is not a template -> leave...
					isInTemplate = false;
					break;
				}
			}
		}

		// handle parenthesies
		if (currentChar == '(' || currentChar == '[' || (isInTemplate && currentChar == '<'))
		{
			parenStack->back()++;
			if (currentChar == '[')
				isInBlParen = true;
		}
		else if (currentChar == ')' || currentChar == ']' || (isInTemplate && currentChar == '>'))
		{
			parenStack->back()--;
			if (isInTemplate && currentChar == '>')
			{
				templateDepth--;
				if (templateDepth == 0)
				{
					isInTemplate = false;
					isCharImmediatelyPostTemplate = true;
				}
			}

			// check if this parenthesis closes a header, e.g. if (...), while (...)
			if (isInHeader && parenStack->back() == 0)
			{
				isInHeader = false;
				isImmediatelyPostHeader = true;
			}
			if (currentChar == ']')
				isInBlParen = false;
			if (currentChar == ')')
				foundCastOperator = false;
		}

		// handle brackets
		if (currentChar == '{' || currentChar == '}')
		{
			// if appendOpeningBracket this was already done for the original bracket
			if (currentChar == '{' && !appendOpeningBracket)
			{
				BracketType newBracketType = getBracketType();
				foundNamespaceHeader = false;
				foundClassHeader = false;
				foundStructHeader = false;
				foundInterfaceHeader = false;
				foundPreDefinitionHeader = false;
				foundPreCommandHeader = false;
				isInPotentialCalculation = false;
				isJavaStaticConstructor = false;
				needHeaderOpeningBracket = false;

				bracketTypeStack->push_back(newBracketType);
				preBracketHeaderStack->push_back(currentHeader);
				currentHeader = NULL;

				isPreviousBracketBlockRelated = !isBracketType(newBracketType, ARRAY_TYPE);
			}

			// this must be done before the bracketTypeStack is popped
			BracketType bracketType = bracketTypeStack->back();
			bool isOpeningArrayBracket = (isBracketType(bracketType, ARRAY_TYPE)
			                              && bracketTypeStack->size() >= 2
			                              && !isBracketType((*bracketTypeStack)[bracketTypeStack->size()-2], ARRAY_TYPE)
			                             );

			if (currentChar == '}')
			{
				// if a request has been made to append a post block empty line,
				// but the block exists immediately before a closing bracket,
				// then there is no need for the post block empty line.
				isAppendPostBlockEmptyLineRequested = false;

				if (bracketTypeStack->size() > 1)
				{
					previousBracketType = bracketTypeStack->back();
					bracketTypeStack->pop_back();
					isPreviousBracketBlockRelated = !isBracketType(bracketType, ARRAY_TYPE);
				}
				else
				{
					previousBracketType = NULL_TYPE;
					isPreviousBracketBlockRelated = false;
				}

				if (!preBracketHeaderStack->empty())
				{
					currentHeader = preBracketHeaderStack->back();
					preBracketHeaderStack->pop_back();
				}
				else
					currentHeader = NULL;

				if (isBracketType(bracketType, ARRAY_NIS_TYPE))
					clearNonInStatement = true;

			}

			// format brackets
			appendOpeningBracket = false;
			if (isBracketType(bracketType, ARRAY_TYPE))
			{
				formatArrayBrackets(bracketType, isOpeningArrayBracket);
			}
			else
			{
				if (currentChar == '{')
					formatOpeningBracket(bracketType);
				else
					formatClosingBracket(bracketType);
			}
			continue;
		}

		if ((((previousCommandChar == '{' && isPreviousBracketBlockRelated)
		        || ((previousCommandChar == '}'
		             && !isImmediatelyPostEmptyBlock
		             && isPreviousBracketBlockRelated
		             && !isPreviousCharPostComment       // Fixes wrongly appended newlines after '}' immediately after comments
		             && peekNextChar() != ' '
		             && !isBracketType(previousBracketType,  DEFINITION_TYPE))
		            && !isBracketType(bracketTypeStack->back(),  DEFINITION_TYPE)))
		        && isOkToBreakBlock(bracketTypeStack->back()))
		        // check for array
		        || (isBracketType(bracketTypeStack->back(), ARRAY_TYPE)
		            && !isBracketType(bracketTypeStack->back(), SINGLE_LINE_TYPE)
		            && isNonInStatementArray))
		{
			isCharImmediatelyPostOpenBlock = (previousCommandChar == '{');
			isCharImmediatelyPostCloseBlock = (previousCommandChar == '}');

			if (isCharImmediatelyPostOpenBlock
			        && !isCharImmediatelyPostComment
			        && !isCharImmediatelyPostLineComment)
			{
				previousCommandChar = ' ';

				if (bracketFormatMode == NONE_MODE)
				{
					if (shouldBreakOneLineBlocks
					        && isBracketType(bracketTypeStack->back(),  SINGLE_LINE_TYPE))
						isInLineBreak = true;
					else if (currentLineBeginsWithBracket)
						formatRunIn();
					else
						breakLine();
				}
				else if (bracketFormatMode == HORSTMANN_MODE
				         && currentChar != '#')
					formatRunIn();
				else
					isInLineBreak = true;
			}
			else if (isCharImmediatelyPostCloseBlock
			         && shouldBreakOneLineStatements
			         && (isLegalNameChar(currentChar) && currentChar != '.')
			         && !isCharImmediatelyPostComment)
			{
				previousCommandChar = ' ';
				breakCurrentOneLineBlock = false;
				isInLineBreak = true;
			}
		}

		// reset block handling flags
		isImmediatelyPostEmptyBlock = false;

		// look for headers
		bool isPotentialHeader = isCharPotentialHeader(currentLine, charNum);

		if (isPotentialHeader && !isInTemplate)
		{
			isNonParenHeader = false;
			foundClosingHeader = false;
			newHeader = findHeader(headers);

			if (newHeader != NULL)
			{
				char peekChar = ASBase::peekNextChar(currentLine, charNum + newHeader->length() - 1);

				// is not a header if part of a definition
				if (peekChar == ',' || peekChar == ')')
					newHeader = NULL;
				// the following accessor definitions are NOT headers
				// goto default; is NOT a header
				else if ((newHeader == &AS_GET || newHeader == &AS_SET || newHeader == &AS_DEFAULT)
				         && peekChar == ';')
				{
					newHeader = NULL;
				}
			}

			if (newHeader != NULL)
			{
				const string *previousHeader;

				// recognize closing headers of do..while, if..else, try..catch..finally
				if ((newHeader == &AS_ELSE && currentHeader == &AS_IF)
				        || (newHeader == &AS_WHILE && currentHeader == &AS_DO)
				        || (newHeader == &AS_CATCH && currentHeader == &AS_TRY)
				        || (newHeader == &AS_CATCH && currentHeader == &AS_CATCH)
				        || (newHeader == &AS_FINALLY && currentHeader == &AS_TRY)
				        || (newHeader == &AS_FINALLY && currentHeader == &AS_CATCH)
				        || (newHeader == &AS_SET && currentHeader == &AS_GET)
				        || (newHeader == &AS_REMOVE && currentHeader == &AS_ADD))
					foundClosingHeader = true;

				previousHeader = currentHeader;
				currentHeader = newHeader;
				needHeaderOpeningBracket = true;

				if (foundClosingHeader && previousNonWSChar == '}')
				{
					if (isOkToBreakBlock(bracketTypeStack->back()))
						isLineBreakBeforeClosingHeader();

					// get the adjustment for a comment following the closing header
					if (isInLineBreak)
						nextLineSpacePadNum = getNextLineCommentAdjustment();
					else
						spacePadNum = getCurrentLineCommentAdjustment();
				}

				// check if the found header is non-paren header
//				isNonParenHeader = (find(nonParenHeaders.begin(), nonParenHeaders.end(),
//				                         newHeader) != nonParenHeaders.end());
				isNonParenHeader = findHeader(nonParenHeaders) != NULL;

				// join 'else if' statements
				if (currentHeader == &AS_IF && previousHeader == &AS_ELSE && isInLineBreak && !shouldBreakElseIfs)
				{
					// 'else' must be last thing on the line, but must not be #else
					size_t start = formattedLine.length() >= 6 ? formattedLine.length()-6 : 0;
					if (formattedLine.find("else", start) != string::npos
					        && formattedLine.find("#else", start) == string::npos)
					{
						appendSpacePad();
						isInLineBreak = false;
					}
				}

				appendSequence(*currentHeader);
				goForward(currentHeader->length() - 1);
				// if a paren-header is found add a space after it, if needed
				// this checks currentLine, appendSpacePad() checks formattedLine
				// in C# 'catch' can be either a paren or non-paren header
				if (shouldPadHeader
				        && (!isNonParenHeader || (currentHeader == &AS_CATCH && peekNextChar() == '('))
				        && charNum < (int) currentLine.length() && !isWhiteSpace(currentLine[charNum+1]))
					appendSpacePad();

				// Signal that a header has been reached
				// *** But treat a closing while() (as in do...while)
				//     as if it were NOT a header since a closing while()
				//     should never have a block after it!
				if (!(foundClosingHeader && currentHeader == &AS_WHILE))
				{
					isInHeader = true;
					// in C# 'catch' and 'delegate' can be a paren or non-paren header
					if (isNonParenHeader && peekNextChar() != '(')
					{
						isImmediatelyPostHeader = true;
						isInHeader = false;
					}
				}

				if (shouldBreakBlocks
				        && isOkToBreakBlock(bracketTypeStack->back()))
				{
					if (previousHeader == NULL
					        && !foundClosingHeader
					        && !isCharImmediatelyPostOpenBlock
					        && !isImmediatelyPostCommentOnly)
					{
						isPrependPostBlockEmptyLineRequested = true;
					}

					if (currentHeader == &AS_ELSE
					        || currentHeader == &AS_CATCH
					        || currentHeader == &AS_FINALLY
					        || foundClosingHeader)
					{
						isPrependPostBlockEmptyLineRequested = false;
					}

					if (shouldBreakClosingHeaderBlocks
					        && isCharImmediatelyPostCloseBlock
					        && !isImmediatelyPostCommentOnly
					        && currentHeader != &AS_WHILE)    // closing do-while block
					{
						isPrependPostBlockEmptyLineRequested = true;
					}

				}

				continue;
			}
			else if ((newHeader = findHeader(preDefinitionHeaders)) != NULL
			         && parenStack->back() == 0)
			{
				if (newHeader == &AS_NAMESPACE)
					foundNamespaceHeader = true;
				if (newHeader == &AS_CLASS)
					foundClassHeader = true;
				if (newHeader == &AS_STRUCT)
					foundStructHeader = true;
				if (newHeader == &AS_INTERFACE)
					foundInterfaceHeader = true;
				foundPreDefinitionHeader = true;
				appendSequence(*newHeader);
				goForward(newHeader->length() - 1);

				continue;
			}
			else if ((newHeader = findHeader(preCommandHeaders)) != NULL)
			{
				if (!(*newHeader == AS_CONST && previousCommandChar != ')')) // 'const' member functions is a command bracket
					foundPreCommandHeader = true;
				appendSequence(*newHeader);
				goForward(newHeader->length() - 1);

				continue;
			}
			else if ((newHeader = findHeader(castOperators)) != NULL)
			{
				foundCastOperator = true;
				appendSequence(*newHeader);
				goForward(newHeader->length() - 1);
				continue;
			}
		}   // (isPotentialHeader &&  !isInTemplate)

		if (isInLineBreak)          // OK to break line here
			breakLine();

		if (previousNonWSChar == '}' || currentChar == ';')
		{
			if (shouldBreakOneLineStatements && currentChar == ';'
			        && (isOkToBreakBlock(bracketTypeStack->back()))
			   )
			{
				passedSemicolon = true;
			}

			// append post block empty line for unbracketed header
			if (shouldBreakBlocks && currentChar == ';' && currentHeader != NULL && parenStack->back() == 0)
			{
				isAppendPostBlockEmptyLineRequested = true;
			}

			// end of block if a closing bracket was found
			// or an opening bracket was not found (';' closes)
			if (currentChar != ';'
			        || (needHeaderOpeningBracket && parenStack->back() == 0))
				currentHeader = NULL;

			foundQuestionMark = false;
			foundNamespaceHeader = false;
			foundClassHeader = false;
			foundStructHeader = false;
			foundInterfaceHeader = false;
			foundPreDefinitionHeader = false;
			foundPreCommandHeader = false;
			foundCastOperator = false;
			isInPotentialCalculation = false;
			isSharpAccessor = false;
			isSharpDelegate = false;
			isInExtern = false;
			isInEnum = false;
			nonInStatementBracket = 0;
		}

		if (currentChar == ':' && shouldBreakOneLineStatements)
		{
			if (isInCase
			        && previousChar != ':'          // not part of '::'
			        && peekNextChar() != ':')       // not part of '::'
			{
				isInCase = false;
				passedColon = true;
			}
			else if (isCStyle()                     // for C/C++ only
			         && !foundQuestionMark          // not in a ... ? ... : ... sequence
			         && !foundPreDefinitionHeader   // not in a definition block (e.g. class foo : public bar
			         && previousCommandChar != ')'  // not immediately after closing paren of a method header, e.g. ASFormatter::ASFormatter(...) : ASBeautifier(...)
			         && previousChar != ':'         // not part of '::'
			         && peekNextChar() != ':'       // not part of '::'
			         && !isdigit(peekNextChar()))   // not a bit field
			{
				passedColon = true;
			}
		}

		if (currentChar == '?')
			foundQuestionMark = true;

		if (isPotentialHeader &&  !isInTemplate)
		{
			if (findKeyword(currentLine, charNum, AS_CASE)
			        || findKeyword(currentLine, charNum, AS_DEFAULT))
				isInCase = true;

			if (findKeyword(currentLine, charNum, AS_NEW))
				isInPotentialCalculation = false;

			if (findKeyword(currentLine, charNum, AS_RETURN))
			{
				isInPotentialCalculation = true;	// return is the same as an = sign
				isImmediatelyPostReturn = true;
			}

			if (findKeyword(currentLine, charNum, AS_OPERATOR))
				isImmediatelyPostOperator = true;

			if (isCStyle() && findKeyword(currentLine, charNum, AS_EXTERN))
				isInExtern = true;

			if (isCStyle() && findKeyword(currentLine, charNum, AS_ENUM))
				isInEnum = true;

			if (isJavaStyle()
			        && (findKeyword(currentLine, charNum, AS_STATIC)
			            && isNextCharOpeningBracket(charNum + 6)))
				isJavaStaticConstructor = true;

			if (isSharpStyle()
			        && (findKeyword(currentLine, charNum, AS_DELEGATE)
			            || findKeyword(currentLine, charNum, AS_UNCHECKED)))
				isSharpDelegate = true;

			// append the entire name
			string name = getCurrentWord(currentLine, charNum);
			appendSequence(name);
			goForward(name.length() - 1);

			continue;

		}   // (isPotentialHeader &&  !isInTemplate)

		// determine if this is a potential calculation

		bool isPotentialOperator = isCharPotentialOperator(currentChar);
		newHeader = NULL;

		if (isPotentialOperator)
		{
			newHeader = findOperator(operators);

			if (newHeader != NULL)
			{
				// correct mistake of two >> closing a template
				if (isInTemplate && (newHeader == &AS_GR_GR || newHeader == &AS_GR_GR_GR))
					newHeader = &AS_GR;

				if (!isInPotentialCalculation)
				{
					// must determine if newHeader is an assignment operator
					// do NOT use findOperator!!!
					if (find(assignmentOperators.begin(), assignmentOperators.end(), newHeader)
					        != assignmentOperators.end())
					{
						char peekedChar = peekNextChar();
						isInPotentialCalculation = (!(newHeader == &AS_EQUAL && peekedChar == '*')
						                            && !(newHeader == &AS_EQUAL && peekedChar == '&'));
					}
				}
			}
		}

		// process pointers and references
		// check new header to elimnate things like '&&' sequence
		if (isCStyle()
		        && (newHeader == &AS_MULT || newHeader == &AS_BIT_AND)
		        && isPointerOrReference()
		        && !isDereferenceOrAddressOf())
		{
			formatPointerOrReference();
			continue;
		}

		if (shouldPadOperators && newHeader != NULL)
		{
			padOperators(newHeader);
			continue;
		}

		// pad commas and semi-colons
		if (currentChar == ';'
		        || (currentChar == ',' && shouldPadOperators))
		{
			char nextChar = ' ';
			if (charNum + 1 < (int) currentLine.length())
				nextChar = currentLine[charNum+1];
			if (!isWhiteSpace(nextChar)
			        && nextChar != '}'
			        && nextChar != ')'
			        && nextChar != ']'
			        && nextChar != '>'
			        && nextChar != ';'
			        && !isBeforeAnyComment()
			        /* && !(isBracketType(bracketTypeStack->back(), ARRAY_TYPE)) */
			   )
			{
				appendCurrentChar();
				appendSpaceAfter();
				continue;
			}
		}

		if ((shouldPadParensOutside || shouldPadParensInside || shouldUnPadParens)
		        && (currentChar == '(' || currentChar == ')'))
		{
			padParens();
			continue;
		}

		appendCurrentChar();
	}   // end of while loop  *  end of while loop  *  end of while loop  *  end of while loop

	// return a beautified (i.e. correctly indented) line.

	string beautifiedLine;
	size_t readyFormattedLineLength = trim(readyFormattedLine).length();

	if (prependEmptyLine                // prepend a blank line before this formatted line
	        && readyFormattedLineLength > 0
	        && previousReadyFormattedLineLength > 0)
	{
		isLineReady = true;             // signal a waiting readyFormattedLine
		beautifiedLine = beautify("");
		previousReadyFormattedLineLength = 0;
	}
	else                                // format the current formatted line
	{
		isLineReady = false;
		horstmannIndentInStatement = horstmannIndentChars;
		beautifiedLine = beautify(readyFormattedLine);
		previousReadyFormattedLineLength = readyFormattedLineLength;
		horstmannIndentChars = 0;
		lineCommentNoBeautify = lineCommentNoIndent;
		lineCommentNoIndent = false;
		if (clearNonInStatement)
		{
			isNonInStatementArray = false;
			clearNonInStatement = false;
		}
	}

	prependEmptyLine = false;
	enhancer->enhance(beautifiedLine);
	return beautifiedLine;
}


/**
 * check if there are any indented lines ready to be read by nextLine()
 *
 * @return    are there any indented lines ready?
 */
bool ASFormatter::hasMoreLines() const
{
	return !endOfCodeReached;
}

/**
 * comparison function for BracketType enum
 */
bool ASFormatter::isBracketType(BracketType a, BracketType b) const
{
	return ((a & b) == b);
}

/**
 * get indent manually set flag
 */
bool ASFormatter::getIndentManuallySet()
{
	return isIndentManuallySet;
}

/**
 * get mode manually set flag
 */
bool ASFormatter::getModeManuallySet()
{
	return isModeManuallySet;
}

/**
 * set the formatting style.
 *
 * @param mode         the formatting style.
 */
void ASFormatter::setFormattingStyle(FormatStyle style)
{
	formattingStyle = style;
}

/**
 * set the add brackets mode.
 * options:
 *    true     brackets added to headers for single line statements.
 *    false    brackets NOT added to headers for single line statements.
 *
 * @param mode         the bracket formatting mode.
 */
void ASFormatter::setAddBracketsMode(bool state)
{
	shouldAddBrackets = state;
}

/**
 * set the add one line brackets mode.
 * options:
 *    true     one line brackets added to headers for single line statements.
 *    false    one line brackets NOT added to headers for single line statements.
 *
 * @param mode         the bracket formatting mode.
 */
void ASFormatter::setAddOneLineBracketsMode(bool state)
{
	shouldAddBrackets = state;
	shouldAddOneLineBrackets = state;
}

/**
 * set the bracket formatting mode.
 * options:
 *
 * @param mode         the bracket formatting mode.
 */
void ASFormatter::setBracketFormatMode(BracketMode mode)
{
	bracketFormatMode = mode;
}

/**
 * set closing header bracket breaking mode
 * options:
 *    true     brackets just before closing headers (e.g. 'else', 'catch')
 *             will be broken, even if standard brackets are attached.
 *    false    closing header brackets will be treated as standard brackets.
 *
 * @param state         the closing header bracket breaking mode.
 */
void ASFormatter::setBreakClosingHeaderBracketsMode(bool state)
{
	shouldBreakClosingHeaderBrackets = state;
}

/**
 * set 'else if()' breaking mode
 * options:
 *    true     'else' headers will be broken from their succeeding 'if' headers.
 *    false    'else' headers will be attached to their succeeding 'if' headers.
 *
 * @param state         the 'else if()' breaking mode.
 */
void ASFormatter::setBreakElseIfsMode(bool state)
{
	shouldBreakElseIfs = state;
}

/**
 * set indent manually set flag
 */
void ASFormatter::setIndentManuallySet(bool state)
{
	isIndentManuallySet = state;
}

/**
 * set mode manually set flag
 */
void ASFormatter::setModeManuallySet(bool state)
{
	isModeManuallySet = state;
}

/**
 * set operator padding mode.
 * options:
 *    true     statement operators will be padded with spaces around them.
 *    false    statement operators will not be padded.
 *
 * @param state         the padding mode.
 */
void ASFormatter::setOperatorPaddingMode(bool state)
{
	shouldPadOperators = state;
}

/**
 * set parenthesis outside padding mode.
 * options:
 *    true     statement parenthesiss will be padded with spaces around them.
 *    false    statement parenthesiss will not be padded.
 *
 * @param state         the padding mode.
 */
void ASFormatter::setParensOutsidePaddingMode(bool state)
{
	shouldPadParensOutside = state;
}

/**
 * set parenthesis inside padding mode.
 * options:
 *    true     statement parenthesis will be padded with spaces around them.
 *    false    statement parenthesis will not be padded.
 *
 * @param state         the padding mode.
 */
void ASFormatter::setParensInsidePaddingMode(bool state)
{
	shouldPadParensInside = state;
}

/**
 * set header padding mode.
 * options:
 *    true     headers will be padded with spaces around them.
 *    false    headers will not be padded.
 *
 * @param state         the padding mode.
 */
void ASFormatter::setParensHeaderPaddingMode(bool state)
{
	shouldPadHeader = state;
}

/**
 * set parenthesis unpadding mode.
 * options:
 *    true     statement parenthesis will be unpadded with spaces removed around them.
 *    false    statement parenthesis will not be unpadded.
 *
 * @param state         the padding mode.
 */
void ASFormatter::setParensUnPaddingMode(bool state)
{
	shouldUnPadParens = state;
}

/**
 * set option to break/not break one-line blocks
 *
 * @param state        true = break, false = don't break.
 */
void ASFormatter::setBreakOneLineBlocksMode(bool state)
{
	shouldBreakOneLineBlocks = state;
}

/**
 * set option to break/not break lines consisting of multiple statements.
 *
 * @param state        true = break, false = don't break.
 */
void ASFormatter::setSingleStatementsMode(bool state)
{
	shouldBreakOneLineStatements = state;
}

/**
 * set option to convert tabs to spaces.
 *
 * @param state        true = convert, false = don't convert.
 */
void ASFormatter::setTabSpaceConversionMode(bool state)
{
	shouldConvertTabs = state;
}


/**
 * set option to break unrelated blocks of code with empty lines.
 *
 * @param state        true = convert, false = don't convert.
 */
void ASFormatter::setBreakBlocksMode(bool state)
{
	shouldBreakBlocks = state;
}

/**
 * set option to break closing header blocks of code (such as 'else', 'catch', ...) with empty lines.
 *
 * @param state        true = convert, false = don't convert.
 */
void ASFormatter::setBreakClosingHeaderBlocksMode(bool state)
{
	shouldBreakClosingHeaderBlocks = state;
}

/**
 * set option to delete empty lines.
 *
 * @param state        true = delete, false = don't delete.
 */
void ASFormatter::setDeleteEmptyLinesMode(bool state)
{
	shouldDeleteEmptyLines = state;
}

/**
 * set the pointer alignment.
 * options:
 *
 * @param alignment    the pointer alignment.
 */
void ASFormatter::setPointerAlignment(PointerAlign alignment)
{
	pointerAlignment = alignment;
}

/**
 * jump over several characters.
 *
 * @param i       the number of characters to jump over.
 */
void ASFormatter::goForward(int i)
{
	while (--i >= 0)
		getNextChar();
}

/**
 * peek at the next unread character.
 *
 * @return     the next unread character.
 */
char ASFormatter::peekNextChar() const
{
	char ch = ' ';
	size_t peekNum = currentLine.find_first_not_of(" \t", charNum + 1);

	if (peekNum == string::npos)
		return ch;

	ch = currentLine[peekNum];

	return ch;
}

/**
 * check if current placement is before a comment
 *
 * @return     is before a comment.
 */
bool ASFormatter::isBeforeComment() const
{
	bool foundComment = false;
	size_t peekNum = currentLine.find_first_not_of(" \t", charNum + 1);

	if (peekNum == string::npos)
		return foundComment;

	foundComment = (currentLine.compare(peekNum, 2, "/*") == 0);

	return foundComment;
}

/**
 * check if current placement is before a comment or line-comment
 *
 * @return     is before a comment or line-comment.
 */
bool ASFormatter::isBeforeAnyComment() const
{
	bool foundComment = false;
	size_t peekNum = currentLine.find_first_not_of(" \t", charNum + 1);

	if (peekNum == string::npos)
		return foundComment;

	foundComment = (currentLine.compare(peekNum, 2, "/*") == 0
	                || currentLine.compare(peekNum, 2, "//") == 0);

	return foundComment;
}

/**
 * check if current placement is before a comment or line-comment
 * if a block comment it must be at the end of the line
 *
 * @return     is before a comment or line-comment.
 */
bool ASFormatter::isBeforeAnyLineEndComment(int startPos) const
{
	bool foundLineEndComment = false;
	size_t peekNum = currentLine.find_first_not_of(" \t", startPos + 1);

	if (peekNum != string::npos)
	{
		if (currentLine.compare(peekNum, 2, "//") == 0)
			foundLineEndComment = true;
		else if (currentLine.compare(peekNum, 2, "/*") == 0)
		{
			// comment must be closed on this line with nothing after it
			size_t endNum = currentLine.find("*/", peekNum + 2);
			if (endNum != string::npos)
			{
				size_t nextChar = currentLine.find_first_not_of(" \t", endNum + 2);
				if (nextChar == string::npos)
					foundLineEndComment = true;
			}
		}
	}
	return foundLineEndComment;
}

/**
 * check if current placement is before a comment followed by a line-comment
 *
 * @return     is before a multiple line-end comment.
 */
bool ASFormatter::isBeforeMultipleLineEndComments(int startPos) const
{
	bool foundMultipleLineEndComment = false;
	size_t peekNum = currentLine.find_first_not_of(" \t", startPos + 1);

	if (peekNum != string::npos)
	{
		if (currentLine.compare(peekNum, 2, "/*") == 0)
		{
			// comment must be closed on this line with nothing after it
			size_t endNum = currentLine.find("*/", peekNum + 2);
			if (endNum != string::npos)
			{
				size_t nextChar = currentLine.find_first_not_of(" \t", endNum + 2);
				if (nextChar != string::npos
				        && currentLine.compare(nextChar, 2, "//") == 0)
					foundMultipleLineEndComment = true;
			}
		}
	}
	return foundMultipleLineEndComment;
}


/**
 * get the next character, increasing the current placement in the process.
 * the new character is inserted into the variable currentChar.
 *
 * @return   whether succeded to recieve the new character.
 */
bool ASFormatter::getNextChar()
{
	isInLineBreak = false;
	previousChar = currentChar;

	if (!isWhiteSpace(currentChar))
	{
		previousNonWSChar = currentChar;
		if (!isInComment && !isInLineComment && !isInQuote
		        && !isImmediatelyPostComment
		        && !isImmediatelyPostLineComment
		        && !isInPreprocessor
		        && !isSequenceReached("/*")
		        && !isSequenceReached("//"))
			previousCommandChar = currentChar;
	}

	if (charNum + 1 < (int) currentLine.length()
	        && (!isWhiteSpace(peekNextChar()) || isInComment || isInLineComment))
	{
		currentChar = currentLine[++charNum];

		if (shouldConvertTabs && currentChar == '\t')
			convertTabToSpaces();

		return true;
	}

	// end of line has been reached
	return getNextLine();
}

/**
 * get the next line of input, increasing the current placement in the process.
 *
 * @param sequence         the sequence to append.
 * @return   whether succeded in reading the next line.
 */
bool ASFormatter::getNextLine(bool emptyLineWasDeleted /*false*/)
{
	if (sourceIterator->hasMoreLines())
	{
		if (appendOpeningBracket)
			currentLine = "{";		// append bracket that was removed from the previous line
		else
			currentLine = sourceIterator->nextLine(emptyLineWasDeleted);
		// reset variables for new line
//		spacePadNum = 0;
//		nextLineSpacePadNum = 0;
		inLineNumber++;
		isInCase = false;
		isInQuoteContinuation = isInVerbatimQuote | haveLineContinuationChar;
		haveLineContinuationChar= false;
		isImmediatelyPostEmptyLine = lineIsEmpty;
		previousChar = ' ';

		if (currentLine.length() == 0)
		{
			currentLine = string(" ");        // a null is inserted if this is not done
		}

		// unless reading in the first line of the file, break a new line.
		if (!isVirgin)
			isInLineBreak = true;
		else
			isVirgin = false;

		// check if is in preprocessor before line trimming
		// a blank line after a \ will remove the flag
		isImmediatelyPostPreprocessor = isInPreprocessor;
		if (previousNonWSChar != '\\'
		        || isEmptyLine(currentLine))
			isInPreprocessor = false;

		initNewLine();
		currentChar = currentLine[charNum];
		if (isInHorstmannRunIn && previousNonWSChar == '{')
			isInLineBreak = false;
		isInHorstmannRunIn = false;

		if (shouldConvertTabs && currentChar == '\t')
			convertTabToSpaces();

		// check for an empty line inside a command bracket.
		// if yes then read the next line (calls getNextLine recursively).
		// must be after initNewLine.
		if (shouldDeleteEmptyLines
		        && lineIsEmpty
		        && isBracketType((*bracketTypeStack)[bracketTypeStack->size()-1], COMMAND_TYPE))
		{
			// but do NOT delete an empty line between comments if blocks are being broken
			if (!(shouldBreakBlocks || shouldBreakClosingHeaderBlocks)
			        || !isImmediatelyPostCommentOnly
			        || !commentAndHeaderFollows())
			{
				isInPreprocessor = isImmediatelyPostPreprocessor;  // restore isInPreprocessor
				lineIsEmpty = false;
				return getNextLine(true);
			}
		}

		return true;
	}
	else
	{
		endOfCodeReached = true;
		return false;
	}
}

/**
 * jump over the leading white space in the current line,
 * IF the line does not begin a comment or is in a preprocessor definition.
 */
void ASFormatter::initNewLine()
{
	size_t len = currentLine.length();
	size_t indent = getIndentLength();
	charNum = 0;

	if (isInPreprocessor || isInQuoteContinuation)
		return;

	// trim comment line
	if (isInComment)
	{
		// the continuation lines must be adjusted so the leading spaces
		// is equivalent to the opening comment
		if (noTrimCommentContinuation)
			leadingSpaces = tabIncrementIn = 0;
		if (leadingSpaces > 0 && len > 0)
		{
			size_t j;
			size_t commentIncrementIn = 0;
			for (j = 0; (j < len) && (j + commentIncrementIn < leadingSpaces); j++)
			{
				if (!isWhiteSpace(currentLine[j]))		// don't delete any text
				{
					j = 0;
					commentIncrementIn = tabIncrementIn;
					break;
				}
				if (currentLine[j] == '\t')
					commentIncrementIn += indent - 1 - ((commentIncrementIn + j) % indent);
			}

			if ((int) commentIncrementIn == tabIncrementIn)
				charNum = j;
			else
			{
				// build a new line with the equivalent leading chars
				string newLine;
				int leadingChars = 0;
				if ((int) leadingSpaces > tabIncrementIn)
					leadingChars = leadingSpaces - tabIncrementIn;
				newLine.append(leadingChars, ' ');
				newLine.append(currentLine, j, len-j);
				currentLine = newLine;
				charNum = leadingChars;
			}
			if (j >= len)
				charNum = 0;
		}
		return;
	}

	// compute leading spaces
	isImmediatelyPostCommentOnly = lineIsLineCommentOnly || lineEndsInCommentOnly;
	lineIsLineCommentOnly = false;
	lineEndsInCommentOnly = false;
	doesLineStartComment = false;
	currentLineBeginsWithBracket = false;
	lineIsEmpty = false;
	currentLineBracketNum = string::npos;
	tabIncrementIn = 0;

	for (charNum = 0; isWhiteSpace(currentLine[charNum]) && charNum + 1 < (int) len; charNum++)
	{
		if (currentLine[charNum] == '\t')
			tabIncrementIn += indent - 1 - ((tabIncrementIn + charNum) % indent);
	}
	leadingSpaces = charNum + tabIncrementIn;

	if (isSequenceReached("/*"))
	{
		doesLineStartComment = true;
	}
	else if (isSequenceReached("//"))
	{
		lineIsLineCommentOnly = true;
	}
	else if (isSequenceReached("{"))
	{
		currentLineBeginsWithBracket = true;
		currentLineBracketNum = charNum;
		size_t firstText = currentLine.find_first_not_of(" \t", charNum + 1);
		if (firstText != string::npos)
		{
			if (currentLine.compare(firstText, 2, "//") == 0)
				lineIsLineCommentOnly = true;
			else if (currentLine.compare(firstText, 2, "/*") == 0)
			{
				doesLineStartComment = true;
				// get the extra adjustment
				size_t j;
				for (j = charNum + 1; isWhiteSpace(currentLine[j]) && j < firstText; j++)
				{
					if (currentLine[j] == '\t')
						tabIncrementIn += indent - 1 - ((tabIncrementIn + j) % indent);
				}
				leadingSpaces = j + tabIncrementIn;
			}
		}
	}
	else if (isWhiteSpace(currentLine[charNum]) && !(charNum + 1 < (int) currentLine.length()))
	{
		lineIsEmpty = true;
	}
}

/**
 * append a string sequence to the current formatted line.
 * Unless disabled (via canBreakLine == false), first check if a
 * line-break has been registered, and if so break the
 * formatted line, and only then append the sequence into
 * the next formatted line.
 *
 * @param sequence         the sequence to append.
 * @param canBreakLine     if true, a registered line-break
 */
void ASFormatter::appendSequence(const string &sequence, bool canBreakLine)
{
	if (canBreakLine && isInLineBreak)
		breakLine();
	formattedLine.append(sequence);
}

/**
 * append a space to the current formattedline, UNLESS the
 * last character is already a white-space character.
 */
void ASFormatter::appendSpacePad()
{
	int len = formattedLine.length();
	if (len > 0 && !isWhiteSpace(formattedLine[len-1]))
	{
		formattedLine.append(1, ' ');
		spacePadNum++;
	}
}

/**
 * append a space to the current formattedline, UNLESS the
 * next character is already a white-space character.
 */
void ASFormatter::appendSpaceAfter()
{
	int len = currentLine.length();
	if (charNum + 1 < len && !isWhiteSpace(currentLine[charNum+1]))
	{
		formattedLine.append(1, ' ');
		spacePadNum++;
	}
}

/**
 * register a line break for the formatted line.
 */
void ASFormatter::breakLine()
{
	isLineReady = true;
	isInLineBreak = false;
	spacePadNum = nextLineSpacePadNum;
	nextLineSpacePadNum = 0;
	formattedLineCommentNum = string::npos;

	// queue an empty line prepend request if one exists
	prependEmptyLine = isPrependPostBlockEmptyLineRequested;

	readyFormattedLine =  formattedLine;
	if (isAppendPostBlockEmptyLineRequested)
	{
		isAppendPostBlockEmptyLineRequested = false;
		isPrependPostBlockEmptyLineRequested = true;
	}
	else
	{
		isPrependPostBlockEmptyLineRequested = false;
	}

	formattedLine = "";
}

/**
 * check if the currently reached open-bracket (i.e. '{')
 * opens a:
 * - a definition type block (such as a class or namespace),
 * - a command block (such as a method block)
 * - a static array
 * this method takes for granted that the current character
 * is an opening bracket.
 *
 * @return    the type of the opened block.
 */
BracketType ASFormatter::getBracketType()
{
	assert(currentChar == '{');

	BracketType returnVal;

	if ((previousNonWSChar == '='
	        || isBracketType(bracketTypeStack->back(),  ARRAY_TYPE))
	        && previousCommandChar != ')')
		returnVal = ARRAY_TYPE;
	else if (foundPreDefinitionHeader)
	{
		returnVal = DEFINITION_TYPE;
		if (foundNamespaceHeader)
			returnVal = (BracketType)(returnVal | NAMESPACE_TYPE);
		else if (foundClassHeader)
			returnVal = (BracketType)(returnVal | CLASS_TYPE);
		else if (foundStructHeader)
			returnVal = (BracketType)(returnVal | STRUCT_TYPE);
		else if (foundInterfaceHeader)
			returnVal = (BracketType)(returnVal | INTERFACE_TYPE);
	}
	else
	{
		bool isCommandType = (foundPreCommandHeader
		                      || (currentHeader != NULL && isNonParenHeader)
		                      || (previousCommandChar == ')')
		                      || (previousCommandChar == ':' && !foundQuestionMark)
		                      || (previousCommandChar == ';')
		                      || ((previousCommandChar == '{' ||  previousCommandChar == '}')
		                          && isPreviousBracketBlockRelated)
		                      || isJavaStaticConstructor
		                      || isSharpDelegate);

		// C# methods containing 'get', 'set', 'add', and 'remove' do NOT end with parens
		if (!isCommandType && isSharpStyle() && isNextWordSharpNonParenHeader(charNum + 1))
		{
			isCommandType = true;
			isSharpAccessor = true;
		}

		if (!isCommandType && isInExtern)
			returnVal = EXTERN_TYPE;
		else
			returnVal = (isCommandType ? COMMAND_TYPE : ARRAY_TYPE);
	}

	if (isOneLineBlockReached())
		returnVal = (BracketType)(returnVal | SINGLE_LINE_TYPE);

	if (isBracketType(returnVal, ARRAY_TYPE)
	        && isNonInStatementArrayBracket())
	{
		returnVal = (BracketType)(returnVal | ARRAY_NIS_TYPE);
		isNonInStatementArray = true;
		nonInStatementBracket = formattedLine.length() - 1;
	}

	return returnVal;
}

/**
 * check if a line is empty
 *
 * @return        whether line is empty
 */
bool ASFormatter::isEmptyLine(const string &line) const
{
	return line.find_first_not_of(" \t") == string::npos;
}

/**
 * Check if the currently reached  '*' or '&' character is
 * a pointer-or-reference symbol, or another operator.
 * A pointer dereference (*) or an "address of" character (&)
 * counts as a pointer or reference because it is not an
 * arithmetic operator.
 *
 * @return        whether current character is a reference-or-pointer
 */
bool ASFormatter::isPointerOrReference() const
{
	assert(currentChar == '*' || currentChar == '&');

	if (!isCStyle())
		return false;

	if (previousNonWSChar == '=' || isCharImmediatelyPostReturn)
		return true;

	if (currentHeader == &AS_CATCH)
		return true;

	// get the last legal word (may be a number)
	string lastWord = getPreviousWord(currentLine, charNum);
	if (lastWord.empty())
		lastWord[0] = ' ';
	char nextChar = peekNextChar();

	// check for preceding or following numeric values
	if (isdigit(lastWord[0])
	        || isdigit(nextChar))
		return false;

	// checks on other chars
	if (isLegalNameChar(lastWord[0])
	        && isLegalNameChar(nextChar)
	        && parenStack->back() > 0)
	{
		// if followed by an assignment it is a pointer or reference
		size_t nextNum = currentLine.find_first_of("=;)", charNum + 1);
		if (nextNum != string::npos && currentLine[nextNum] == '=')
			return true;

		// if a function definition it is a pointer or reference
		// otherwise it is an arithmetic operator
		if (!isBracketType(bracketTypeStack->back(), COMMAND_TYPE))
			return true;
		else
			return false;
	}

	if (nextChar == '-'
	        || nextChar == '+')
	{
		size_t nextNum = currentLine.find_first_not_of(" \t", charNum + 1);
		if (nextNum != string::npos)
		{
			if (currentLine.compare(nextNum, 2, "++") != 0
			        && currentLine.compare(nextNum, 2, "--") != 0)
				return false;
		}
	}

	bool isPR = (!isInPotentialCalculation
	             || isBracketType(bracketTypeStack->back(), DEFINITION_TYPE)
	             || (!isLegalNameChar(previousNonWSChar)
	                 && !(previousNonWSChar == ')' && nextChar == '(')
	                 && !(previousNonWSChar == ')' && currentChar == '*')
	                 && previousNonWSChar != ']')
	            );

	if (!isPR)
	{
		isPR |= (!isWhiteSpace(nextChar)
		         && nextChar != '-'
		         && nextChar != '('
		         && nextChar != '['
		         && !isLegalNameChar(nextChar));
	}

	return isPR;
}

/**
 * Check if the currently reached  '*' or '&' character is
 * a dereferenced pointer or "address of" symbol.
 * NOTE: this MUST be a pointer or reference as determined by
 * the function isPointerOrReference().
 *
 * @return        whether current character is a dereference or address of
 */
bool ASFormatter::isDereferenceOrAddressOf() const
{
	assert(isPointerOrReference());

	if (previousNonWSChar == '='
	        || previousNonWSChar == '.'
	        || previousNonWSChar == '>'
	        || previousNonWSChar == '<'
	        || isCharImmediatelyPostReturn)
		return true;

	// check for **
	if (currentChar == '*'
	        && (int) currentLine.length() > charNum
	        && currentLine[charNum+1] == '*'
	        && previousNonWSChar != '('
	        && (int) currentLine.length() > charNum + 1
	        && currentLine[charNum+2] != ')')
		return false;

	// check first char on the line
	if (charNum == (int) currentLine.find_first_not_of(" \t"))
		return true;

	string lastWord = getPreviousWord(currentLine, charNum);
	if (lastWord == "else" || lastWord == "delete")
		return true;

	bool isDA = (!(isLegalNameChar(previousNonWSChar) || previousNonWSChar == '>')
	             || !isLegalNameChar(peekNextChar())
	             || peekNextChar() == ')'
	             || (ispunct(previousNonWSChar) && previousNonWSChar != '.')
	             || isCharImmediatelyPostReturn);

	return isDA;
}

/**
 * Check if the currently reached  '*' or '&' character is
 * centered with one space on each side.
 * Only spaces are checked, not tabs.
 * If true then a space wil be deleted on the output.
 *
 * @return        whether current character is centered.
 */
bool ASFormatter::isPointerOrReferenceCentered() const
{
	assert(currentLine[charNum] == '*' || currentLine[charNum] == '&');

	int prNum = charNum;
	int lineLength = (int) currentLine.length();
	// check space before
	if (prNum < 1
	        || currentLine[prNum-1] != ' ')
		return false;

	// check no space before that
	if (prNum < 2
	        || currentLine[prNum-2] == ' ')
		return false;

	// check for **
	if (prNum + 1 < lineLength
	        && currentLine[prNum+1] == '*')
		prNum++;

	// check space after
	if (prNum + 1 < lineLength
	        && currentLine[prNum+1] != ' ')
		return false;

	// check no space after that
	if (prNum + 2 < lineLength
	        && currentLine[prNum+2] == ' ')
		return false;

	return true;
}

/**
 * check if the currently reached '+' or '-' character is a unary operator
 * this method takes for granted that the current character
 * is a '+' or '-'.
 *
 * @return        whether the current '+' or '-' is a unary operator.
 */
bool ASFormatter::isUnaryOperator() const
{
	assert(currentChar == '+' || currentChar == '-');

	return ((isCharImmediatelyPostReturn || !isLegalNameChar(previousCommandChar))
	        && previousCommandChar != '.'
	        && previousCommandChar != '\"'
	        && previousCommandChar != '\''
	        && previousCommandChar != ')'
	        && previousCommandChar != ']');
}


/**
 * check if the currently reached '+' or '-' character is
 * part of an exponent, i.e. 0.2E-5.
 *
 * this method takes for granted that the current character
 * is a '+' or '-'.
 *
 * @return        whether the current '+' or '-' is in an exponent.
 */
bool ASFormatter::isInExponent() const
{
	assert(currentChar == '+' || currentChar == '-');

	int formattedLineLength = formattedLine.length();
	if (formattedLineLength >= 2)
	{
		char prevPrevFormattedChar = formattedLine[formattedLineLength - 2];
		char prevFormattedChar = formattedLine[formattedLineLength - 1];

		return ((prevFormattedChar == 'e' || prevFormattedChar == 'E')
		        && (prevPrevFormattedChar == '.' || isdigit(prevPrevFormattedChar)));
	}
	else
		return false;
}

/**
 * check if an array bracket should NOT have an in-statement indent
 *
 * @return        the array is non in-statement
 */
bool ASFormatter::isNonInStatementArrayBracket() const
{
	bool returnVal = false;

	// if this opening bracket begins the line there will be no inStatement indent
	if (currentLineBeginsWithBracket && charNum == (int) currentLineBracketNum)
		returnVal = true;
	// if an opening bracket ends the line there will be no inStatement indent
	char nextChar = peekNextChar();
	if (isWhiteSpace(nextChar)
	        || isBeforeAnyLineEndComment(charNum)
	        || nextChar == '{')
		returnVal = true;

	// Java "new Type [] {...}" IS an inStatement indent
	if (isJavaStyle() && previousNonWSChar == ']')
		returnVal = false;

	// trace
	//if (isNonInStatementArray)
	//	cout << traceLineNumber << " " << 'x' << endl;
	//else
	//	cout << traceLineNumber << " " << ' ' << endl

	return returnVal;
}

/**
 * check if a one-line bracket has been reached,
 * i.e. if the currently reached '{' character is closed
 * with a complimentry '}' elsewhere on the current line,
 *.
 * @return        has a one-line bracket been reached?
 */
bool ASFormatter::isOneLineBlockReached() const
{
	bool isInComment = false;
	bool isInQuote = false;
	int bracketCount = 1;
	int currentLineLength = currentLine.length();
	char quoteChar = ' ';

	for (int i = charNum + 1; i < currentLineLength; ++i)
	{
		char ch = currentLine[i];

		if (isInComment)
		{
			if (currentLine.compare(i, 2, "*/") == 0)
			{
				isInComment = false;
				++i;
			}
			continue;
		}

		if (ch == '\\')
		{
			++i;
			continue;
		}

		if (isInQuote)
		{
			if (ch == quoteChar)
				isInQuote = false;
			continue;
		}

		if (ch == '"' || ch == '\'')
		{
			isInQuote = true;
			quoteChar = ch;
			continue;
		}

		if (currentLine.compare(i, 2, "//") == 0)
			break;

		if (currentLine.compare(i, 2, "/*") == 0)
		{
			isInComment = true;
			++i;
			continue;
		}

		if (ch == '{')
			++bracketCount;
		else if (ch == '}')
			--bracketCount;

		if (bracketCount == 0)
			return true;
	}

	return false;
}

/**
 * check if a line begins with the specified character
 * i.e. if the current line begins with a open bracket.
 *
 * @return        true or false
 */
/*
bool ASFormatter::lineBeginsWith(char charToCheck) const
{
	bool beginsWith = false;
	size_t i = currentLine.find_first_not_of(" \t");

	if (i != string::npos)
		if (currentLine[i] == charToCheck && (int) i == charNum)
			beginsWith = true;

	return beginsWith;
}
*/
/**
 * peek at the next word to determine if it is a C# non-paren header.
 * will look ahead in the input file if necessary.
 *
 * @param       char position on currentLine to start the search
 * @return      true if the next word is get or set.
 */
bool ASFormatter::isNextWordSharpNonParenHeader(int startChar) const
{
	// look ahead to find the next non-comment text
	string nextText = peekNextText(currentLine.substr(startChar));
	if (nextText.length() == 0)
		return false;
	if (nextText[0] == '[')
		return true;
	if (!isCharPotentialHeader(nextText, 0))
		return false;
	if (findKeyword(nextText, 0, AS_GET) || findKeyword(nextText, 0, AS_SET)
	        || findKeyword(nextText, 0, AS_ADD) || findKeyword(nextText, 0, AS_REMOVE))
		return true;
	return false;
}

/**
 * peek at the next char to determine if it is an opening bracket.
 * will look ahead in the input file if necessary.
 * this determines a java static constructor.
 *
 * @param       char position on currentLine to start the search
 * @return      true if the next word is an opening bracket.
 */
bool ASFormatter::isNextCharOpeningBracket(int startChar) const
{
	bool retVal = false;
	string nextText = peekNextText(currentLine.substr(startChar));
	if (nextText.compare(0, 1, "{") == 0)
		retVal = true;
	return retVal;
}

/**
 * get the next non-whitespace substring on following lines, bypassing all comments.
 *
 * @param   the first line to check
 * @return  the next non-whitespace substring.
 */
string ASFormatter::peekNextText(const string& firstLine, bool endOnEmptyLine /*false*/) const
{
	bool isFirstLine = true;
	bool needReset = false;
	string nextLine = firstLine;
	size_t firstChar= string::npos;

	// find the first non-blank text, bypassing all comments.
	bool isInComment = false;
	while (sourceIterator->hasMoreLines())
	{
		if (isFirstLine)
			isFirstLine = false;
		else
		{
			nextLine = sourceIterator->peekNextLine();
			needReset = true;
		}

		firstChar = nextLine.find_first_not_of(" \t");
		if (firstChar == string::npos)
		{
			if (endOnEmptyLine && !isInComment)
				break;
			else
				continue;
		}

		if (nextLine.compare(firstChar, 2, "/*") == 0)
			isInComment = true;

		if (isInComment)
		{
			firstChar = nextLine.find("*/", firstChar);
			if (firstChar == string::npos)
				continue;
			firstChar += 2;
			isInComment = false;
			firstChar = nextLine.find_first_not_of(" \t", firstChar);
			if (firstChar == string::npos)
				continue;
		}

		if (nextLine.compare(firstChar, 2, "//") == 0)
			continue;

		// found the next text
		break;
	}

	if (needReset)
		sourceIterator->peekReset();
	if (firstChar == string::npos)
		nextLine = "";
	else
		nextLine = nextLine.substr(firstChar);
	return nextLine;
}

/**
 * adjust comment position because of adding or deleting spaces
 * the spaces are added or deleted to formattedLine
 * spacePadNum contains the adjustment
 */
void ASFormatter::adjustComments(void)
{
	assert(spacePadNum != 0);
	assert(currentLine.compare(charNum, 2, "//") == 0
	       || currentLine.compare(charNum, 2, "/*") == 0);


	// block comment must be closed on this line with nothing after it
	if (currentLine.compare(charNum, 2, "/*") == 0)
	{
		size_t endNum = currentLine.find("*/", charNum + 2);
		if (endNum == string::npos)
			return;
		if (currentLine.find_first_not_of(" \t", endNum + 2) != string::npos)
			return;
	}

	size_t len = formattedLine.length();
	// don't adjust a tab
	if (formattedLine[len-1] == '\t')
		return;
	// if spaces were removed, need to add spaces before the comment
	if (spacePadNum < 0)
	{
		int adjust = -spacePadNum;          // make the number positive
		formattedLine.append(adjust, ' ');
	}
	// if spaces were added, need to delete extra spaces before the comment
	// if cannot be done put the comment one space after the last text
	else if (spacePadNum > 0)
	{
		int adjust = spacePadNum;
		size_t lastText = formattedLine.find_last_not_of(' ');
		if (lastText != string::npos
		        && lastText < len - adjust - 1)
			formattedLine.resize(len - adjust);
		else if (len > lastText + 2)
			formattedLine.resize(lastText + 2);
		else if (len < lastText + 2)
			formattedLine.append(len - lastText, ' ');
	}
}

/**
 * append the current bracket inside the end of line comments
 * currentChar contains the bracket, it will be appended to formattedLine
 * formattedLineCommentNum is the comment location on formattedLine
 * returns true f appended, false if not
 */
void ASFormatter::appendCharInsideComments(void)
{
	if (formattedLineCommentNum == string::npos)    // does the comment start on the previous line?
//	        || isBeforeAnyComment())                   // does a comment follow on this line?
	{
		appendCurrentChar();                        // don't attach
		return; // false;
	}
	assert(formattedLine.compare(formattedLineCommentNum, 2, "//") == 0
	       || formattedLine.compare(formattedLineCommentNum, 2, "/*") == 0);

	// find the previous non space char
	size_t end = formattedLineCommentNum;
	size_t beg = formattedLine.find_last_not_of(" \t", end-1);
	if (beg == string::npos)                // is the previous line comment only?
	{
		appendCurrentChar();                // don't attach
		return; // false;
	}
	beg++;

	// insert the bracket
	if (end - beg < 3)                      // is there room to insert?
		formattedLine.insert(beg, 3-end+beg, ' ');
	if (formattedLine[beg] == '\t')         // don't pad with a tab
		formattedLine.insert(beg, 1, ' ');
	formattedLine[beg+1] = currentChar;

	if (isBeforeComment())
		breakLine();
	if (isCharImmediatelyPostLineComment)
		shouldBreakLineAtNextChar = true;
	return; // true;
}

/**
 * add or remove space padding to operators
 * currentChar contains the paren
 * the operators and necessary padding will be appended to formattedLine
 * the calling function should have a continue statement after calling this method
 *
 * @param *newOperator     the operator to be padded
 */
void ASFormatter::padOperators(const string *newOperator)
{
	assert(newOperator != NULL);

	bool shouldPad = (newOperator != &AS_COLON_COLON
	                  && newOperator != &AS_PAREN_PAREN
	                  && newOperator != &AS_BLPAREN_BLPAREN
	                  && newOperator != &AS_PLUS_PLUS
	                  && newOperator != &AS_MINUS_MINUS
	                  && newOperator != &AS_NOT
	                  && newOperator != &AS_BIT_NOT
	                  && newOperator != &AS_ARROW
	                  && !(newOperator == &AS_MINUS && isInExponent())
	                  && !((newOperator == &AS_PLUS || newOperator == &AS_MINUS)  // check for unary plus or minus
	                       && (previousNonWSChar == '('
	                           || previousNonWSChar == '='
	                           || previousNonWSChar == ','))
	                  && !(newOperator == &AS_PLUS && isInExponent())
	                  && !isCharImmediatelyPostOperator
	                  && !((newOperator == &AS_MULT || newOperator == &AS_BIT_AND)
	                       && isPointerOrReference())
	                  && !(newOperator == &AS_MULT
	                       && (previousNonWSChar == '.'
	                           || previousNonWSChar == '>'))    // check for ->
	                  && !((isInTemplate || isCharImmediatelyPostTemplate)
	                       && (newOperator == &AS_LS || newOperator == &AS_GR))
	                  && !(newOperator == &AS_GCC_MIN_ASSIGN
	                       && ASBase::peekNextChar(currentLine, charNum+1) == '>')
	                  && !(newOperator == &AS_GR && previousNonWSChar == '?')
	                  && !isInCase
	                 );

	// pad before operator
	if (shouldPad
	        && !isInBlParen
	        && !(newOperator == &AS_COLON && !foundQuestionMark)
	        && !(newOperator == &AS_QUESTION && isSharpStyle() // check for C# nullable type (e.g. int?)
	             && currentLine.find(':', charNum+1) == string::npos)
	   )
		appendSpacePad();
	appendSequence(*newOperator);
	goForward(newOperator->length() - 1);

	// since this block handles '()' and '[]',
	// the parenStack must be updated here accordingly!
	if (newOperator == &AS_PAREN_PAREN
	        || newOperator == &AS_BLPAREN_BLPAREN)
		parenStack->back()--;

	currentChar = (*newOperator)[newOperator->length() - 1];
	// pad after operator
	// but do not pad after a '-' that is a unary-minus.
	if (shouldPad
	        && !isInBlParen
	        && !isBeforeAnyComment()
	        && !(newOperator == &AS_PLUS && isUnaryOperator())
	        && !(newOperator == &AS_MINUS && isUnaryOperator())
	        && !(currentLine.compare(charNum + 1, 1,  ";") == 0)
	        && !(currentLine.compare(charNum + 1, 2, "::") == 0)
	        && !(newOperator == &AS_QUESTION && isSharpStyle() // check for C# nullable type (e.g. int?)
	             && currentLine[charNum+1] == '[')
	   )
		appendSpaceAfter();

	previousOperator = newOperator;
	return;
}

/**
 * align pointer or reference
 * currentChar contains the pointer or reference
 * the sign and necessary padding will be appended to formattedLine
 * the calling function should have a continue statement after calling this method
 */
void ASFormatter::formatPointerOrReference(void)
{
	assert(currentChar == '*' || currentChar == '&');
	assert(isCStyle());

	// do this before bumping charNum
	bool isOldPRCentered = isPointerOrReferenceCentered();

	if (pointerAlignment == ALIGN_TYPE)
	{
		size_t prevCh = formattedLine.find_last_not_of(" \t");
		if (prevCh == string::npos)
			prevCh = 0;
		if (formattedLine.length() == 0 || prevCh == formattedLine.length() - 1)
			appendCurrentChar();
		else
		{
			// exchange * or & with character following the type
			// this may not work every time with a tab character
			string charSave = formattedLine.substr(prevCh+1, 1);
			formattedLine[prevCh+1] = currentChar;
			formattedLine.append(charSave);
		}
		if (isSequenceReached("**"))
		{
			formattedLine.insert(prevCh+2, "*");
			goForward(1);
		}
		// if no space after * then add one
		if (charNum < (int) currentLine.length() - 1
		        && !isWhiteSpace(currentLine[charNum+1])
		        && currentLine[charNum+1] != ')')
			appendSpacePad();
		// if old pointer or reference is centered, remove a space
		if (isOldPRCentered
		        && isWhiteSpace(formattedLine[formattedLine.length()-1]))
		{
			formattedLine.erase(formattedLine.length()-1, 1);
			spacePadNum--;
		}
	}
	else if (pointerAlignment == ALIGN_MIDDLE)
	{
		// compute current whitespace before
		size_t wsBefore = currentLine.find_last_not_of(" \t", charNum - 1);
		if (wsBefore == string::npos)
			wsBefore = 0;
		else
			wsBefore = charNum - wsBefore - 1;
		// adjust for **
		string sequenceToInsert = currentChar == '*' ? "*" : "&";
		if (isSequenceReached("**"))
		{
			sequenceToInsert = "**";
			goForward(1);
		}
		// compute current whitespace after
		size_t wsAfter = currentLine.find_first_not_of(" \t", charNum + 1);
		if (wsAfter == string::npos)
			wsAfter = 0;
		else
			wsAfter = wsAfter - charNum - 1;
		// add whitespace after
		// this may not work every time with tab characters
		string charSave = currentLine.substr(charNum+1, wsAfter);
		formattedLine.append(charSave);
		// adjust current position before adjusting spaces
		goForward(wsAfter);
		// whitespace should be at least 2 chars
		if (wsBefore + wsAfter < 2)
		{
			size_t charsToAppend = (2 - (wsBefore + wsAfter));
			formattedLine.append(charsToAppend, ' ');
			spacePadNum += charsToAppend;
			if (wsBefore == 0) wsBefore++;
			if (wsAfter == 0) wsAfter++;
		}
		// insert the pointer or reference char
		size_t padAfter = (wsBefore + wsAfter) / 2;
		if ((wsBefore + wsAfter) % 2)
			padAfter++;
		formattedLine.insert(formattedLine.length() - padAfter, sequenceToInsert);
	}

	else if (pointerAlignment == ALIGN_NAME)
	{
		size_t startNum = formattedLine.find_last_not_of(" \t");
		string sequenceToInsert = currentChar == '*' ? "*" : "&";
		if (isSequenceReached("**"))
		{
			sequenceToInsert = "**";
			goForward(1);
		}
		size_t nextCh = currentLine.find_first_not_of(" \t", charNum+1);
		if (nextCh == string::npos)
			nextCh = currentLine.length();
		int charsToInsert = nextCh - charNum - 1;
		if (charsToInsert > 0)
		{
			// exchange preceding characters with the following characters
			// this may not work every time with tab characters
			string charSave = currentLine.substr(charNum+1, charsToInsert);
			formattedLine.append(charSave);
			appendSequence(sequenceToInsert, false);
			goForward(charsToInsert);
		}
		else
			appendSequence(sequenceToInsert, false);
		// if no space before * then add one
		if (startNum != string::npos
		        && !isWhiteSpace(formattedLine[startNum+1]))
		{
			formattedLine.insert(startNum+1 , 1, ' ');
			spacePadNum++;
		}
		// if old pointer or reference is centered, remove a space
		if (isOldPRCentered
		        && formattedLine.length() > startNum+1
		        && isWhiteSpace(formattedLine[startNum+1]))
		{
			formattedLine.erase(startNum+1, 1);
			spacePadNum--;
		}
	}
	else	// pointerAlignment == ALIGN_NONE
	{
		appendCurrentChar();
	}
	return;
}

/**
 * add or remove space padding to parens
 * currentChar contains the paren
 * the parens and necessary padding will be appended to formattedLine
 * the calling function should have a continue statement after calling this method
 */
void ASFormatter::padParens(void)
{
	assert(currentChar == '(' || currentChar == ')');

	if (currentChar == '(')
	{
		int spacesOutsideToDelete = formattedLine.length() - 1;
		int spacesInsideToDelete = 0;

		// compute spaces outside the opening paren to delete
		if (shouldUnPadParens)
		{
			char lastChar = ' ';
			bool prevIsParenHeader = false;
			size_t i = formattedLine.find_last_not_of(" \t");
			if (i != string::npos)
			{
				// if last char is a bracket the previous whitespace is an indent
				if (formattedLine[i] == '{')
					spacesOutsideToDelete = 0;
				else
				{
					spacesOutsideToDelete -= i;
					lastChar = formattedLine[i];
					// if previous word is a header, it will be a paren header
					string prevWord = getPreviousWord(formattedLine, formattedLine.length());
					const string* prevWordH = NULL;
					if (shouldPadHeader
					        && prevWord.length() > 0
					        && isCharPotentialHeader(prevWord, 0))
						prevWordH = ASBeautifier::findHeader(prevWord, 0, headers);
					if (prevWordH != NULL)
					{
						prevIsParenHeader = true;
						// trace
						//cout << traceLineNumber << " " << *prevWordH << endl;
					}
					else if (prevWord == "return"   // don't unpad return statements
					         || prevWord == "*")    // don't unpad multiply or pointer
					{
						prevIsParenHeader = true;
						// trace
						//cout << traceLineNumber << " " << prevWord << endl;
					}
					// don't unpad variables
					else if (prevWord == "bool"
					         || prevWord ==  "int"
					         || prevWord ==  "void"
					         || prevWord ==  "void*"
					         || (prevWord.length() >= 6     // check end of word for _t
					             && prevWord.compare(prevWord.length()-2, 2, "_t") == 0)
					         || prevWord ==  "BOOL"
					         || prevWord ==  "DWORD"
					         || prevWord ==  "HWND"
					         || prevWord ==  "INT"
					         || prevWord ==  "LPSTR"
					         || prevWord ==  "VOID"
					         || prevWord ==  "LPVOID"
					        )
					{
						prevIsParenHeader = true;
						// trace
						//cout << traceLineNumber << " " << prevWord << endl;
					}
				}
			}
			// do not unpad operators, but leave them if already padded
			if (shouldPadParensOutside || prevIsParenHeader)
				spacesOutsideToDelete--;
			else if (lastChar == '|'          // check for ||
			         || lastChar == '&'      // check for &&
			         || lastChar == ','
			         || (lastChar == '>' && !foundCastOperator)
			         || lastChar == '<'
			         || lastChar == '?'
			         || lastChar == ':'
			         || lastChar == ';'
			         || lastChar == '='
			         || lastChar == '+'
			         || lastChar == '-'
			         || (lastChar == '*' && isInPotentialCalculation)
			         || lastChar == '/'
			         || lastChar == '%')
				spacesOutsideToDelete--;

			if (spacesOutsideToDelete > 0)
			{
				formattedLine.erase(i + 1, spacesOutsideToDelete);
				spacePadNum -= spacesOutsideToDelete;
			}
		}

		// pad open paren outside
		char peekedCharOutside = peekNextChar();
		if (shouldPadParensOutside)
			if (!(currentChar == '(' && peekedCharOutside == ')'))
				appendSpacePad();

		appendCurrentChar();

		// unpad open paren inside
		if (shouldUnPadParens)
		{
			size_t j = currentLine.find_first_not_of(" \t", charNum + 1);
			if (j != string::npos)
				spacesInsideToDelete = j - charNum - 1;
			if (shouldPadParensInside)
				spacesInsideToDelete--;
			if (spacesInsideToDelete > 0)
			{
				currentLine.erase(charNum + 1, spacesInsideToDelete);
				spacePadNum -= spacesInsideToDelete;
			}
			// convert tab to space if requested
			if (shouldConvertTabs
			        && (int)currentLine.length() > charNum
			        && currentLine[charNum+1] == '\t')
				currentLine[charNum+1] = ' ';

		}

		// pad open paren inside
		char peekedCharInside = peekNextChar();
		if (shouldPadParensInside)
			if (!(currentChar == '(' && peekedCharInside == ')'))
				appendSpaceAfter();
		// trace
		//if(spacesOutsideToDelete > 0 || spacesInsideToDelete > 0)
		//    cout << traceLineNumber << " " << spacesOutsideToDelete << '(' << spacesInsideToDelete << endl;
	}
	else if (currentChar == ')' /*|| currentChar == ']'*/)
	{
		int spacesOutsideToDelete = 0;
		int spacesInsideToDelete = formattedLine.length();

		// unpad close paren inside
		if (shouldUnPadParens)
		{
			size_t i = formattedLine.find_last_not_of(" \t");
			if (i != string::npos)
				spacesInsideToDelete = formattedLine.length() - 1 - i;
			if (shouldPadParensInside)
				spacesInsideToDelete--;
			if (spacesInsideToDelete > 0)
			{
				formattedLine.erase(i + 1, spacesInsideToDelete);
				spacePadNum -= spacesInsideToDelete;
			}
		}

		// pad close paren inside
		if (shouldPadParensInside)
			if (!(previousChar == '(' && currentChar == ')'))
				appendSpacePad();

		appendCurrentChar();

		// unpad close paren outside
		if (shouldUnPadParens)
		{
			// may have end of line comments
			size_t j = currentLine.find_first_not_of(" \t", charNum + 1);
			if (j != string::npos)
				if (currentLine[j] == '[' || currentLine[j] == ']')
					spacesOutsideToDelete = j - charNum - 1;
			if (shouldPadParensOutside)
				spacesOutsideToDelete--;

			if (spacesOutsideToDelete > 0)
			{
				currentLine.erase(charNum + 1, spacesOutsideToDelete);
				spacePadNum -= spacesOutsideToDelete;
			}
		}

		// pad close paren outside
		char peekedCharOutside = peekNextChar();
		if (shouldPadParensOutside)
			if (peekedCharOutside != ';'
			        && peekedCharOutside != ','
			        && peekedCharOutside != '.'
			        && peekedCharOutside != '-')    // check for ->
				appendSpaceAfter();

		// trace
		//if(spacesInsideToDelete > 0)
		//	cout << traceLineNumber << " " << spacesInsideToDelete << ')' << 0 << endl;
	}
	return;
}

/**
 * format opening bracket as attached or broken
 * currentChar contains the bracket
 * the brackets will be appended to the current formattedLine or a new formattedLine as necessary
 * the calling function should have a continue statement after calling this method
 *
 * @param bracketType    the type of bracket to be formatted.
 */
void ASFormatter::formatOpeningBracket(BracketType bracketType)
{
	assert(!isBracketType(bracketType, ARRAY_TYPE));
	assert(currentChar == '{');

	parenStack->push_back(0);

	bool breakBracket = isCurrentBracketBroken();
//	previousBracketIsBroken = breakBracket;

	if (breakBracket)
	{
		if (isBeforeAnyComment() && isOkToBreakBlock(bracketType))
		{
			// if comment is at line end leave the comment on this line
			if (isBeforeAnyLineEndComment(charNum) && !currentLineBeginsWithBracket)	// lineBeginsWith('{')
			{
				currentChar = ' ';              // remove bracket from current line
				currentLine[charNum] = currentChar;
				appendOpeningBracket = true;    // append bracket to following line
				// if possible comment was moved by attaching a bracket
				// move comment 2 spaces to the left
//				if (currentLine.compare(charNum+1, 2, " /") == 0)
//					spacePadNum += 2;
			}
			// else put comment after the bracket
			else if (!isBeforeMultipleLineEndComments(charNum))
				breakLine();
		}
		else if (!isBracketType(bracketType, SINGLE_LINE_TYPE))
			breakLine();
		else if (shouldBreakOneLineBlocks && peekNextChar() != '}')
			breakLine();
		else if (!isInLineBreak)
			appendSpacePad();

		appendCurrentChar();

		// should a following comment break from the bracket?
		// must break the line AFTER the bracket
		if (isBeforeComment()
		        && formattedLine[0] == '{'
		        && isOkToBreakBlock(bracketType)
		        && (bracketFormatMode == BREAK_MODE
		            || bracketFormatMode == LINUX_MODE
		            || bracketFormatMode == STROUSTRUP_MODE))
		{
			shouldBreakLineAtNextChar = true;
		}

	}
	else    // attach bracket
	{
		// are there comments before the bracket?
		if (isCharImmediatelyPostComment || isCharImmediatelyPostLineComment)
		{
			if (isOkToBreakBlock(bracketType)
			        && !(isCharImmediatelyPostComment && isCharImmediatelyPostLineComment)	// don't attach if two comments on the line
			        && peekNextChar() != '}'        // don't attach { }
			        && previousCommandChar != '{'   // don't attach { {
			        && previousCommandChar != '}'   // don't attach } {
			        && previousCommandChar != ';')  // don't attach ; {
			{
				appendCharInsideComments();
//				if (!appendCharInsideComments())
//					previousBracketIsBroken = true;
				//if (isBeforeComment())
				//{
				//	breakLine();
				//}
			}
			else
			{
				appendCurrentChar();            // don't attach
//				previousBracketIsBroken = true;
			}
		}
		else if (previousCommandChar == '{'
		         || previousCommandChar == '}'
		         || previousCommandChar == ';')  // '}' , ';' chars added for proper handling of '{' immediately after a '}' or ';'
		{
			appendCurrentChar();                // don't attach
		}
		else
		{
			// if a blank line preceeds this don't attach
			if (isEmptyLine(formattedLine))
				appendCurrentChar();            // don't attach
			else if (isOkToBreakBlock(bracketType)
			         && !(isImmediatelyPostPreprocessor
			              && currentLineBeginsWithBracket))		// lineBeginsWith('{')
			{
				if (peekNextChar() != '}')
				{
					appendSpacePad();
					appendCurrentChar(false);       // OK to attach
					// should a following comment attach with the bracket?
					// insert spaces to reposition the comment
					if (isBeforeComment()
					        && !isBeforeMultipleLineEndComments(charNum)
					        && (!isBeforeAnyLineEndComment(charNum)	|| currentLineBeginsWithBracket))	// lineBeginsWith('{')
					{
						breakLine();
						currentLine.insert(charNum+1, charNum+1, ' ');
					}
				}
				else
				{
					appendSpacePad();
					appendCurrentChar();
				}
			}
			else
			{
				if (!isInLineBreak)
					appendSpacePad();
				appendCurrentChar();            // don't attach
			}
		}
	}
}

/**
 * format closing bracket
 * currentChar contains the bracket
 * the calling function should have a continue statement after calling this method
 *
 * @param bracketType    the type of bracket to be formatted.
 */
void ASFormatter::formatClosingBracket(BracketType bracketType)
{
	assert(!isBracketType(bracketType, ARRAY_TYPE));
	assert(currentChar == '}');

	// parenStack must contain one entry
	if (parenStack->size() > 1)
		parenStack->pop_back();

	// mark state of immediately after empty block
	// this state will be used for locating brackets that appear immedately AFTER an empty block (e.g. '{} \n}').
	if (previousCommandChar == '{')
		isImmediatelyPostEmptyBlock = true;

	if ((!(previousCommandChar == '{' && isPreviousBracketBlockRelated))    // this '{' does not close an empty block
	        && isOkToBreakBlock(bracketType)                                // astyle is allowed to break on line blocks
	        && !isImmediatelyPostEmptyBlock)                                // this '}' does not immediately follow an empty block
	{
		breakLine();
		appendCurrentChar();
	}
	else
	{
		//			if (!isCharImmediatelyPostComment
		////                  && !bracketFormatMode == NONE_MODE
		//			        && !isImmediatelyPostEmptyBlock)
		//				isInLineBreak = false;

		appendCurrentChar();
	}

	// if a declaration follows a definition, space pad
	if (isLegalNameChar(peekNextChar()))
		appendSpaceAfter();

	if (shouldBreakBlocks && currentHeader != NULL && parenStack->back() == 0)
	{
		isAppendPostBlockEmptyLineRequested = true;
	}
}

/**
 * format array brackets as attached or broken
 * determine if the brackets can have an inStatement indent
 * currentChar contains the bracket
 * the brackets will be appended to the current formattedLine or a new formattedLine as necessary
 * the calling function should have a continue statement after calling this method
 *
 * @param bracketType            the type of bracket to be formatted, must be an ARRAY_TYPE.
 * @param isOpeningArrayBracket  indicates if this is the opening bracket for the array block.
 */
void ASFormatter::formatArrayBrackets(BracketType bracketType, bool isOpeningArrayBracket)
{
	assert(isBracketType(bracketType, ARRAY_TYPE));
	assert(currentChar == '{' || currentChar == '}');

	if (currentChar == '{')
	{
		// is this the first opening bracket in the array?
		if (isOpeningArrayBracket)
		{
			if (bracketFormatMode == ATTACH_MODE
			        || bracketFormatMode == LINUX_MODE
			        || bracketFormatMode == STROUSTRUP_MODE)
			{
				// don't attach to a preprocessor directive
				if (isImmediatelyPostPreprocessor && currentLineBeginsWithBracket)	// lineBeginsWith('{')
				{
					isInLineBreak = true;
					appendCurrentChar();                // don't attach
				}
				else if (isCharImmediatelyPostComment)
				{
					// TODO: attach bracket to line-end comment
					appendCurrentChar();                // don't attach
				}
				else if (isCharImmediatelyPostLineComment)
				{
					appendCharInsideComments();
				}
				else
				{
					// if a blank line preceeds this don't attach
					if (isEmptyLine(formattedLine))
						appendCurrentChar();            // don't attach
					else
					{
						// if bracket is broken or not an assignment
						if (currentLineBeginsWithBracket 	// lineBeginsWith('{')
						        && !isBracketType(bracketTypeStack->back(), SINGLE_LINE_TYPE))
						{
							appendSpacePad();
							appendCurrentChar(false);       // OK to attach

							if (currentLineBeginsWithBracket
							        && (int)currentLineBracketNum == charNum)
								shouldBreakLineAtNextChar = true;
						}
						else
						{
							appendSpacePad();
							appendCurrentChar();
						}
					}
				}
			}
			else if (bracketFormatMode == BREAK_MODE)
			{
				if (isWhiteSpace(peekNextChar()))
					breakLine();
				else if (isBeforeAnyComment())
				{
					// do not break unless comment is at line end
					if (isBeforeAnyLineEndComment(charNum) && !currentLineBeginsWithBracket)
					{
						currentChar = ' ';              // remove bracket from current line
						appendOpeningBracket = true;    // append bracket to following line
					}
				}
				if (!isInLineBreak)
					appendSpacePad();
				appendCurrentChar();

				if (currentLineBeginsWithBracket
				        && (int)currentLineBracketNum == charNum
				        && !isBracketType(bracketTypeStack->back(), SINGLE_LINE_TYPE))
					shouldBreakLineAtNextChar = true;
			}
			else if (bracketFormatMode == HORSTMANN_MODE)
			{
				if (isWhiteSpace(peekNextChar()))
					breakLine();
				else if (isBeforeAnyComment())
				{
					// do not break unless comment is at line end
					if (isBeforeAnyLineEndComment(charNum) && !currentLineBeginsWithBracket)	// lineBeginsWith('{')
					{
						currentChar = ' ';              // remove bracket from current line
						appendOpeningBracket = true;    // append bracket to following line
					}
				}
				if (!isInLineBreak)
					appendSpacePad();
				appendCurrentChar();
				//if (isWhiteSpace(peekNextChar()))
				//	formatRunIn();
			}
			else if (bracketFormatMode == NONE_MODE)
			{
				if (currentLineBeginsWithBracket)       // lineBeginsWith('{')
				{
					appendCurrentChar();                // don't attach
					//if (!isWhiteSpace(peekNextChar()))
					//	formatRunIn();
				}
				else
				{
					appendSpacePad();
					appendCurrentChar(false);           // OK to attach
				}
			}
		}
		else	     // not the first opening bracket
		{
			if (bracketFormatMode == HORSTMANN_MODE)
			{
				if (previousNonWSChar == '{'
				        && bracketTypeStack->size() > 2
				        && !isBracketType((*bracketTypeStack)[bracketTypeStack->size()-2], SINGLE_LINE_TYPE))
					formatArrayRunIn();
			}
			else if (!isInLineBreak
			         && !isWhiteSpace(peekNextChar())
			         && previousNonWSChar == '{'
			         && bracketTypeStack->size() > 2
			         && !isBracketType((*bracketTypeStack)[bracketTypeStack->size()-2], SINGLE_LINE_TYPE))
				formatArrayRunIn();

			appendCurrentChar();
		}
	}
	else if (currentChar == '}')
	{
		// does this close the first opening bracket in the array?
		if (!isBracketType(bracketType, SINGLE_LINE_TYPE) )
			breakLine();
		appendCurrentChar();

		// if a declaration follows an enum definition, space pad
		char peekedChar = peekNextChar();
		if (isLegalNameChar(peekedChar)
		        || peekedChar == '[')
			appendSpaceAfter();
	}
}

/**
 * determine if a run-in can be attached.
 * if it can insert the indents in formattedLine and reset the current line break.
 */
void ASFormatter::formatRunIn()
{
	assert(bracketFormatMode == HORSTMANN_MODE || bracketFormatMode == NONE_MODE);

	// keep one line blocks returns true without indenting the run-in
	if (!isOkToBreakBlock(bracketTypeStack->back()))
		return; // true;

	size_t lastText = formattedLine.find_last_not_of(" \t");
	if (lastText == string::npos || formattedLine[lastText] != '{')
		return; // false;

	// make sure the bracket is broken
	if (formattedLine.find_first_not_of(" \t{") != string::npos)
		return; // false;

	if (isBracketType(bracketTypeStack->back(), NAMESPACE_TYPE))
		return; // false;

	bool extraIndent = false;
	isInLineBreak = true;

	// cannot attach a class modifier without indent-classes
	if (isCStyle()
	        && isBracketType(bracketTypeStack->back(), CLASS_TYPE)
	        && isCharPotentialHeader(currentLine, charNum))
	{
		if (findKeyword(currentLine, charNum, AS_PUBLIC)
		        || findKeyword(currentLine, charNum, AS_PRIVATE)
		        || findKeyword(currentLine, charNum, AS_PROTECTED))
		{
			if (!getClassIndent())
				return; // false;
		}
		else if (getClassIndent())
			extraIndent = true;
	}

	// cannot attach a 'case' statement without indent-switches
	if (!getSwitchIndent()
	        && isCharPotentialHeader(currentLine, charNum)
	        && (findKeyword(currentLine, charNum, AS_CASE)
	            || findKeyword(currentLine, charNum, AS_DEFAULT)))
		return; // false;

	// extra indent for switch statements
	if (getSwitchIndent()
	        && !preBracketHeaderStack->empty()
	        && preBracketHeaderStack->back() == &AS_SWITCH
	        && ((isLegalNameChar(currentChar)
	             && !findKeyword(currentLine, charNum, AS_CASE))
	            || isSequenceReached("//")
	            || isSequenceReached("/*")))
		extraIndent = true;

	isInLineBreak = false;
	// remove for extra whitespace
	if (formattedLine.length() > lastText+1
	        && formattedLine.find_first_not_of(" \t", lastText+1) == string::npos)
		formattedLine.erase(lastText+1);

	if (getIndentString() == "\t")
	{
		appendChar('\t', false);
		horstmannIndentChars = 2;	// one for { and one for tab
		if (extraIndent)
		{
			appendChar('\t', false);
			horstmannIndentChars++;
		}
	}
	else
	{
		int indent = getIndentLength();
		formattedLine.append(indent-1, ' ');
		horstmannIndentChars = indent;
		if (extraIndent)
		{
			formattedLine.append(indent, ' ');
			horstmannIndentChars += indent;
		}
	}
	isInHorstmannRunIn = true;
	// return true;
}

/**
 * remove whitepace and add indentation for an array run-in.
 */
void ASFormatter::formatArrayRunIn()
{
	assert(isBracketType(bracketTypeStack->back(), ARRAY_TYPE));

	// make sure the bracket is broken
	if (formattedLine.find_first_not_of(" \t{") != string::npos)
		return;

	size_t lastText = formattedLine.find_last_not_of(" \t");
	if (lastText == string::npos || formattedLine[lastText] != '{')
		return;

	// check for extra whitespace
	if (formattedLine.length() > lastText+1
	        && formattedLine.find_first_not_of(" \t", lastText+1) == string::npos)
		formattedLine.erase(lastText+1);

	if (getIndentString() == "\t")
	{
		appendChar('\t', false);
		horstmannIndentChars = 2;	// one for { and one for tab
	}
	else
	{
		int indent = getIndentLength();
		formattedLine.append(indent-1, ' ');
		horstmannIndentChars = indent;
	}
	isInHorstmannRunIn = true;
	isInLineBreak = false;
}

/**
 * delete a bracketTypeStack vector object
 * BracketTypeStack did not work with the DeleteContainer template
 */
void ASFormatter::deleteContainer(vector<BracketType>* &container)
{
	if (container != NULL)
	{
		container->clear();
		delete (container);
		container = NULL;
	}
}

/**
 * delete a vector object
 * T is the type of vector
 * used for all vectors except bracketTypeStack
 */
template<typename T>
void ASFormatter::deleteContainer(T &container)
{
	if (container != NULL)
	{
		container->clear();
		delete (container);
		container = NULL;
	}
}

/**
 * initialize a BracketType vector object
 * BracketType did not work with the DeleteContainer template
 */
void ASFormatter::initContainer(vector<BracketType>* &container, vector<BracketType>* value)
{
	if (container != NULL)
		deleteContainer(container);
	container = value;
}

/**
 * initialize a vector object
 * T is the type of vector
 * used for all vectors except bracketTypeStack
 */
template<typename T>
void ASFormatter::initContainer(T &container, T value)
{
	// since the ASFormatter object is never deleted,
	// the existing vectors must be deleted before creating new ones
	if (container != NULL)
		deleteContainer(container);
	container = value;
}

/**
 * convert tabs to spaces.
 * charNum points to the current character to convert to spaces.
 * tabIncrementIn is the increment that must be added for tab indent characters
 *     to get the correct column for the current tab.
 * replaces the tab in currentLine with the required number of spaces.
 * replaces the value of currentChar.
 */
void ASFormatter::convertTabToSpaces()
{
	assert(currentLine[charNum] == '\t');

	// do NOT replace if in quotes
	if (isInQuote || isInQuoteContinuation)
		return;

	size_t indent = getIndentLength();
	size_t numSpaces = indent - ((tabIncrementIn + charNum) % indent);
	currentLine.replace(charNum, 1, numSpaces, ' ');
	currentChar = currentLine[charNum];
}

/**
 * check for a following header when a comment is reached.
 * if a header follows, the comments are kept as part of the header block.
 * firstLine must contain the start of the comment.
 */
void ASFormatter::checkForFollowingHeader(const string& firstLine)
{
	// look ahead to find the next non-comment text
	string nextText = peekNextText(firstLine, true);
	if (nextText.length() == 0 || !isCharPotentialHeader(nextText, 0))
		return;

	const string* newHeader = ASBeautifier::findHeader(nextText, 0, headers);

	if (newHeader == NULL)
		return;

	// may need to break if a header follows
	bool isClosingHeader = (newHeader == &AS_ELSE
	                        || newHeader == &AS_CATCH
	                        || newHeader == &AS_FINALLY);

	// if a closing header, reset break unless break is requested
	if (isClosingHeader)
	{
		if (!shouldBreakClosingHeaderBlocks)
			isPrependPostBlockEmptyLineRequested = false;
	}
	// if an opening header, break before the comment
	else
	{
		isPrependPostBlockEmptyLineRequested = true;
	}
}

/**
 * process preprocessor statements.
 * charNum should be the index of the preprocessor directive.
 *
 * delete bracketTypeStack entries added by #if if a #else is found.
 * prevents double entries in the bracketTypeStack.
 */
void ASFormatter::processPreprocessor()
{
	assert(currentChar == '#');

	const int preproc = charNum + 1;

	if (currentLine.compare(preproc, 2, "if") == 0)
	{
		preprocBracketTypeStackSize = bracketTypeStack->size();
	}
	else if (currentLine.compare(preproc, 4, "else") == 0)
	{
		// delete stack entries added in #if
		// should be replaced by #else
		if (preprocBracketTypeStackSize > 0)
		{
			int addedPreproc = bracketTypeStack->size() - preprocBracketTypeStackSize;
			for (int i=0; i < addedPreproc; i++)
				bracketTypeStack->pop_back();
		}
	}
}

/**
 * determine if the next line starts a comment
 * and a header follows the comment or comments
 */
bool ASFormatter::commentAndHeaderFollows() const
{
	// is the next line a comment
	string nextLine = sourceIterator->peekNextLine();
	size_t firstChar = nextLine.find_first_not_of(" \t");
	if (firstChar == string::npos
	        || !(nextLine.compare(firstChar, 2, "//") == 0
	             || nextLine.compare(firstChar, 2, "/*") == 0))
	{
		sourceIterator->peekReset();
		return false;
	}

	// if next line is a comment, find the next non-comment text
	// peekNextText will do the peekReset
	string nextText = peekNextText(nextLine, true);
	if (nextText.length() == 0 || !isCharPotentialHeader(nextText, 0))
		return false;

	const string* newHeader = ASBeautifier::findHeader(nextText, 0, headers);

	if (newHeader == NULL)
		return false;

	bool isClosingHeader = (newHeader == &AS_ELSE
	                        || newHeader == &AS_CATCH
	                        || newHeader == &AS_FINALLY);

	if (isClosingHeader && !shouldBreakClosingHeaderBlocks)
		return false;

	return true;
}

/**
 * determine if a bracket should be attached or broken
 * uses brackets in the bracketTypeStack
 * the last bracket in the bracketTypeStack is the one being formatted
 * returns true if the bracket should be broken
 */
bool ASFormatter::isCurrentBracketBroken() const
{
	assert(bracketTypeStack->size() > 0);

	bool breakBracket = false;
	size_t bracketTypeStackEnd = bracketTypeStack->size()-1;

	if (isBracketType((*bracketTypeStack)[bracketTypeStackEnd], EXTERN_TYPE))
	{
		if (currentLineBeginsWithBracket
		        || bracketFormatMode == HORSTMANN_MODE)
			breakBracket = true;
	}
	else if (bracketFormatMode == NONE_MODE)
	{
		if (currentLineBeginsWithBracket)		// lineBeginsWith('{')
			breakBracket = true;
	}
	else if (bracketFormatMode == BREAK_MODE || bracketFormatMode == HORSTMANN_MODE)
	{
		breakBracket = true;
	}
	else if (bracketFormatMode == LINUX_MODE || bracketFormatMode == STROUSTRUP_MODE)
	{
		// break a class if Linux
		if (isBracketType((*bracketTypeStack)[bracketTypeStackEnd], CLASS_TYPE))
		{
			if (bracketFormatMode == LINUX_MODE)
				breakBracket = true;
		}
		// break a namespace or interface if Linux
		else if (isBracketType((*bracketTypeStack)[bracketTypeStackEnd], NAMESPACE_TYPE)
		         || isBracketType((*bracketTypeStack)[bracketTypeStackEnd], INTERFACE_TYPE))
		{
			if (bracketFormatMode == LINUX_MODE)
				breakBracket = true;
		}
		// break the first bracket if a function
		else if (bracketTypeStackEnd == 1
		         && isBracketType((*bracketTypeStack)[bracketTypeStackEnd], COMMAND_TYPE))
		{
			breakBracket = true;
		}
		else if (bracketTypeStackEnd > 1)
		{
			// break the first bracket after a namespace or extern if a function
			if (isBracketType((*bracketTypeStack)[bracketTypeStackEnd-1], NAMESPACE_TYPE)
			        || isBracketType((*bracketTypeStack)[bracketTypeStackEnd-1], EXTERN_TYPE))
			{
				if (isBracketType((*bracketTypeStack)[bracketTypeStackEnd], COMMAND_TYPE))
					breakBracket = true;
			}
			// if not C style then break the first bracket after a class if a function
			else if (!isCStyle())
			{
				if ((isBracketType((*bracketTypeStack)[bracketTypeStackEnd-1], CLASS_TYPE)
				        || isBracketType((*bracketTypeStack)[bracketTypeStackEnd-1], ARRAY_TYPE)
				        || isBracketType((*bracketTypeStack)[bracketTypeStackEnd-1], STRUCT_TYPE))
				        && isBracketType((*bracketTypeStack)[bracketTypeStackEnd], COMMAND_TYPE))
					breakBracket = true;
			}
		}
	}
	return breakBracket;
}

/**
 * format comment body
 * the calling function should have a continue statement after calling this method
 */
void ASFormatter::formatCommentBody()
{
	assert(isInComment);

	if (isSequenceReached("*/"))
	{
		isInComment = false;
		noTrimCommentContinuation = false;
		isImmediatelyPostComment = true;
		appendSequence(AS_CLOSE_COMMENT);
		goForward(1);
		if (doesLineStartComment
		        && (currentLine.find_first_not_of(" \t", charNum+1) == string::npos))
			lineEndsInCommentOnly = true;
		if (peekNextChar() == '}'
		        && previousCommandChar != ';'
		        && !isBracketType(bracketTypeStack->back(),  ARRAY_TYPE)
		        && isOkToBreakBlock(bracketTypeStack->back()))
			breakLine();
	}
	else
	{
		appendCurrentChar();
		// append the comment up to the next tab or comment end
		// tabs must be checked for convert-tabs before appending
		while (charNum + 1 < (int) currentLine.length()
		        && currentLine[charNum+1] != '\t'
		        && currentLine.compare(charNum+1, 2, "*/") != 0)
		{
			currentChar = currentLine[++charNum];
			appendCurrentChar();
		}
	}
}

/**
 * format a comment opener
 * the comment opener will be appended to the current formattedLine or a new formattedLine as necessary
 * the calling function should have a continue statement after calling this method
 */
void ASFormatter::formatCommentOpener()
{
	assert(isSequenceReached("/*"));

	isInComment = true;
	isImmediatelyPostLineComment = false;

	if (spacePadNum != 0)
		adjustComments();
	formattedLineCommentNum = formattedLine.length();

	// must be done BEFORE appendSequence
	if (previousCommandChar == '{'
	        && !isImmediatelyPostComment
	        && !isImmediatelyPostLineComment)
	{
		if (bracketFormatMode == NONE_MODE)
		{
			// should a run-in statement be attached?
			if (currentLineBeginsWithBracket)
				formatRunIn();
		}
		else if (bracketFormatMode == ATTACH_MODE)
		{
			// if the bracket was not attached?
			if (formattedLine[0] == '{'
			        && !isBracketType(bracketTypeStack->back(), SINGLE_LINE_TYPE))
				isInLineBreak = true;
		}
		else if (bracketFormatMode == HORSTMANN_MODE)
		{
			// should a run-in statement be attached?
			if (formattedLine[0] == '{')
				formatRunIn();
		}
	}
	else if (!doesLineStartComment)
		noTrimCommentContinuation = true;

	// appendSequence will write the previous line
	appendSequence(AS_OPEN_COMMENT);
	goForward(1);

	// must be done AFTER appendSequence
	if (shouldBreakBlocks)
	{
		// break before the comment if a header follows the comment
		// for speed, do not check if previous line is empty,
		//     if previous line is a line comment or if previous line is '{'
		if (doesLineStartComment
		        && !isImmediatelyPostEmptyLine
		        && !isImmediatelyPostComment
		        && !isImmediatelyPostLineComment
		        && previousCommandChar != '{')
		{
			checkForFollowingHeader(currentLine.substr(charNum-1));
		}
	}

	if (previousCommandChar == '}')
		currentHeader = NULL;
}

/**
 * format a line comment body
 * the calling function should have a continue statement after calling this method
 */
void ASFormatter::formatLineCommentBody()
{
	assert(isInLineComment);

	appendCurrentChar();
	// append the comment up to the next tab
	// tabs must be checked for convert-tabs before appending
	while (charNum + 1 < (int) currentLine.length()
	        && currentLine[charNum+1] != '\t')
	{
		currentChar = currentLine[++charNum];
		appendCurrentChar();
	}

	// explicitely break a line when a line comment's end is found.
	if (charNum + 1 == (int) currentLine.length())
	{
		isInLineBreak = true;
		isInLineComment = false;
		isImmediatelyPostLineComment = true;
		currentChar = 0;  //make sure it is a neutral char.
	}
}

/**
 * format a line comment opener
 * the line comment opener will be appended to the current formattedLine or a new formattedLine as necessary
 * the calling function should have a continue statement after calling this method
 */
void ASFormatter::formatLineCommentOpener()
{
	assert(isSequenceReached("//"));

	if (currentLine[charNum+2] == '\xf2')       // check for windows line marker
		isAppendPostBlockEmptyLineRequested = false;

	isInLineComment = true;
	isCharImmediatelyPostComment = false;

	// do not indent if in column 1 or 2
	if (lineCommentNoIndent == false)
	{
		if (charNum == 0)
			lineCommentNoIndent = true;
		else if (charNum == 1 && currentLine[0] == ' ')
			lineCommentNoIndent = true;
	}
	// move comment if spaces were added or deleted
	if (lineCommentNoIndent == false && spacePadNum != 0)
		adjustComments();
	formattedLineCommentNum = formattedLine.length();

	// must be done BEFORE appendSequence
	// check for run-in statement
	if (previousCommandChar == '{'
	        && !isImmediatelyPostComment
	        && !isImmediatelyPostLineComment)
	{
		if (bracketFormatMode == NONE_MODE)
		{
			if (currentLineBeginsWithBracket)
				formatRunIn();
		}
		else if (bracketFormatMode == HORSTMANN_MODE)
		{
			if (!lineCommentNoIndent)
				formatRunIn();
			else
				isInLineBreak = true;
		}
		else if (bracketFormatMode == BREAK_MODE)
		{
			if (formattedLine[0] == '{')
				isInLineBreak = true;
		}
		else
		{
			if (currentLineBeginsWithBracket)
				isInLineBreak = true;
		}
	}

	// appendSequence will write the previous line
	appendSequence(AS_OPEN_LINE_COMMENT);
	goForward(1);

	if (formattedLine.compare(0, 2, "//") == 0)
		lineIsLineCommentOnly = true;

	// must be done AFTER appendSequence
	if (shouldBreakBlocks)
	{
		// break before the comment if a header follows the line comment
		// for speed, do not check if previous line is empty,
		//     if previous line is a comment or if previous line is '{'
		if (lineIsLineCommentOnly
		        && previousCommandChar != '{'
		        && !isImmediatelyPostEmptyLine
		        && !isImmediatelyPostComment
		        && !isImmediatelyPostLineComment)
		{
			checkForFollowingHeader(currentLine.substr(charNum-1));
		}
	}

	if (previousCommandChar == '}')
		currentHeader = NULL;

	// if tabbed input don't convert the immediately following tabs to spaces
	if (getIndentString() == "\t" && lineCommentNoIndent)
	{
		while (charNum + 1 < (int) currentLine.length()
		        && currentLine[charNum+1] == '\t')
		{
			currentChar = currentLine[++charNum];
			appendCurrentChar();
		}
	}

	// explicitely break a line when a line comment's end is found.
	if (charNum + 1 == (int) currentLine.length())
	{
		isInLineBreak = true;
		isInLineComment = false;
		isImmediatelyPostLineComment = true;
		currentChar = 0;  //make sure it is a neutral char.
	}
}

/**
 * format quote body
 * the calling function should have a continue statement after calling this method
 */
void ASFormatter::formatQuoteBody()
{
	assert(isInQuote);

	if (isSpecialChar)
	{
		isSpecialChar = false;
	}
	else if (currentChar == '\\' && !isInVerbatimQuote)
	{
		if (peekNextChar() == ' ')              // is this '\' at end of line
			haveLineContinuationChar = true;
		else
			isSpecialChar = true;
	}
	else if (isInVerbatimQuote && currentChar == '"')
	{
		if (peekNextChar() == '"')              // check consecutive quotes
		{
			appendSequence("\"\"");
			goForward(1);
			return;
		}
		else
		{
			isInQuote = false;
			isInVerbatimQuote = false;
		}
	}
	else if (quoteChar == currentChar)
	{
		isInQuote = false;
	}

	appendCurrentChar();

	// append the text to the ending quoteChar or an escape sequence
	// tabs in quotes are NOT changed by convert-tabs
	if (isInQuote && currentChar != '\\')
	{
		while (charNum + 1 < (int) currentLine.length()
		        && currentLine[charNum+1] != quoteChar
		        && currentLine[charNum+1] != '\\')
		{
			currentChar = currentLine[++charNum];
			appendCurrentChar();
		}
	}
}

/**
 * format a quote opener
 * the quote opener will be appended to the current formattedLine or a new formattedLine as necessary
 * the calling function should have a continue statement after calling this method
 */
void ASFormatter::formatQuoteOpener()
{
	assert(currentChar == '"' || currentChar == '\'');

	isInQuote = true;
	quoteChar = currentChar;
	if (isSharpStyle() && previousChar == '@')
		isInVerbatimQuote = true;

	// a quote following a bracket is an array
	if (previousCommandChar == '{'
	        && !isImmediatelyPostComment
	        && !isImmediatelyPostLineComment
	        && isNonInStatementArray
	        && !isBracketType(bracketTypeStack->back(), SINGLE_LINE_TYPE)
	        && !isWhiteSpace(peekNextChar()))
	{
		if (bracketFormatMode == NONE_MODE)
		{
			if (currentLineBeginsWithBracket)
				formatRunIn();
		}
		else if (bracketFormatMode == HORSTMANN_MODE)
		{
			if (!lineCommentNoIndent)
				formatRunIn();
			else
				isInLineBreak = true;
		}
		else if (bracketFormatMode == BREAK_MODE)
		{
			if (formattedLine[0] == '{')
				isInLineBreak = true;
		}
		else
		{
			if (currentLineBeginsWithBracket)
				isInLineBreak = true;
		}
	}
	previousCommandChar = ' ';
	appendCurrentChar();
}

/**
 * get the next line comment adjustment that results from breaking a closing bracket.
 * the bracket must be on the same line as the closing header.
 * i.e "} else" changed to "} \n else".
 */
int ASFormatter::getNextLineCommentAdjustment()
{
	assert(foundClosingHeader && previousNonWSChar == '}');
	if (charNum < 1)
		return 0;
	size_t lastBracket = currentLine.rfind('}', charNum - 1);
	if (lastBracket != string::npos)
		return (lastBracket - charNum);	// return a negative number
	return 0;
}

/**
 * get the current line comment adjustment that results from attaching
 * a closing header to a closing bracket.
 * the bracket must be on the line previous to the closing header.
 * the adjustment is 2 chars, one for the bracket and one for the space.
 * i.e "} \n else" changed to "} else".
 */
int ASFormatter::getCurrentLineCommentAdjustment()
{
	assert(foundClosingHeader && previousNonWSChar == '}');
	if (charNum < 1)
		return 2;
	size_t lastBracket = currentLine.rfind('}', charNum - 1);
	if (lastBracket == string::npos)
		return 2;
	return 0;
}

/**
 * get the previous word
 * the argument 'end' must point to the search start.
 * return is the previous word.
 */
string ASFormatter::getPreviousWord(const string& line, int currPos) const
{
	// get the last legal word (may be a number)
	if (currPos == 0)
		return string();

	size_t end = line.find_last_not_of(" \t", currPos-1);
	if (end == string::npos || !isLegalNameChar(line[end]))
		return string();

	int start;          // start of the previous word
	for (start = end; start > -1; start--)
	{
		if (!isLegalNameChar(line[start]) || line[start] == '.')
			break;
	}
	start++;

	return (line.substr(start, end-start+1));
}

/**
 * check if a line break is needed when a closing bracket
 * is followed by a closing header.
 * the break depends on the bracketFormatMode and other factors.
 */
void ASFormatter::isLineBreakBeforeClosingHeader()
{
	assert(foundClosingHeader && previousNonWSChar == '}');
	if (bracketFormatMode == BREAK_MODE || bracketFormatMode == HORSTMANN_MODE)
	{
		isInLineBreak = true;
	}
	else if (bracketFormatMode == NONE_MODE)
	{
		if (shouldBreakClosingHeaderBrackets
		        || getBracketIndent() || getBlockIndent())
		{
			isInLineBreak = true;
		}
		else
		{
			appendSpacePad();
			// is closing bracket broken?
			size_t i = currentLine.find_first_not_of(" \t");
			if (i != string::npos && currentLine[i] == '}')
				isInLineBreak = false;

			if (shouldBreakBlocks)
				isAppendPostBlockEmptyLineRequested = false;
		}
	}
	// bracketFormatMode == ATTACH_MODE, LINUX_MODE, STROUSTRUP_MODE
	else
	{
		if (shouldBreakClosingHeaderBrackets
		        || getBracketIndent() || getBlockIndent())
		{
			isInLineBreak = true;
		}
		else
		{
			// if a blank line does not preceed this
			// or last line is not a one line block, attach header
			size_t firstText = formattedLine.find_first_not_of(" \t");
			if (firstText != string::npos
			        && formattedLine[firstText] != '{')
			{
				isInLineBreak = false;
				appendSpacePad();
				spacePadNum = 0;	// don't count as comment padding
			}

			if (shouldBreakBlocks)
				isAppendPostBlockEmptyLineRequested = false;
		}
	}
}

/**
* add brackets to a single line statement following a header.
*/
void ASFormatter::addBracketsToStatement()
{
	assert(isImmediatelyPostHeader);

	if (currentHeader != &AS_IF
	        && currentHeader != &AS_ELSE
	        && currentHeader != &AS_FOR
	        && currentHeader != &AS_WHILE
	        && currentHeader != &AS_DO
	        && currentHeader != &AS_FOREACH)
		return;

	// do not add if a header follows (i.e. else if)
	if (isCharPotentialHeader(currentLine, charNum))
		if (findHeader(headers) != NULL)
			return;

	// find the next semi-colon
	size_t i;
	for (i = charNum + 1; i < currentLine.length(); i++)
	{
		if (currentLine.compare(i, 2, "//") == 0)
			return;
		if (currentLine.compare(i, 2, "/*") == 0)
		{
			size_t endComment = currentLine.find("*/", i+2);
			if (endComment == string::npos)
				return;
			i = endComment + 2;
		}
		if (currentLine[i] == '\'' || currentLine[i] == '\"')
		{
			size_t endQuote = currentLine.find(currentLine[i], i+1);
			if (endQuote == string::npos)
				return;
			if (currentLine[endQuote-1] == '\\')
				return;
			i = endQuote;
		}
		if (currentLine[i] == ';')
			break;
	}
	if (i >= currentLine.length())	// didn't find semi-colon
		return;

	// add closing bracket before changing the line length
	if (i == currentLine.length() - 1)
		currentLine.append(" }");
	else
		currentLine.insert(i+1, " }");
	// add opening bracket
	currentLine.insert(charNum, "{ ");
	currentChar = '{';
}

/**
* is it ok to break this block?
*/
bool ASFormatter::isOkToBreakBlock(BracketType bracketType) const
{
	if (!isBracketType(bracketType,  SINGLE_LINE_TYPE)
	        || shouldBreakOneLineBlocks
	        || breakCurrentOneLineBlock)
		return true;
	return false;
}

}   // end namespace astyle
