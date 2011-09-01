#ifndef HEADER_8420C535795AC84B
#define HEADER_8420C535795AC84B

#ifdef __GNUG__
// #pragma interface
#endif

#ifndef __COMMAND_H__
#define __COMMAND_H__

#include <wx/cmdproc.h>

#include "bricks.h"
#include "NassiFileContent.h"

class NassiFileContent;

class NassiEditTextCommand: public wxCommand
{
public:
    NassiEditTextCommand( NassiFileContent *nfc, NassiBrick *brick, const wxString &str, wxInt32 nmbr);
    virtual ~NassiEditTextCommand(void);

    bool Do(void);
    bool Undo(void);
protected:
    NassiBrick *m_brick;
    wxString m_str;
    wxInt32 m_nmbr;
    // data
    NassiFileContent *m_nfc;
private:
    NassiEditTextCommand(const NassiEditTextCommand &p);
    NassiEditTextCommand &operator=(const NassiEditTextCommand &rhs);
};
class NassiAddChildIndicatorCommand: public wxCommand
{
public:
    NassiAddChildIndicatorCommand( NassiFileContent *nfc, NassiBrick *brick, NassiBrick *InsrBrick, wxUint32 ChildAddNumber, wxString _strc = _T(""), wxString _strs = _T("") );
    virtual ~NassiAddChildIndicatorCommand(void);
    bool Do(void);
    bool Undo(void);

protected:
    NassiFileContent *m_nfc;
    NassiBrick *m_brick;
    bool m_done;
    wxUint32 m_ChildAddNumber;
    NassiBrick *m_nbrk;
    NassiBrick *m_nlbrk;
    wxString strc, strs;
private:
    NassiAddChildIndicatorCommand(const NassiAddChildIndicatorCommand &p);
    NassiAddChildIndicatorCommand &operator=(const NassiAddChildIndicatorCommand &rhs);
};
class NassiInsertFirstBrick:public wxCommand
{
protected:
    NassiFileContent *m_nfc;
    bool m_done;
    NassiBrick *m_nbrk;
    NassiBrick *m_nlbrk;
public:
    NassiInsertFirstBrick( NassiFileContent *nfc, NassiBrick *InsrBrick,  bool CanUndo = true);
    virtual ~NassiInsertFirstBrick(void);
    bool Do(void);
    bool Undo(void);
private:
    NassiInsertFirstBrick(const NassiInsertFirstBrick &p);
    NassiInsertFirstBrick &operator=(const NassiInsertFirstBrick &rhs);
};
class NassiInsertChildBrickCommand: public wxCommand
{
protected:
    NassiFileContent *m_nfc;
    NassiBrick *m_brick;
    bool m_done;
    NassiBrick *m_nbrk;
    NassiBrick *m_nlbrk;
    wxUint32 m_childNumber;
public :
    NassiInsertChildBrickCommand( NassiFileContent *nfc, NassiBrick *brick, NassiBrick *InsrBrick, wxUint32 ChildNumber);
    virtual ~NassiInsertChildBrickCommand(void);
    bool Do(void);
    bool Undo(void);
private:
    NassiInsertChildBrickCommand(const NassiInsertChildBrickCommand &p);
    NassiInsertChildBrickCommand &operator=(const NassiInsertChildBrickCommand &rhs);
};

class NassiInsertBrickBefore : public wxCommand
{
protected:
    NassiFileContent *m_nfc;
    NassiBrick *m_brick;
    bool m_done;
    NassiBrick *m_nbrk;
    NassiBrick *m_nlbrk;
public:
    NassiInsertBrickBefore( NassiFileContent *nfc, NassiBrick *brick, NassiBrick *InsrBrick);
    virtual ~NassiInsertBrickBefore(void);
    bool Do(void);
    bool Undo(void);
private:
    NassiInsertBrickBefore(const NassiInsertBrickBefore &p);
    NassiInsertBrickBefore &operator=(const NassiInsertBrickBefore &rhs);
};
class NassiInsertBrickAfter : public wxCommand
{
protected:
    NassiFileContent *m_nfc;
    NassiBrick *m_brick;
    bool m_done;
    NassiBrick *m_nbrk;
    NassiBrick *m_nlbrk;
public:
    NassiInsertBrickAfter( NassiFileContent *nfc, NassiBrick *brick, NassiBrick *InsrBrick);
    virtual ~NassiInsertBrickAfter(void);
    bool Do(void);
    bool Undo(void);
private:
    NassiInsertBrickAfter(const NassiInsertBrickAfter &p);
    NassiInsertBrickAfter &operator=(const NassiInsertBrickAfter &rhs);
};
class NassiDeleteCommand : public wxCommand
{
protected:
    NassiFileContent *m_nfc;
    NassiBrick *m_first;
    NassiBrick *m_last;
    wxInt32 m_function;
    bool m_done;
    wxInt32 m_childnmbr;
    NassiBrick *parPrev;
    wxString strc, strs;
    //bool m_isdndmove;
    bool firstCall;
public:
    NassiDeleteCommand( NassiFileContent *nfc, NassiBrick *first, NassiBrick *last);
    virtual ~NassiDeleteCommand(void);
    bool Do(void);
    bool Undo(void);
private:
    NassiDeleteCommand(const NassiDeleteCommand &p);
    NassiDeleteCommand &operator=(const NassiDeleteCommand &rhs);
};
class NassiDeleteChildRootCommand : public wxCommand
{
protected:
    NassiFileContent *m_nfc;
    NassiBrick *m_parent;
    wxString m_strC, m_strS;
    bool m_done;
    wxCommand *m_delcmd;
    wxInt32 m_childNmbr;
public:
    NassiDeleteChildRootCommand(NassiFileContent *nfc, NassiBrick *parent, wxInt32 childNumber );
    virtual ~NassiDeleteChildRootCommand(void);
    bool Do(void);
    bool Undo(void);
private:
    NassiDeleteChildRootCommand(const NassiDeleteChildRootCommand &p);
    NassiDeleteChildRootCommand &operator=(const NassiDeleteChildRootCommand &rhs);
};
class NassiMoveBrick : public wxCommand
{
public:
    NassiMoveBrick( wxCommand *addCmd, wxCommand *delCmd);
    virtual ~NassiMoveBrick(void);
    bool Do(void);
    bool Undo(void);
protected:
    wxCommand *m_addCmd;
    wxCommand *m_delCmd;
private:
    NassiMoveBrick(const NassiMoveBrick &p);
    NassiMoveBrick &operator=(const NassiMoveBrick &rhs);
};


#endif

#endif // header guard 
