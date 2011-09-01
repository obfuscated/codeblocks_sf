#ifndef __PARSEACTIONS_H__
#define __PARSEACTIONS_H__

#ifdef __GNUG__
// #pragma interface
#endif


#include <wx/string.h>

//#define string wxString
//#define string std::string

class NassiBrick;

/// /////////////////////////////////////////////////////////////////////
class comment_collector
{
public:
    comment_collector(wxString &str);
    //void operator() ( iterator_t first, iterator_t const& last)const;
    void operator() ( wxChar const *first, wxChar const *last ) const;
private:
    wxString &m_str;
};
class AddSpace_to_collector
{
public:
    AddSpace_to_collector(wxString &str);
    //void operator() (iterator_t first, iterator_t const& last)const;
    void operator() ( wxChar const *first, wxChar const *last ) const;
private:
    wxString &m_str;
};
class instr_collector
{
public:
    instr_collector(wxString &str);
    //void operator() (iterator_t first, iterator_t const& last) const;
    void operator() ( wxChar const *first, wxChar const *last ) const;
    ///
    void operator() (const wxChar *ch)const;
    void operator() (const wxChar ch)const;
    void operator() (wxChar *first)const;
private:
    void remove_carrage_return(void) const;
    wxString &m_str;
};

class AddNewline_to_collector
{
public:
    AddNewline_to_collector(wxString &str);
    //void operator() (iterator_t first, iterator_t const& last)const;
    void operator() ( wxChar const *first, wxChar const *last ) const;
private:
    wxString &m_str;
};
class RemoveDoubleSpaces_from_collector
{
public:
    RemoveDoubleSpaces_from_collector(wxString &str);
    //void operator() (iterator_t first, iterator_t const& last)const;
    void operator() ( wxChar const *first, wxChar const *last ) const;
private:
    wxString &m_str;
};
/// ///////////////////////////////////////////////////////////////////////////
class CreateNassiBreakBrick
{
public:
    CreateNassiBreakBrick(wxString &c_str, wxString &s_str, NassiBrick *&brick);
    //void operator()(iterator_t first, iterator_t const& last)const;
    void operator() ( wxChar const *first, wxChar const *last ) const;
private:
    wxString &m_c_str;
    wxString &m_s_str;
    NassiBrick *&m_brick;
};
class CreateNassiContinueBrick
{
public:
    CreateNassiContinueBrick(wxString &c_str, wxString &s_str, NassiBrick *&brick);
    //void operator()(iterator_t first, iterator_t const& last)const;
    void operator() ( wxChar const *first, wxChar const *last ) const;
private:
    wxString &m_c_str;
    wxString &m_s_str;
    NassiBrick *&m_brick;
};
class CreateNassiReturnBrick
{
public:
    CreateNassiReturnBrick(wxString &c_str, wxString &s_str, NassiBrick *&brick);
    //void operator()(iterator_t first, iterator_t const& last)const;
    void operator() ( wxChar const *first, wxChar const *last ) const;
private:
    wxString &m_c_str;
    wxString &m_s_str;
    NassiBrick *&m_brick;
};
class CreateNassiInstructionBrick
{
public:
    CreateNassiInstructionBrick(wxString &c_str, wxString &s_str, NassiBrick *&brick);
    //void operator()(iterator_t first, iterator_t const& last)const;
    void operator() ( wxChar const *first, wxChar const *last ) const;
private:
    wxString &m_c_str;
    wxString &m_s_str;
    NassiBrick *&m_brick;
};
class CreateNassiBlockBrick
{
public:
    CreateNassiBlockBrick(wxString &c_str, wxString &s_str, NassiBrick *&brick);
    void operator()(const wxChar ch)const;
    void operator() ( wxChar const *first, wxChar const *last ) const;
private:
    wxString &m_c_str;
    wxString &m_s_str;
    NassiBrick *&m_brick;
    void DoCreate() const;
};
class CreateNassiBlockEnd
{
public:
    CreateNassiBlockEnd(wxString &c_str, wxString &s_str, NassiBrick *&brick);
    //void operator()(iterator_t first, iterator_t const& last)const;
    void operator()(const wxChar ch)const;
    void operator() ( wxChar const *first, wxChar const *last ) const;
private:
    wxString &m_c_str;
    wxString &m_s_str;
    NassiBrick *&m_brick;
    void DoEnd() const;
};
class CreateNassiIfBrick
{
public:
    CreateNassiIfBrick(wxString &c_str, wxString &s_str, NassiBrick *&brick);
    //void operator()(iterator_t first, iterator_t const& last)const;
    void operator() ( wxChar const *first, wxChar const *last ) const;
private:
    wxString &m_c_str;
    wxString &m_s_str;
    NassiBrick *&m_brick;
};
class CreateNassiIfEndIfClause
{
public:
    CreateNassiIfEndIfClause(NassiBrick *&brick);
    //void operator()(iterator_t first, iterator_t const& last)const;
    void operator() ( wxChar const *first, wxChar const *last ) const;
private:
    NassiBrick *&m_brick;
};
class CreateNassiIfBeginElseClause
{
public:
    CreateNassiIfBeginElseClause(wxString &c_str, wxString &s_str, NassiBrick *&brick);
    //void operator()(iterator_t first, iterator_t const& last)const;
    void operator() ( wxChar const *first, wxChar const *last ) const;
private:
    wxString &m_c_str;
    wxString &m_s_str;
    NassiBrick *&m_brick;
};
class CreateNassiIfThenText
{
public:
    CreateNassiIfThenText(wxString &c_str, wxString &s_str, NassiBrick *&brick);
    //void operator()(iterator_t first, iterator_t const& last)const;
    void operator() ( wxChar const *first, wxChar const *last ) const;
private:
    wxString &m_c_str;
    wxString &m_s_str;
    NassiBrick *&m_brick;
};
class CreateNassiIfEndElseClause
{
public:
    CreateNassiIfEndElseClause(NassiBrick *&brick);
    //void operator()(iterator_t first, iterator_t const& last)const;
    void operator() ( wxChar const *first, wxChar const *last ) const;
private:
    NassiBrick *&m_brick;
};
class CreateNassiForBrick
{
public:
    CreateNassiForBrick(wxString &c_str, wxString &s_str, NassiBrick *&brick);
    //void operator()(iterator_t first, iterator_t const& last)const;
    void operator() ( wxChar const *first, wxChar const *last ) const;
private:
    wxString &m_c_str;
    wxString &m_s_str;
    NassiBrick *&m_brick;
};
class CreateNassiWhileBrick
{
public:
    CreateNassiWhileBrick(wxString &c_str, wxString &s_str, NassiBrick *&brick);
    //void operator()(iterator_t first, iterator_t const& last)const;
    void operator() ( wxChar const *first, wxChar const *last ) const;
private:
    wxString &m_c_str;
    wxString &m_s_str;
    NassiBrick *&m_brick;
};
class CreateNassiForWhileEnd
{
public:
    CreateNassiForWhileEnd(NassiBrick *&brick);
    //void operator()(iterator_t first, iterator_t const& last)const;
    void operator() ( wxChar const *first, wxChar const *last ) const;
private:
    NassiBrick *&m_brick;
};
class CreateNassiDoWhileBrick
{
public:
    CreateNassiDoWhileBrick(NassiBrick *&brick);
    //void operator()(iterator_t first, iterator_t const& last)const;
    void operator() ( wxChar const *first, wxChar const *last ) const;
private:
    NassiBrick *&m_brick;
};
class CreateNassiDoWhileEnd
{
public:
    CreateNassiDoWhileEnd(wxString &c_str, wxString &s_str, NassiBrick *&brick);
    //void operator()(iterator_t first, iterator_t const& last)const;
    void operator() ( wxChar const *first, wxChar const *last ) const;
private:
    wxString &m_c_str;
    wxString &m_s_str;
    NassiBrick *&m_brick;
};
class CreateNassiSwitchBrick
{
public:
    CreateNassiSwitchBrick(wxString &c_str, wxString &s_str, NassiBrick *&brick);
    //void operator()(iterator_t first, iterator_t const& last)const;
    void operator() ( wxChar const *first, wxChar const *last ) const;
private:
    wxString &m_c_str;
    wxString &m_s_str;
    NassiBrick *&m_brick;
};
class CreateNassiSwitchEnd
{
public:
    CreateNassiSwitchEnd( NassiBrick *&brick);
    //void operator()(iterator_t first, iterator_t const& last)const;
    void operator() ( wxChar const *first, wxChar const *last ) const;
private:
    NassiBrick *&m_brick;
};
class CreateNassiSwitchChild
{
public:
    CreateNassiSwitchChild(wxString &c_str, wxString &s_str, NassiBrick *&brick);
    //void operator()(iterator_t first, iterator_t const& last)const;
    void operator() ( wxChar const *first, wxChar const *last ) const;
private:
    wxString &m_c_str;
    wxString &m_s_str;
    NassiBrick *&m_brick;
};



#endif
