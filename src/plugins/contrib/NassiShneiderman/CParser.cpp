
#include "NassiEditorPanel.h"

//parser-generator framework:
#include <boost/spirit/include/classic.hpp>
#include <boost/spirit/include/classic_core.hpp>
#include <boost/spirit/include/classic_symbols.hpp>
#include <boost/spirit/include/classic_confix.hpp>

using namespace boost::spirit::classic;


#include "parseactions.h"
#include "bricks.h"
#include "commands.h"
class NassiBrick;


bool NassiEditorPanel::ParseC(const wxString &str)
{
    wxString comment_str, source_str;
    NassiBrick *brickptr;

    typedef rule<scanner<wxChar const *> > rule_t;

    const wxChar *buf = str.wc_str();

    //wxMessageBox(str, _T("parsing:"));
    rule_t preprocessor      = comment_p( _T("#") );
    rule_t cpp_comment       = comment_p( _T("//") );                      // C++ comment
    rule_t c_comment         = comment_p( _T("/*"), _T("*/") );            // C comment
    rule_t cstr              = confix_p(_T('"'), *c_escape_ch_p, _T('"')); // string
    rule_t comment           = c_comment | cpp_comment;
    rule_t comment_collected = comment[comment_collector(comment_str)];
    rule_t parentheseshelper = (
                confix_p(ch_p(_T('(')),
                *(comment_collected | cstr | parentheseshelper | anychar_p),
                ch_p(_T(')')))  );
    rule_t parentheses       = parentheseshelper[instr_collector(source_str)];
    rule_t keywordend        = (eps_p - (alnum_p | _T('_')));
    rule_t spaces            = *(space_p | comment_collected);
    rule_t instruction;
    rule_t break_instr, continue_instr, return_instr, block, if_instr, for_instr, while_instr, dowhile_instr;
    rule_t switch_instr, switch_head, switch_body, switch_case, other_instr;
    break_instr     = str_p(_T("break"))    >> keywordend >> spaces >> ch_p(_T(';'));
    continue_instr  = str_p(_T("continue")) >> keywordend >> spaces >> ch_p(_T(';'));
    return_instr    = confix_p(str_p(_T("return"))  >> keywordend,
                           *( comment_collected | cstr[instr_collector(source_str)] | (anychar_p-ch_p(_T(';')))[instr_collector(source_str)] ),
                           ch_p(_T(';')));
    block = *space_p >>
            ch_p(_T('{'))[CreateNassiBlockBrick(comment_str, source_str, brickptr)] >>
            *( instruction | block ) >>
            *space_p >>
            ch_p(_T('}'))[CreateNassiBlockEnd(comment_str, source_str, brickptr)];

    if_instr        = (str_p(_T("if")) >> keywordend >> spaces >>
                      parentheses >> spaces)[CreateNassiIfBrick(comment_str, source_str, brickptr)] >>
                      (instruction | block | ch_p(_T(';'))) >> eps_p[CreateNassiIfEndIfClause(brickptr)] >>
                      !(spaces >> (str_p(_T("else")) >> keywordend >> spaces)[CreateNassiIfBeginElseClause(comment_str, source_str, brickptr)] >>
                      (instruction | block | ch_p(_T(';'))) >> eps_p[CreateNassiIfEndElseClause(brickptr)]);

    for_instr       = (str_p(_T("for")) >> keywordend >> spaces >>
                      parentheses >> spaces)[CreateNassiForBrick(comment_str, source_str, brickptr)] >>
                      (instruction | block | ch_p(_T(';'))) >> eps_p[CreateNassiForWhileEnd(brickptr)];

    while_instr     = (str_p(_T("while")) >> keywordend >> spaces >>
                      parentheses >> spaces)[CreateNassiWhileBrick(comment_str, source_str, brickptr)] >>
                      (instruction | block | ch_p(_T(';'))) >> eps_p[CreateNassiForWhileEnd(brickptr)];

    dowhile_instr   = str_p(_T("do")) >> keywordend >> spaces >> eps_p[CreateNassiDoWhileBrick(brickptr)] >>
                      (instruction | block ) >>
                      (spaces >> str_p(_T("while")) >> keywordend >> spaces >>
                      parentheses >> spaces >> ch_p(_T(';')))[CreateNassiDoWhileEnd(comment_str, source_str, brickptr)];

    switch_instr    = switch_head[CreateNassiSwitchBrick(comment_str, source_str, brickptr)] >>
                      switch_body >>
                      eps_p[CreateNassiSwitchEnd(brickptr)];
    switch_head     = str_p(_T("switch")) >> keywordend >> spaces >>
                      parentheses >> spaces;
    switch_body     = ch_p(_T('{')) >>
                      *(switch_case[CreateNassiSwitchChild(comment_str, source_str, brickptr)] >> *(instruction | block)) >>
                      *space_p >> ch_p(_T('}'));

    switch_case     = spaces >>
                      ( confix_p(str_p(_T("case"))[instr_collector(source_str)] >> keywordend,
                        //*( spaces | anychar_p[instr_collector(source_str)]),
                        *( comment_collected | cstr[instr_collector(source_str)] | (anychar_p-ch_p(_T(':')))[instr_collector(source_str)] ),
                        ch_p(_T(':'))[instr_collector(source_str)]) |
                       (str_p(_T("default"))[instr_collector(source_str)] >>  keywordend >>
                       spaces >>
                       ch_p(_T(':'))[instr_collector(source_str)]));

    rule_t special_w = (str_p(_T("break"))   |
                        str_p(_T("continue"))|
                        str_p(_T("return"))  |
                        str_p(_T("if"))      |
                        str_p(_T("for"))     |
                        str_p(_T("while"))   |
                        str_p(_T("do"))      |
                        str_p(_T("switch"))  |
                        str_p(_T("case"))    |
                        str_p(_T("default")) ) >> keywordend;

    other_instr    = ( preprocessor |
                       (*
                          (cstr|( anychar_p -
                            (
                                comment              |
                                ch_p(_T(';'))        |
                                ch_p(_T('{'))        |
                                ch_p(_T('}'))        |
                                special_w
                            )
                          ))
                          >> ch_p(_T(';')) )
                      )[instr_collector(source_str)];

    instruction =
        spaces >>
        (
            break_instr[CreateNassiBreakBrick(comment_str, source_str, brickptr)]        |
            continue_instr[CreateNassiContinueBrick(comment_str, source_str, brickptr)]  |
            return_instr[CreateNassiReturnBrick(comment_str, source_str, brickptr)]      |
            if_instr                                                                     |
            for_instr                                                                    |
            while_instr                                                                  |
            dowhile_instr                                                                |
            switch_instr                                                                 |
            block                                                                        |
            other_instr[CreateNassiInstructionBrick(comment_str, source_str, brickptr)]
        );

    NassiBrick *rootbrick = new NassiInstructionBrick();
    brickptr = rootbrick;

    // Parse
    parse_info< const wxChar * > info =
    parse(
        buf, *instruction >> *space_p
    );

    /// check if the whole input was parsed.
    if(!info.full)//failed
    {
        delete rootbrick;
        return false;
    }
    else
    {
        NassiBrick *brk = rootbrick->SetNext(0);
        delete rootbrick;
        rootbrick = brk;
        wxCommandProcessor *prc = m_filecontent->GetCommandProcessor();
        NassiInsertFirstBrick *cmd =
            new NassiInsertFirstBrick( (NassiFileContent *)m_filecontent, rootbrick, false );
        prc->Submit( cmd );
        return true;
    }
}
