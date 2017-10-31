#ifndef HEADER_A5EC72EAD78DC2F5
#define HEADER_A5EC72EAD78DC2F5

//#include <string>
//#include <iostream>

#include <boost/spirit/core.hpp>
#include <boost/spirit/symbols/symbols.hpp>
#include <boost/spirit/utility/chset.hpp>
#include <boost/spirit/utility/escape_char.hpp>
#include <boost/spirit/utility/confix.hpp>


#include "parseactions.h"

using namespace std;
using namespace boost::spirit;
extern NassiBrick *brickptr;
extern wxString comment_str;
extern wxString source_str;


//NassiGrammar grammar;
    //{
        rule<rule_t> identifier, preprocessor, comment, cstr;
        // () //brackets_expression, // [] //braces_expression; // {}
        rule<rule_t> parentheses_expression;

        rule<rule_t> break_instr, continue_instr, return_instr, simple_instr, if_instr,
               for_instr, while_instr, dowhile_instr, switch_instr;

        rule<rule_t> instruction;
        rule<rule_t> block;
        rule<rule_t> switch_head, switch_body, switch_case;
        identifier     = ((alpha_p | '_') >> *(alnum_p | '_'));
            preprocessor   = '#' >> *space_p >> identifier;
            comment        = +((comment_p("//") | comment_p("/*", "*/")));
            cstr         = confix_p('"', *c_escape_ch_p, '"');
            parentheses_expression =
                    confix_p(ch_p('('),
                    *(comment | cstr | parentheses_expression | anychar_p),
                    ch_p(')'));
            instruction       = *( comment | space_p )[comment_collector(comment_str)] >>
                  ( break_instr[CreateNassiBreakBrick(comment_str, source_str, brickptr)]       |
                    continue_instr[CreateNassiContinueBrick(comment_str, source_str, brickptr)] |
                    return_instr[CreateNassiReturnBrick(comment_str, source_str, brickptr)]     |
                    if_instr |
                    switch_instr |
                    for_instr |
                    while_instr |
                    dowhile_instr |
                    simple_instr[CreateNassiInstructionBrick(comment_str, source_str, brickptr)]
                  );
            block   = *( comment | space_p )[comment_collector(comment_str)] >>
               eps_p[CreateNassiBlockBrick(comment_str, source_str, brickptr)] >>
               confix_p(ch_p('{'), *( instruction | block ), ch_p('}')) >>
               eps_p[CreateNassiBlockEnd(brickptr)];
            break_instr     = as_lower_d[str_p("break")[instr_collector(source_str)]]            >> (eps_p - (alnum_p | '_')) >> *( comment | space_p )[comment_collector(comment_str)] >> ch_p(';')[instr_collector(source_str)];
            continue_instr  = as_lower_d[str_p("continue")[instr_collector(source_str)]]         >> (eps_p - (alnum_p | '_')) >> *( comment | space_p )[comment_collector(comment_str)] >> ch_p(';')[instr_collector(source_str)];
            return_instr    = confix_p(as_lower_d[str_p("return")]  >> (eps_p - (alnum_p | '_')),
                                       *( comment | cstr | anychar_p ),
                                       ch_p(';'))[instr_collector(source_str)];

            /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// ///

            simple_instr    = !( (preprocessor | ( anychar_p >> !ch_p(';') ) ) - comment )[instr_collector(source_str)]; //optional

            if_instr        = (str_p("if")[instr_collector(source_str)] >> ( eps_p - ( alnum_p | '_' ) ) >> *( comment | space_p )[comment_collector(comment_str)] >>
                              parentheses_expression[instr_collector(source_str)] >>
                              *(comment | space_p)[comment_collector(comment_str)])[CreateNassiIfBrick(comment_str, source_str, brickptr)] >>
                              (instruction | block | ch_p(';')) >>
                              eps_p[CreateNassiIfEndIfClause(brickptr)] >>
                              !(*(comment | space_p)[comment_collector(comment_str)] >>
                                (str_p("else")[instr_collector(source_str)] >> (eps_p - (alnum_p | '_')) >>
                                 *( comment | space_p )[comment_collector(comment_str)])[CreateNassiIfBeginElseClause(comment_str, source_str, brickptr)] >>
                              (instruction | block | ch_p(';')) >> eps_p[CreateNassiIfEndElseClause(brickptr)]
                              );

            for_instr       = (str_p("for")[instr_collector(source_str)] >> (eps_p - (alnum_p | '_')) >> *( comment | space_p )[comment_collector(comment_str)] >>
                              parentheses_expression[instr_collector(source_str)] >>
                              *(comment | space_p)[comment_collector(comment_str)])[CreateNassiForBrick(comment_str, source_str, brickptr)] >>
                              (instruction | block | ch_p(';')) >> eps_p[CreateNassiForWhileEnd(brickptr)];

            while_instr     = (str_p("while")[instr_collector(source_str)] >> (eps_p - (alnum_p | '_')) >> *( comment | space_p )[comment_collector(comment_str)] >>
                              parentheses_expression[instr_collector(source_str)] >>
                              *(comment | space_p)[comment_collector(comment_str)])[CreateNassiWhileBrick(comment_str, source_str, brickptr)] >>
                              (instruction | block | ch_p(';')) >> eps_p[CreateNassiForWhileEnd(brickptr)];

            dowhile_instr   = str_p("do") >> (eps_p - (alnum_p | '_')) >> *( comment | space_p )[comment_collector(comment_str)] >>
                              eps_p[CreateNassiDoWhileBrick(brickptr)] >>
                              (instruction | block ) >>
                              (*( comment | space_p )[comment_collector(comment_str)] >> str_p("while")[instr_collector(source_str)] >> (eps_p- (alnum_p | '_')) >> *( comment | space_p )[comment_collector(comment_str)] >>
                              parentheses_expression[instr_collector(source_str)] >> *( comment | space_p )[comment_collector(comment_str)] >> ch_p(';'))[CreateNassiDoWhileEnd(comment_str, source_str, brickptr)];


            switch_instr    = switch_head[CreateNassiSwitchBrick(comment_str, source_str, brickptr)] >>
                              switch_body >>
                              eps_p[CreateNassiSwitchEnd(brickptr)];
            switch_head     = str_p("switch")[instr_collector(source_str)] >> (eps_p - (alnum_p | '_')) >> *( comment | space_p )[comment_collector(comment_str)] >>
                              parentheses_expression[instr_collector(source_str)] >> *( comment | space_p )[comment_collector(comment_str)];
            switch_body     = confix_p(ch_p('{'),
                              *(switch_case[CreateNassiSwitchChild(comment_str, source_str, brickptr)] >> *(instruction | block)),
                              ch_p('}'));

            switch_case     = *( comment | space_p )[comment_collector(comment_str)] >>
                              (confix_p(str_p("case")[instr_collector(source_str)] >>  (eps_p - (alnum_p | '_')),  *( (comment | space_p )[comment_collector(comment_str)] |anychar_p[instr_collector(source_str)]), ch_p(':')[instr_collector(source_str)]) |
                               confix_p(str_p("default")[instr_collector(source_str)] >>  (eps_p - (alnum_p | '_')),  *(comment|space_p)[comment_collector(comment_str)], ch_p(':')[instr_collector(source_str)])
                              );
    //}

#endif // header guard
