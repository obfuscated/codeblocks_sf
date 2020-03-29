// Scintilla source code edit control
/** @file CellBuffer.cxx
 ** Manages a buffer of cells.
 **/
// Copyright 1998-2001 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cstdarg>

#include <stdexcept>
#include <vector>
#include <algorithm>
#include <memory>

#include "Platform.h"

#include "Scintilla.h"
#include "Position.h"
#include "SplitVector.h"
#include "Partitioning.h"
#include "CellBuffer.h"
#include "UniConversion.h"

#ifdef SCI_NAMESPACE
using namespace Scintilla;
#endif

/* CHANGEBAR begin */
LineChanges::LineChanges() : collecting(0), edition(0) {
}

LineChanges::~LineChanges() {
}

void LineChanges::AdvanceEdition() {
	edition = (edition + 1) % 0x40000000;
}

int LineChanges::GetEdition() const {
	return edition;
}

char *LineChanges::PersistantForm() const {
	if (collecting)
		return state.PersistantForm();
	else
		return 0;
}

void LineChanges::SetChanges(const char *changesState) {
	if (collecting && changesState) {
		state.FromPersistant(changesState);
		AdvanceEdition();
	}
}

void LineChanges::InsertText(int line, int edition, bool undoing) {
	if (collecting && !undoing) {
		int position = line;
		int fillLength = 1;
		if (state.FillRange(position, edition, fillLength)) {
			if (fillLength > 0) {
				AdvanceEdition();
			}
		}
	}
}

void LineChanges::InsertLine(int line, int edition, bool undoing) {
	if (collecting && !undoing) {
		state.InsertSpace(line, 1);
		int linePosition = line;
		int fillLength = 1;
		if (state.FillRange(linePosition, edition, fillLength))
			AdvanceEdition();
	}
}

void LineChanges::RemoveLine(int line, bool undoing) {
	if (collecting && !undoing) {
		state.DeleteRange(line, 1);
		AdvanceEdition();
	}
}

void LineChanges::EnableChangeCollection(bool collecting_, int lines) {
	collecting = collecting_;
	if (collecting) {
		state.InsertSpace(0, lines);
	}
}

void LineChanges::ClearChanged() {
	if (collecting) {
		int position = 0;
		int length = state.Length();
		if (state.FillRange(position, 0, length))
			AdvanceEdition();
	}
}

int LineChanges::GetChanged(int line) const {
	if (collecting) {
		return state.ValueAt(line);
	}
	return 0;
}
/* CHANGEBAR end */

LineVector::LineVector() : starts(256), perLine(0) {
	Init();
}

LineVector::~LineVector() {
	starts.DeleteAll();
}

void LineVector::Init() {
	starts.DeleteAll();
	if (perLine) {
		perLine->Init();
	}
}

void LineVector::SetPerLine(PerLine *pl) {
	perLine = pl;
}

/* CHANGEBAR begin */
void LineVector::InsertText(Sci::Line line, Sci::Position delta, int edition, bool undoing, bool lineUnchanged) {
/* CHANGEBAR end */
	starts.InsertText(line, delta);
/* CHANGEBAR begin */
	// Line stays unchanged if inserted/deleted "something\n" at line start
	// or "\nsomething" at line end
	if (!lineUnchanged) {
		changes.InsertText(line, edition, undoing);
	}
/* CHANGEBAR end */
}

/* CHANGEBAR begin */
void LineVector::InsertLine(Sci::Line line, Sci::Position position, bool lineStart, int edition, bool undoing) {
/* CHANGEBAR end */
	starts.InsertPartition(line, position);
	if (perLine) {
		if ((line > 0) && lineStart)
			line--;
		perLine->InsertLine(line);
	}
/* CHANGEBAR begin */
	changes.InsertLine(line, edition, undoing);
/* CHANGEBAR end */
}

void LineVector::SetLineStart(Sci::Line line, Sci::Position position) {
	starts.SetPartitionStartPosition(line, position);
}

/* CHANGEBAR begin */
void LineVector::RemoveLine(Sci::Line line, bool undoing) {
/* CHANGEBAR end */
	starts.RemovePartition(line);
	if (perLine) {
		perLine->RemoveLine(line);
	}
/* CHANGEBAR begin */
	changes.RemoveLine(line, undoing);
/* CHANGEBAR end */
}

Sci::Line LineVector::LineFromPosition(Sci::Position pos) const {
	return starts.PartitionFromPosition(pos);
}
/* CHANGEBAR begin */
void LineVector::EnableChangeCollection(bool changesCollecting_) {
    DeleteChangeCollection();
    changes.EnableChangeCollection(changesCollecting_, Lines());
}

void LineVector::DeleteChangeCollection() {
    changes.ClearChanged();
}

int LineVector::GetChanged(Sci::Line line) const {
    return changes.GetChanged(line);
}

int LineVector::GetChangesEdition() const {
    return changes.GetEdition();
}

void LineVector::SetSavePoint() {
    changes.AdvanceEdition();
}

char *LineVector::PersistantForm() const {
    return changes.PersistantForm();
}

void LineVector::SetChanges(const char *changesState) {
    changes.SetChanges(changesState);
}
/* CHANGEBAR end */

Action::Action() {
	at = startAction;
	position = 0;
	lenData = 0;
	mayCoalesce = false;
}

Action::Action(Action &&other) {
	at = other.at;
	position = other.position;
	data = std::move(other.data);
	lenData = other.lenData;
	mayCoalesce = other.mayCoalesce;
}

Action::~Action() {
}

void Action::Create(actionType at_, Sci::Position position_, const char *data_, Sci::Position lenData_, bool mayCoalesce_) {
	data = nullptr;
	position = position_;
	at = at_;
	if (lenData_) {
		data = std::unique_ptr<char []>(new char[lenData_]);
		memcpy(&data[0], data_, lenData_);
	}
	lenData = lenData_;
	mayCoalesce = mayCoalesce_;
}

void Action::Clear() {
	data = nullptr;
	lenData = 0;
}

// The undo history stores a sequence of user operations that represent the user's view of the
// commands executed on the text.
// Each user operation contains a sequence of text insertion and text deletion actions.
// All the user operations are stored in a list of individual actions with 'start' actions used
// as delimiters between user operations.
// Initially there is one start action in the history.
// As each action is performed, it is recorded in the history. The action may either become
// part of the current user operation or may start a new user operation. If it is to be part of the
// current operation, then it overwrites the current last action. If it is to be part of a new
// operation, it is appended after the current last action.
// After writing the new action, a new start action is appended at the end of the history.
// The decision of whether to start a new user operation is based upon two factors. If a
// compound operation has been explicitly started by calling BeginUndoAction and no matching
// EndUndoAction (these calls nest) has been called, then the action is coalesced into the current
// operation. If there is no outstanding BeginUndoAction call then a new operation is started
// unless it looks as if the new action is caused by the user typing or deleting a stream of text.
// Sequences that look like typing or deletion are coalesced into a single user operation.

UndoHistory::UndoHistory() {

	actions.resize(3);
	maxAction = 0;
	currentAction = 0;
	undoSequenceDepth = 0;
	savePoint = 0;
	tentativePoint = -1;
/* CHANGEBAR begin */
	savePointEffective = 0;

	changeActions = 0;
/* CHANGEBAR end */

	actions[currentAction].Create(startAction);
}

UndoHistory::~UndoHistory() {
/* CHANGEBAR begin */
	DeleteChangeHistory();
/* CHANGEBAR end */
}

void UndoHistory::EnsureUndoRoom() {
	// Have to test that there is room for 2 more actions in the array
	// as two actions may be created by the calling function
	if (static_cast<size_t>(currentAction) >= (actions.size() - 2)) {
		// Run out of undo nodes so extend the array
		actions.resize(actions.size() * 2);

/* CHANGEBAR begin */
		if (changeActions) {
            int lenActionsNew = actions.size();
			int **changeActionsNew = new int *[lenActionsNew];
			if (!changeActionsNew)
				return;
			for (int i=0;i<lenActionsNew;i++) {
				changeActionsNew[i] = (i < lenActionsNew/2) ? changeActions[i] : 0;
			}
			delete []changeActions;
			changeActions = changeActionsNew;
		}
/* CHANGEBAR end */
	}
}

/* CHANGEBAR begin */
const char * UndoHistory::AppendAction(actionType at, Sci::Position position, const char *data, Sci::Position lengthData,
	bool &startSequence, char *persistantChanges, bool mayCoalesce) {
/* CHANGEBAR end */
	EnsureUndoRoom();
	//Platform::DebugPrintf("%% %d action %d %d %d\n", at, position, lengthData, currentAction);
	//Platform::DebugPrintf("^ %d action %d %d\n", actions[currentAction - 1].at,
	//	actions[currentAction - 1].position, actions[currentAction - 1].lenData);
	if (currentAction < savePoint) {
/* CHANGEBAR begin */
		savePointEffective = currentAction;
/* CHANGEBAR end */
		savePoint = -1;
	}
	int oldCurrentAction = currentAction;
	if (currentAction >= 1) {
		if (0 == undoSequenceDepth) {
			// Top level actions may not always be coalesced
			int targetAct = -1;
			const Action *actPrevious = &(actions[currentAction + targetAct]);
			// Container actions may forward the coalesce state of Scintilla Actions.
			while ((actPrevious->at == containerAction) && actPrevious->mayCoalesce) {
				targetAct--;
				actPrevious = &(actions[currentAction + targetAct]);
			}
			// See if current action can be coalesced into previous action
			// Will work if both are inserts or deletes and position is same
			if ((currentAction == savePoint) || (currentAction == tentativePoint)) {
				currentAction++;
			} else if (!actions[currentAction].mayCoalesce) {
				// Not allowed to coalesce if this set
				currentAction++;
			} else if (!mayCoalesce || !actPrevious->mayCoalesce) {
				currentAction++;
			} else if (at == containerAction || actions[currentAction].at == containerAction) {
				;	// A coalescible containerAction
			} else if ((at != actPrevious->at) && (actPrevious->at != startAction)) {
				currentAction++;
			} else if ((at == insertAction) &&
			           (position != (actPrevious->position + actPrevious->lenData))) {
				// Insertions must be immediately after to coalesce
				currentAction++;
			} else if (at == removeAction) {
				if ((lengthData == 1) || (lengthData == 2)) {
					if ((position + lengthData) == actPrevious->position) {
						; // Backspace -> OK
					} else if (position == actPrevious->position) {
						; // Delete -> OK
					} else {
						// Removals must be at same position to coalesce
						currentAction++;
					}
				} else {
					// Removals must be of one character to coalesce
					currentAction++;
				}
			} else {
				// Action coalesced.
			}

		} else {
			// Actions not at top level are always coalesced unless this is after return to top level
			if (!actions[currentAction].mayCoalesce)
				currentAction++;
		}
	} else {
		currentAction++;
	}
	startSequence = oldCurrentAction != currentAction;
	const int actionWithData = currentAction;
	actions[currentAction].Create(at, position, data, lengthData, mayCoalesce);

/* CHANGEBAR begin */
	if (changeActions) {
		delete []changeActions[currentAction];
		changeActions[currentAction] = (int *)persistantChanges;
	}
/* CHANGEBAR end */

	currentAction++;
	actions[currentAction].Create(startAction);
	maxAction = currentAction;
	return actions[actionWithData].data.get();
}

void UndoHistory::BeginUndoAction() {
	EnsureUndoRoom();
	if (undoSequenceDepth == 0) {
		if (actions[currentAction].at != startAction) {
			currentAction++;
			actions[currentAction].Create(startAction);
			maxAction = currentAction;
		}
		actions[currentAction].mayCoalesce = false;
	}
	undoSequenceDepth++;
}

void UndoHistory::EndUndoAction() {
	PLATFORM_ASSERT(undoSequenceDepth > 0);
	EnsureUndoRoom();
	undoSequenceDepth--;
	if (0 == undoSequenceDepth) {
		if (actions[currentAction].at != startAction) {
			currentAction++;
			actions[currentAction].Create(startAction);
			maxAction = currentAction;
		}
		actions[currentAction].mayCoalesce = false;
	}
}

void UndoHistory::DropUndoSequence() {
	undoSequenceDepth = 0;
}

void UndoHistory::DeleteUndoHistory() {
	for (int i = 1; i < maxAction; i++)
		actions[i].Clear();
	maxAction = 0;
	currentAction = 0;
	actions[currentAction].Create(startAction);
	savePoint = 0;
	tentativePoint = -1;
/* CHANGEBAR begin */
	savePointEffective = 0;
/* CHANGEBAR end */
}

/* CHANGEBAR begin */
void UndoHistory::DeleteChangeHistory() {
    if (changeActions) {
        for (std::vector<Action>::size_type i=0;i<actions.size();i++) {
            delete []changeActions[i];
        }
        delete []changeActions;
        changeActions = 0;
    }
}

void UndoHistory::EnableChangeHistory(bool enable) {
    if (enable) {
        if (!changeActions) {
            changeActions = new int *[actions.size()];
            for (std::vector<Action>::size_type i=0;i<actions.size();i++) {
                changeActions[i] = 0;
            }
        }
    } else {
        DeleteChangeHistory();
    }
}
/* CHANGEBAR end */

void UndoHistory::SetSavePoint() {
	savePoint = currentAction;
/* CHANGEBAR begin */
	savePointEffective = currentAction;
/* CHANGEBAR end */
}

bool UndoHistory::IsSavePoint() const {
	return savePoint == currentAction;
}

/* CHANGEBAR begin */
bool UndoHistory::BeforeSavePointEffective(int action) const {
	return action <= savePointEffective;
}
/* CHANGEBAR end */

void UndoHistory::TentativeStart() {
	tentativePoint = currentAction;
}

void UndoHistory::TentativeCommit() {
	tentativePoint = -1;
	// Truncate undo history
	maxAction = currentAction;
}

int UndoHistory::TentativeSteps() {
	// Drop any trailing startAction
	if (actions[currentAction].at == startAction && currentAction > 0)
		currentAction--;
	if (tentativePoint >= 0)
		return currentAction - tentativePoint;
	else
		return -1;
}

bool UndoHistory::CanUndo() const {
	return (currentAction > 0) && (maxAction > 0);
}

int UndoHistory::StartUndo() {
	// Drop any trailing startAction
	if (actions[currentAction].at == startAction && currentAction > 0)
		currentAction--;

	// Count the steps in this action
	int act = currentAction;
	while (actions[act].at != startAction && act > 0) {
		act--;
	}
	return currentAction - act;
}

const Action &UndoHistory::GetUndoStep() const {
	return actions[currentAction];
}

void UndoHistory::CompletedUndoStep() {
	currentAction--;
}

/* CHANGEBAR begin */
char *UndoHistory::GetChangesStep() const {
	return changeActions ? (char *)changeActions[currentAction] : 0;
}
/* CHANGEBAR end */

bool UndoHistory::CanRedo() const {
	return maxAction > currentAction;
}

int UndoHistory::StartRedo() {
	// Drop any leading startAction
	if (currentAction < maxAction && actions[currentAction].at == startAction)
		currentAction++;

	// Count the steps in this action
	int act = currentAction;
	while (act < maxAction && actions[act].at != startAction) {
		act++;
	}
	return act - currentAction;
}

const Action &UndoHistory::GetRedoStep() const {
	return actions[currentAction];
}

void UndoHistory::CompletedRedoStep() {
	currentAction++;
}

/* CHANGEBAR begin */
int UndoHistory::Edition() const {
	return currentAction;
}
/* CHANGEBAR end */

CellBuffer::CellBuffer() {
	readOnly = false;
	utf8LineEnds = 0;
	collectingUndo = true;
}

CellBuffer::~CellBuffer() {
}

char CellBuffer::CharAt(Sci::Position position) const {
	return substance.ValueAt(position);
}

void CellBuffer::GetCharRange(char *buffer, Sci::Position position, Sci::Position lengthRetrieve) const {
	if (lengthRetrieve <= 0)
		return;
	if (position < 0)
		return;
	if ((position + lengthRetrieve) > substance.Length()) {
		Platform::DebugPrintf("Bad GetCharRange %d for %d of %d\n", position,
		                      lengthRetrieve, substance.Length());
		return;
	}
	substance.GetRange(buffer, position, lengthRetrieve);
}

char CellBuffer::StyleAt(Sci::Position position) const {
	return style.ValueAt(position);
}

void CellBuffer::GetStyleRange(unsigned char *buffer, Sci::Position position, Sci::Position lengthRetrieve) const {
	if (lengthRetrieve < 0)
		return;
	if (position < 0)
		return;
	if ((position + lengthRetrieve) > style.Length()) {
		Platform::DebugPrintf("Bad GetStyleRange %d for %d of %d\n", position,
		                      lengthRetrieve, style.Length());
		return;
	}
	style.GetRange(reinterpret_cast<char *>(buffer), position, lengthRetrieve);
}

const char *CellBuffer::BufferPointer() {
	return substance.BufferPointer();
}

const char *CellBuffer::RangePointer(Sci::Position position, Sci::Position rangeLength) {
	return substance.RangePointer(position, rangeLength);
}

Sci::Position CellBuffer::GapPosition() const {
	return substance.GapPosition();
}

// The char* returned is to an allocation owned by the undo history
const char *CellBuffer::InsertString(Sci::Position position, const char *s, Sci::Position insertLength, bool &startSequence) {
	// InsertString and DeleteChars are the bottleneck though which all changes occur
	const char *data = s;
	if (!readOnly) {
		if (collectingUndo) {
			// Save into the undo/redo stack, but only the characters - not the formatting
			// This takes up about half load time
/* CHANGEBAR begin */
			char *persistantForm = lv.PersistantForm();
			data = uh.AppendAction(insertAction, position, s, insertLength, startSequence, persistantForm);
/* CHANGEBAR end */
		}

/* CHANGEBAR begin */
		BasicInsertString(position, s, insertLength, false);
/* CHANGEBAR end */
	}
	return data;
}

bool CellBuffer::SetStyleAt(Sci::Position position, char styleValue) {
	const char curVal = style.ValueAt(position);
	if (curVal != styleValue) {
		style.SetValueAt(position, styleValue);
		return true;
	} else {
		return false;
	}
}

bool CellBuffer::SetStyleFor(Sci::Position position, Sci::Position lengthStyle, char styleValue) {
	bool changed = false;
	PLATFORM_ASSERT(lengthStyle == 0 ||
		(lengthStyle > 0 && lengthStyle + position <= style.Length()));
	while (lengthStyle--) {
		const char curVal = style.ValueAt(position);
		if (curVal != styleValue) {
			style.SetValueAt(position, styleValue);
			changed = true;
		}
		position++;
	}
	return changed;
}

// The char* returned is to an allocation owned by the undo history
const char *CellBuffer::DeleteChars(Sci::Position position, Sci::Position deleteLength, bool &startSequence) {
	// InsertString and DeleteChars are the bottleneck though which all changes occur
	PLATFORM_ASSERT(deleteLength > 0);
	const char *data = 0;
	if (!readOnly) {
		if (collectingUndo) {
			// Save into the undo/redo stack, but only the characters - not the formatting
			// The gap would be moved to position anyway for the deletion so this doesn't cost extra
			data = substance.RangePointer(position, deleteLength);
/* CHANGEBAR begin */
			char *persistantForm = lv.PersistantForm();
			data = uh.AppendAction(removeAction, position, data, deleteLength, startSequence, persistantForm);
/* CHANGEBAR end */
		}

/* CHANGEBAR begin */
		BasicDeleteChars(position, deleteLength, false);
/* CHANGEBAR end */
	}
	return data;
}

Sci::Position CellBuffer::Length() const {
	return substance.Length();
}

void CellBuffer::Allocate(Sci::Position newSize) {
	substance.ReAllocate(newSize);
	style.ReAllocate(newSize);
}

void CellBuffer::SetLineEndTypes(int utf8LineEnds_) {
	if (utf8LineEnds != utf8LineEnds_) {
		utf8LineEnds = utf8LineEnds_;
/* CHANGEBAR begin */
		ResetLineEnds(false);
/* CHANGEBAR end */
	}
}

bool CellBuffer::ContainsLineEnd(const char *s, Sci::Position length) const {
	unsigned char chBeforePrev = 0;
	unsigned char chPrev = 0;
	for (Sci::Position i = 0; i < length; i++) {
		const unsigned char ch = s[i];
		if ((ch == '\r') || (ch == '\n')) {
			return true;
		} else if (utf8LineEnds) {
			const unsigned char back3[3] = { chBeforePrev, chPrev, ch };
			if (UTF8IsSeparator(back3) || UTF8IsNEL(back3 + 1)) {
				return true;
			}
		}
		chBeforePrev = chPrev;
		chPrev = ch;
	}
	return false;
}

void CellBuffer::SetPerLine(PerLine *pl) {
	lv.SetPerLine(pl);
}

Sci::Line CellBuffer::Lines() const {
	return lv.Lines();
}

Sci::Position CellBuffer::LineStart(Sci::Line line) const {
	if (line < 0)
		return 0;
	else if (line >= Lines())
		return Length();
	else
		return lv.LineStart(line);
}

bool CellBuffer::IsReadOnly() const {
	return readOnly;
}

void CellBuffer::SetReadOnly(bool set) {
	readOnly = set;
}

void CellBuffer::SetSavePoint() {
	uh.SetSavePoint();
/* CHANGEBAR begin */
	lv.SetSavePoint();
/* CHANGEBAR end */
}

bool CellBuffer::IsSavePoint() const {
	return uh.IsSavePoint();
}

void CellBuffer::TentativeStart() {
	uh.TentativeStart();
}

void CellBuffer::TentativeCommit() {
	uh.TentativeCommit();
}

int CellBuffer::TentativeSteps() {
	return uh.TentativeSteps();
}

bool CellBuffer::TentativeActive() const {
	return uh.TentativeActive();
}

// Without undo

/* CHANGEBAR begin */
void CellBuffer::InsertLine(Sci::Line line, Sci::Position position, bool lineStart, int edition, bool undoing) {
	lv.InsertLine(line, position, lineStart, edition, undoing);
/* CHANGEBAR end */
}

/* CHANGEBAR begin */
void CellBuffer::RemoveLine(Sci::Line line, bool undoing) {
	lv.RemoveLine(line, undoing);
/* CHANGEBAR end */
}

bool CellBuffer::UTF8LineEndOverlaps(Sci::Position position) const {
	const unsigned char bytes[] = {
		static_cast<unsigned char>(substance.ValueAt(position-2)),
		static_cast<unsigned char>(substance.ValueAt(position-1)),
		static_cast<unsigned char>(substance.ValueAt(position)),
		static_cast<unsigned char>(substance.ValueAt(position+1)),
	};
	return UTF8IsSeparator(bytes) || UTF8IsSeparator(bytes+1) || UTF8IsNEL(bytes+1);
}

/* CHANGEBAR begin */
void CellBuffer::ResetLineEnds(bool undoing) {
/* CHANGEBAR end */
	// Reinitialize line data -- too much work to preserve
	lv.Init();

	Sci::Position position = 0;
	Sci::Position length = Length();
	Sci::Line lineInsert = 1;
	bool atLineStart = true;
/* CHANGEBAR begin */
	lv.InsertText(lineInsert-1, length, uh.Edition(), undoing, false);
/* CHANGEBAR end */
	unsigned char chBeforePrev = 0;
	unsigned char chPrev = 0;
	for (Sci::Position i = 0; i < length; i++) {
		const unsigned char ch = substance.ValueAt(position + i);
		if (ch == '\r') {
/* CHANGEBAR begin */
			InsertLine(lineInsert, (position + i) + 1, atLineStart, uh.Edition(), undoing);
/* CHANGEBAR end */
			lineInsert++;
		} else if (ch == '\n') {
			if (chPrev == '\r') {
				// Patch up what was end of line
				lv.SetLineStart(lineInsert - 1, (position + i) + 1);
			} else {
/* CHANGEBAR begin */
				InsertLine(lineInsert, (position + i) + 1, atLineStart, uh.Edition(), undoing);
/* CHANGEBAR end */
				lineInsert++;
			}
		} else if (utf8LineEnds) {
			const unsigned char back3[3] = {chBeforePrev, chPrev, ch};
			if (UTF8IsSeparator(back3) || UTF8IsNEL(back3+1)) {
/* CHANGEBAR begin */
				InsertLine(lineInsert, (position + i) + 1, atLineStart, uh.Edition(), undoing);
/* CHANGEBAR end */
				lineInsert++;
			}
		}
		chBeforePrev = chPrev;
		chPrev = ch;
	}
}

/* CHANGEBAR begin */
void CellBuffer::BasicInsertString(Sci::Position position, const char *s, Sci::Position insertLength, bool undoing) {
	bool atFileEnd = position == substance.Length();
/* CHANGEBAR end */
	if (insertLength == 0)
		return;
	PLATFORM_ASSERT(insertLength > 0);

	const unsigned char chAfter = substance.ValueAt(position);
	bool breakingUTF8LineEnd = false;
	if (utf8LineEnds && UTF8IsTrailByte(chAfter)) {
		breakingUTF8LineEnd = UTF8LineEndOverlaps(position);
	}

	substance.InsertFromArray(position, s, 0, insertLength);
	style.InsertValue(position, insertLength, 0);

	Sci::Line lineInsert = lv.LineFromPosition(position) + 1;
	bool atLineStart = lv.LineStart(lineInsert-1) == position;
	// Point all the lines after the insertion point further along in the buffer
/* CHANGEBAR begin */
	bool atLineEnd = (lv.LineStart(lineInsert) == position+1) || atFileEnd;
	bool lineUnchanged = (atLineStart && (s[insertLength-1] == '\n')) ||
		(atLineEnd && (s[0] == '\r' || s[0] == '\n'));
	lv.InsertText(lineInsert-1, insertLength, uh.Edition(), undoing, lineUnchanged);
/* CHANGEBAR end */
	unsigned char chBeforePrev = substance.ValueAt(position - 2);
	unsigned char chPrev = substance.ValueAt(position - 1);
	if (chPrev == '\r' && chAfter == '\n') {
		// Splitting up a crlf pair at position
/* CHANGEBAR begin */
		InsertLine(lineInsert, position, false, uh.Edition(), undoing);
/* CHANGEBAR end */
		lineInsert++;
	}
	if (breakingUTF8LineEnd) {
/* CHANGEBAR begin */
		RemoveLine(lineInsert, undoing);
/* CHANGEBAR end */
	}
	unsigned char ch = ' ';
	for (Sci::Position i = 0; i < insertLength; i++) {
		ch = s[i];
		if (ch == '\r') {
/* CHANGEBAR begin */
			InsertLine(lineInsert, (position + i) + 1, atLineStart, uh.Edition(), undoing);
/* CHANGEBAR end */
			lineInsert++;
		} else if (ch == '\n') {
			if (chPrev == '\r') {
				// Patch up what was end of line
				lv.SetLineStart(lineInsert - 1, (position + i) + 1);
			} else {
/* CHANGEBAR begin */
				InsertLine(lineInsert, (position + i) + 1, atLineStart, uh.Edition(), undoing);
/* CHANGEBAR end */
				lineInsert++;
			}
		} else if (utf8LineEnds) {
			const unsigned char back3[3] = {chBeforePrev, chPrev, ch};
			if (UTF8IsSeparator(back3) || UTF8IsNEL(back3+1)) {
/* CHANGEBAR begin */
				InsertLine(lineInsert, (position + i) + 1, atLineStart, uh.Edition(), undoing);
/* CHANGEBAR end */
				lineInsert++;
			}
		}
		chBeforePrev = chPrev;
		chPrev = ch;
	}
	// Joining two lines where last insertion is cr and following substance starts with lf
	if (chAfter == '\n') {
		if (ch == '\r') {
			// End of line already in buffer so drop the newly created one
/* CHANGEBAR begin */
			RemoveLine(lineInsert - 1, undoing);
/* CHANGEBAR end */
		}
	} else if (utf8LineEnds && !UTF8IsAscii(chAfter)) {
		// May have end of UTF-8 line end in buffer and start in insertion
		for (int j = 0; j < UTF8SeparatorLength-1; j++) {
			const unsigned char chAt = substance.ValueAt(position + insertLength + j);
			const unsigned char back3[3] = {chBeforePrev, chPrev, chAt};
			if (UTF8IsSeparator(back3)) {
/* CHANGEBAR begin */
				InsertLine(lineInsert, (position + insertLength + j) + 1, atLineStart, uh.Edition(), undoing);
/* CHANGEBAR end */
				lineInsert++;
			}
			if ((j == 0) && UTF8IsNEL(back3+1)) {
/* CHANGEBAR begin */
				InsertLine(lineInsert, (position + insertLength + j) + 1, atLineStart, uh.Edition(), undoing);
/* CHANGEBAR end */
				lineInsert++;
			}
			chBeforePrev = chPrev;
			chPrev = chAt;
		}
	}
}

/* CHANGEBAR begin */
void CellBuffer::BasicDeleteChars(Sci::Position position, Sci::Position deleteLength, bool undoing) {
/* CHANGEBAR end */
	if (deleteLength == 0)
		return;

	if ((position == 0) && (deleteLength == substance.Length())) {
		// If whole buffer is being deleted, faster to reinitialise lines data
		// than to delete each line.
		lv.Init();
/* CHANGEBAR begin */
		lv.InsertText(0, 0, uh.Edition(), undoing, false);
/* CHANGEBAR end */
	} else {
		// Have to fix up line positions before doing deletion as looking at text in buffer
		// to work out which lines have been removed

		Sci::Line lineRemove = lv.LineFromPosition(position) + 1;
/* CHANGEBAR begin */
		bool atLineEnd = (lv.LineStart(lineRemove) == position+1);
		char chAfter = substance.ValueAt(position + deleteLength);
		bool lineUnchanged = (atLineEnd && (chAfter == '\r' || chAfter == '\n'));
		lv.InsertText(lineRemove-1, - (deleteLength), uh.Edition(), undoing, lineUnchanged);
/* CHANGEBAR end */
		const unsigned char chPrev = substance.ValueAt(position - 1);
		const unsigned char chBefore = chPrev;
		unsigned char chNext = substance.ValueAt(position);
		bool ignoreNL = false;
		if (chPrev == '\r' && chNext == '\n') {
			// Move back one
			lv.SetLineStart(lineRemove, position);
			lineRemove++;
			ignoreNL = true; 	// First \n is not real deletion
		}
		if (utf8LineEnds && UTF8IsTrailByte(chNext)) {
			if (UTF8LineEndOverlaps(position)) {
/* CHANGEBAR begin */
				RemoveLine(lineRemove, undoing);
/* CHANGEBAR end */
			}
		}

		unsigned char ch = chNext;
		for (Sci::Position i = 0; i < deleteLength; i++) {
			chNext = substance.ValueAt(position + i + 1);
			if (ch == '\r') {
				if (chNext != '\n') {
/* CHANGEBAR begin */
					RemoveLine(lineRemove, undoing);
/* CHANGEBAR end */
				}
			} else if (ch == '\n') {
				if (ignoreNL) {
					ignoreNL = false; 	// Further \n are real deletions
				} else {
/* CHANGEBAR begin */
					RemoveLine(lineRemove, undoing);
/* CHANGEBAR end */
				}
			} else if (utf8LineEnds) {
				if (!UTF8IsAscii(ch)) {
					const unsigned char next3[3] = {ch, chNext,
						static_cast<unsigned char>(substance.ValueAt(position + i + 2))};
					if (UTF8IsSeparator(next3) || UTF8IsNEL(next3)) {
/* CHANGEBAR begin */
						RemoveLine(lineRemove, undoing);
/* CHANGEBAR end */
					}
				}
			}

			ch = chNext;
		}
		// May have to fix up end if last deletion causes cr to be next to lf
		// or removes one of a crlf pair
/* CHANGEBAR begin */
		// const char chAfter = substance.ValueAt(position + deleteLength);
/* CHANGEBAR end */
		if (chBefore == '\r' && chAfter == '\n') {
			// Using lineRemove-1 as cr ended line before start of deletion
/* CHANGEBAR begin */
			RemoveLine(lineRemove - 1, undoing);
/* CHANGEBAR end */
			lv.SetLineStart(lineRemove - 1, position + 1);
		}
	}
	substance.DeleteRange(position, deleteLength);
	style.DeleteRange(position, deleteLength);
}

bool CellBuffer::SetUndoCollection(bool collectUndo) {
	collectingUndo = collectUndo;
	uh.DropUndoSequence();
	return collectingUndo;
}

bool CellBuffer::IsCollectingUndo() const {
	return collectingUndo;
}

void CellBuffer::BeginUndoAction() {
	uh.BeginUndoAction();
}

void CellBuffer::EndUndoAction() {
	uh.EndUndoAction();
}

void CellBuffer::AddUndoAction(Sci::Position token, bool mayCoalesce) {
	bool startSequence;
/* CHANGEBAR begin */
	char *persistantForm = lv.PersistantForm();
	uh.AppendAction(containerAction, token, 0, 0, startSequence, persistantForm, mayCoalesce);
/* CHANGEBAR end */
}

/* CHANGEBAR begin */
void CellBuffer::DeleteUndoHistory(bool collectChangeHistory) {
/* CHANGEBAR end */
	uh.DeleteUndoHistory();
/* CHANGEBAR begin */
	uh.EnableChangeHistory(collectChangeHistory);
	lv.EnableChangeCollection(collectChangeHistory);
/* CHANGEBAR end */
}

/* CHANGEBAR begin */
bool CellBuffer::SetChangeCollection(bool collectChange) {
	uh.EnableChangeHistory(collectChange);
	lv.EnableChangeCollection(collectChange);
	return collectChange;
}

void CellBuffer::DeleteChangeCollection() {
	uh.DeleteChangeHistory();
	lv.DeleteChangeCollection();
}
/* CHANGEBAR end */

bool CellBuffer::CanUndo() const {
	return uh.CanUndo();
}

int CellBuffer::StartUndo() {
	return uh.StartUndo();
}

const Action &CellBuffer::GetUndoStep() const {
	return uh.GetUndoStep();
}

void CellBuffer::PerformUndoStep() {
/* CHANGEBAR begin */
	const char *changesState = uh.GetChangesStep();
	lv.SetChanges(changesState);
/* CHANGEBAR end */
	const Action &actionStep = uh.GetUndoStep();
	if (actionStep.at == insertAction) {
		if (substance.Length() < actionStep.lenData) {
			throw std::runtime_error(
				"CellBuffer::PerformUndoStep: deletion must be less than document length.");
		}
/* CHANGEBAR begin */
		BasicDeleteChars(actionStep.position, actionStep.lenData, true);
/* CHANGEBAR end */
	} else if (actionStep.at == removeAction) {
/* CHANGEBAR begin */
		BasicInsertString(actionStep.position, actionStep.data.get(), actionStep.lenData, true);
/* CHANGEBAR end */
	}
	uh.CompletedUndoStep();
}

bool CellBuffer::CanRedo() const {
	return uh.CanRedo();
}

int CellBuffer::StartRedo() {
	return uh.StartRedo();
}

const Action &CellBuffer::GetRedoStep() const {
	return uh.GetRedoStep();
}

void CellBuffer::PerformRedoStep() {
	const Action &actionStep = uh.GetRedoStep();
	if (actionStep.at == insertAction) {
/* CHANGEBAR begin */
		BasicInsertString(actionStep.position, actionStep.data.get(), actionStep.lenData, false);
/* CHANGEBAR end */
	} else if (actionStep.at == removeAction) {
/* CHANGEBAR begin */
		BasicDeleteChars(actionStep.position, actionStep.lenData, false);
/* CHANGEBAR end */
	}
	uh.CompletedRedoStep();
/* CHANGEBAR begin */
	if (IsSavePoint()) {
		lv.SetSavePoint();
	}
/* CHANGEBAR end */
}

/* CHANGEBAR begin */
int CellBuffer::GetChanged(int line) const {
	int changed = lv.GetChanged(line);
	if (changed == 0)
		return 0;
	else if (uh.BeforeSavePointEffective(changed))
		return 2;
	else
		return 1;
}

int CellBuffer::GetChangesEdition() const {
    return lv.GetChangesEdition();
}
/* CHANGEBAR end */
