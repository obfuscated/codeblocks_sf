/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <sdk.h>

#include <stack>

#ifndef CB_PRECOMP
    #include <wx/wx.h>
#endif

#include <logmanager.h>
#include <manager.h>

#include "cclogger.h"
#include "expression.h"
#include "token.h"

#define CC_EXPRESSION_DEBUG_OUTPUT 0

#if defined(CC_GLOBAL_DEBUG_OUTPUT)
    #if CC_GLOBAL_DEBUG_OUTPUT == 1
        #undef CC_EXPRESSION_DEBUG_OUTPUT
        #define CC_EXPRESSION_DEBUG_OUTPUT 1
    #elif CC_GLOBAL_DEBUG_OUTPUT == 2
        #undef CC_EXPRESSION_DEBUG_OUTPUT
        #define CC_EXPRESSION_DEBUG_OUTPUT 2
    #endif
#endif

#ifdef CC_PARSER_TEST
    #define TRACE(format, args...) \
            CCLogger::Get()->DebugLog(F(format, ##args))
    #define TRACE2(format, args...) \
            CCLogger::Get()->DebugLog(F(format, ##args))
#else
    #if CC_EXPRESSION_DEBUG_OUTPUT == 1
        #define TRACE(format, args...) \
            CCLogger::Get()->DebugLog(F(format, ##args))
        #define TRACE2(format, args...)
    #elif CC_EXPRESSION_DEBUG_OUTPUT == 2
    #define TRACE(format, args...)                                              \
        do                                                                      \
        {                                                                       \
            if (g_EnableDebugTrace)                                             \
                CCLogger::Get()->DebugLog(F(format, ##args));                   \
        }                                                                       \
        while (false)
        #define TRACE2(format, args...) \
            CCLogger::Get()->DebugLog(F(format, ##args))
    #else
        #define TRACE(format, args...)
        #define TRACE2(format, args...)
    #endif
#endif

namespace ExpressionConsts
{
    const wxString Plus         (_T("+"));
    const wxString Subtract     (_T("-"));
    const wxString Multiply     (_T("*"));
    const wxString Divide       (_T("/"));
    const wxString LParenthesis (_T("("));
    const wxString RParenthesis (_T(")"));
    const wxString Mod          (_T("%"));
    const wxString Power        (_T("^"));
    const wxString BitwiseAnd   (_T("&"));
    const wxString BitwiseOr    (_T("|"));
    const wxString And          (_T("&&"));
    const wxString Or           (_T("||"));
    const wxString Not          (_T("!"));
    const wxString Equal        (_T("=="));
    const wxString Unequal      (_T("!="));
    const wxString GT           (_T(">"));
    const wxString LT           (_T("<"));
    const wxString GTOrEqual    (_T(">="));
    const wxString LTOrEqual    (_T("<="));
    const wxString LShift       (_T("<<"));
    const wxString RShift       (_T(">>"));
}

ExpressionNode::ExpressionNode()
{
    Initialize(wxEmptyString);
}

void ExpressionNode::Initialize(wxString token)
{
    m_UnaryOperator = false;
    m_Token = token;
    m_Type = ParseNodeType(m_Token);
    m_Priority = GetNodeTypePriority(m_Type);
}

ExpressionNode::ExpressionNodeType ExpressionNode::ParseNodeType(wxString token)
{
    if      (token.IsEmpty())                         return ExpressionNode::Unknown;
    else if (token == ExpressionConsts::Plus)         return ExpressionNode::Plus;
    else if (token == ExpressionConsts::Subtract)     return ExpressionNode::Subtract;
    else if (token == ExpressionConsts::Multiply)     return ExpressionNode::Multiply;
    else if (token == ExpressionConsts::Divide)       return ExpressionNode::Divide;
    else if (token == ExpressionConsts::Mod)          return ExpressionNode::Mod;
    else if (token == ExpressionConsts::Power)        return ExpressionNode::Power;
    else if (token == ExpressionConsts::LParenthesis) return ExpressionNode::LParenthesis;
    else if (token == ExpressionConsts::RParenthesis) return ExpressionNode::RParenthesis;
    else if (token == ExpressionConsts::BitwiseAnd)   return ExpressionNode::BitwiseAnd;
    else if (token == ExpressionConsts::BitwiseOr)    return ExpressionNode::BitwiseOr;
    else if (token == ExpressionConsts::And)          return ExpressionNode::And;
    else if (token == ExpressionConsts::Or)           return ExpressionNode::Or;
    else if (token == ExpressionConsts::Not)          return ExpressionNode::Not;
    else if (token == ExpressionConsts::Equal)        return ExpressionNode::Equal;
    else if (token == ExpressionConsts::Unequal)      return ExpressionNode::Unequal;
    else if (token == ExpressionConsts::GT)           return ExpressionNode::GT;
    else if (token == ExpressionConsts::LT)           return ExpressionNode::LT;
    else if (token == ExpressionConsts::GTOrEqual)    return ExpressionNode::GTOrEqual;
    else if (token == ExpressionConsts::LTOrEqual)    return ExpressionNode::LTOrEqual;
    else if (token == ExpressionConsts::LShift)       return ExpressionNode::LShift;
    else if (token == ExpressionConsts::RShift)       return ExpressionNode::RShift;
    else if (token == ExpressionConsts::Plus)         return ExpressionNode::Plus;
    else
    {
        if (wxIsdigit(token[0]))                      return ExpressionNode::Numeric;
        else                                          return ExpressionNode::Unknown;
    }
}

long ExpressionNode::GetNodeTypePriority(ExpressionNodeType type)
{
    switch (type)
    {
        case LParenthesis:
        case RParenthesis: return 10;
        case Not:          return 9;
        case Mod:          return 8;
        case Multiply:
        case Divide:
        case Power:        return 7;
        case Plus:
        case Subtract:     return 6;
        case LShift:
        case RShift:       return 5;
        case BitwiseAnd:
        case BitwiseOr:    return 4;
        case Equal:
        case Unequal:
        case GT:
        case LT:
        case GTOrEqual:
        case LTOrEqual:    return 3;
        case And:          return 2;
        case Or:           return 1;
        case Numeric:
        case Unknown:
        default:           return 0;
    }
}

bool ExpressionNode::IsUnaryNode(ExpressionNodeType type)
{
    switch (type)
    {
        case ExpressionNode::Plus:
        case ExpressionNode::Subtract:
        case ExpressionNode::Not:
            return true;
        case ExpressionNode::Unknown:
        case ExpressionNode::Multiply:
        case ExpressionNode::Divide:
        case ExpressionNode::LParenthesis:
        case ExpressionNode::RParenthesis:
        case ExpressionNode::Mod:
        case ExpressionNode::Power:
        case ExpressionNode::BitwiseAnd:
        case ExpressionNode::BitwiseOr:
        case ExpressionNode::And:
        case ExpressionNode::Or:
        case ExpressionNode::Equal:
        case ExpressionNode::Unequal:
        case ExpressionNode::GT:
        case ExpressionNode::LT:
        case ExpressionNode::GTOrEqual:
        case ExpressionNode::LTOrEqual:
        case ExpressionNode::LShift:
        case ExpressionNode::RShift:
        case ExpressionNode::Numeric:
        default:
            return false;
    }
}


bool ExpressionNode::IsBinaryOperator(wxString first, wxString second)
{
    switch ((wxChar)first.GetChar(0))
    {
        case _T('&'):
        case _T('|'):
        case _T('='):
        case _T('!'):
        case _T('>'):
        case _T('<'):
        {
            wxString newOperator(first + second);
            if (newOperator == ExpressionConsts::And ||
            newOperator == ExpressionConsts::Or ||
            newOperator == ExpressionConsts::Equal ||
            newOperator == ExpressionConsts::Unequal ||
            newOperator == ExpressionConsts::GTOrEqual ||
            newOperator == ExpressionConsts::LTOrEqual ||
            newOperator == ExpressionConsts::LShift ||
            newOperator == ExpressionConsts::RShift)
                return true;
            else
                return false;
        }
        default:
            return false;
    }
}

void Expression::Clear()
{
    m_InfixExpression.clear();
    m_PostfixExpression.clear();
}

void Expression::AddToInfixExpression(wxString token)
{
    if (token.IsEmpty())
        return;

    if (!m_InfixExpression.empty())
    {
        wxString& lastToken = m_InfixExpression[m_InfixExpression.size() - 1];
        if (ExpressionNode::IsBinaryOperator(lastToken, token))
        {
            lastToken += token;
            return;
        }
    }

    m_InfixExpression.push_back(token);
}

void Expression::ConvertInfixToPostfix()
{
    if (!m_PostfixExpression.empty() || m_InfixExpression.empty())
        return;

    m_Result = true;
    m_Status = true;

    std::stack<ExpressionNode> stackOperator;
    ExpressionNode::ExpressionNodeType lastType = ExpressionNode::Unknown;
    for (PostfixVector::size_type i = 0; i < m_InfixExpression.size(); ++i)
    {
        ExpressionNode expNode;
        expNode.Initialize(m_InfixExpression[i]);
        const ExpressionNode::ExpressionNodeType type = expNode.GetType();
        if (type == ExpressionNode::Numeric)
        {
            // Operand, add to postfix expression
            m_PostfixExpression.push_back(expNode);
            while (!stackOperator.empty() && stackOperator.top().IsUnaryOperator())
            {
                m_PostfixExpression.push_back(stackOperator.top());
                stackOperator.pop();
            }
        }
        else if (type == ExpressionNode::LParenthesis)
        {
            // Left Parentheses, add to stack
            stackOperator.push(expNode);
        }
        else if (type == ExpressionNode::RParenthesis)
        {
            // Right Parentheses, reverse search the Left Parentheses, add all operator of the middle
            ExpressionNode node;
            while (!stackOperator.empty())
            {
                node = stackOperator.top();
                stackOperator.pop();
                if (node.GetType() == ExpressionNode::LParenthesis)
                {
                    while (!stackOperator.empty() && stackOperator.top().IsUnaryOperator())
                    {
                        m_PostfixExpression.push_back(stackOperator.top());
                        stackOperator.pop();
                    }
                    break;
                }
                else
                    m_PostfixExpression.push_back(node);
            }
            // The lastest node must be Left Parentheses
            if (node.GetType() != ExpressionNode::LParenthesis)
            {
                m_Status = false;
            }
        }
        else
        {
            if (ExpressionNode::IsUnaryNode(type) && (m_PostfixExpression.empty() ||
                                                        (lastType != ExpressionNode::Unknown &&
                                                         lastType != ExpressionNode::RParenthesis &&
                                                         lastType != ExpressionNode::Numeric)))
            {
                expNode.SetUnaryOperator();
                stackOperator.push(expNode);
            }
            else if (stackOperator.empty())
            {
                stackOperator.push(expNode);
            }
            else
            {
                ExpressionNode beforeExpNode = stackOperator.top();
                if (beforeExpNode.GetType() != ExpressionNode::LParenthesis &&
                    beforeExpNode.GetPriority() >= expNode.GetPriority())
                {
                    m_PostfixExpression.push_back(beforeExpNode);
                    stackOperator.pop();
                }

                stackOperator.push(expNode);
            }
        }

        lastType = type;
    }

    while (!stackOperator.empty())
    {
        ExpressionNode beforeExpNode = stackOperator.top();
        if (beforeExpNode.GetType() == ExpressionNode::LParenthesis)
        {
            m_Status = false;
        }
        m_PostfixExpression.push_back(beforeExpNode);
        stackOperator.pop();
    }

#ifdef CC_PARSER_TEST
    wxString infix, postfix;
    for (InfixVector::size_type i = 0; i < m_InfixExpression.size(); ++i)
        infix += m_InfixExpression[i] + _T(" ");
    for (PostfixVector::size_type i = 0; i < m_PostfixExpression.size(); ++i)
        postfix += m_PostfixExpression[i].GetToken() + _T(" ");
    TRACE(_T("ConvertInfixToPostfix() : InfixExpression : %s"),   infix.wx_str());
    TRACE(_T("ConvertInfixToPostfix() : PostfixExpression : %s"), postfix.wx_str());
#endif
}

bool Expression::CalcPostfix()
{
    std::pair<long, long> pair;
    std::stack<long> stack;
    int cntNumeric = 0;

    for (PostfixVector::size_type i = 0; i < m_PostfixExpression.size(); ++i)
    {
        const ExpressionNode& node = m_PostfixExpression[i];
        const ExpressionNode::ExpressionNodeType type = node.GetType();
        if (type == ExpressionNode::Numeric)
        {
            ++cntNumeric;
            if (cntNumeric == 1)
                pair.first = node.GetTokenValue();
            else if (cntNumeric == 2)
                pair.second = node.GetTokenValue();
            else if (cntNumeric == 3)
            {
                --cntNumeric;
                stack.push(pair.first);
                TRACE(_T("CalcPostfix() : stack.push(pair.first) : %ld"), pair.first);
                pair.first = pair.second;
                pair.second = node.GetTokenValue();
            }
        }
        else
        {
            if (node.IsUnaryOperator())
            {
                if (cntNumeric == 1)
                    pair.first = CalculateUnary(type, pair.first);
                else if (cntNumeric == 2)
                    pair.second = CalculateUnary(type, pair.second);
            }
            else
            {
                if (cntNumeric == 2)
                {
                    --cntNumeric;
                    pair.first = Calculate(type, pair.first, pair.second);
                }
                else if (cntNumeric == 1)
                {
                    if (stack.empty())
                    {
                        m_Status = false;
                        return false;
                    }
                    pair.second = pair.first;
                    pair.first = stack.top();
                    TRACE(_T("CalcPostfix() : stack.pop() : %ld"), pair.first);
                    stack.pop();
                    pair.first = Calculate(type, pair.first, pair.second);
                }
            }
        }

        TRACE(_T("CalcPostfix() : pair.first : %ld, pair.second : %ld"), pair.first, pair.second);

        if (!m_Status)
            return false;
    }

    if (!stack.empty())
        m_Status = false;
    if (m_Status)
        m_Result = pair.first ? true : false;

    return true;
}

long Expression::Calculate(ExpressionNode::ExpressionNodeType type, long first, long second)
{
    switch (type)
    {
        case ExpressionNode::Plus:
            return first + second;
        case ExpressionNode::Subtract:
            return first - second;
        case ExpressionNode::Multiply:
            return first * second;
        case ExpressionNode::Divide:
            if (second == 0) { m_Status = false; return 0; }
            else return first / second;
        case ExpressionNode::Mod:
            if (second == 0) { m_Status = false; return 0; }
            else return first / second;
        case ExpressionNode::BitwiseAnd:
            return first & second;
        case ExpressionNode::BitwiseOr:
            return first | second;
        case ExpressionNode::And:
            return first && second;
        case ExpressionNode::Or:
            return first || second;
        case ExpressionNode::Equal:
            return first == second;
        case ExpressionNode::Unequal:
            return first != second;
        case ExpressionNode::GT:
            return first > second;
        case ExpressionNode::LT:
            return first < second;
        case ExpressionNode::GTOrEqual:
            return first >= second;
        case ExpressionNode::LTOrEqual:
            return first <= second;
        case ExpressionNode::LShift:
            return first << second;
        case ExpressionNode::RShift:
            return first >> second;
        case ExpressionNode::Unknown:
        case ExpressionNode::LParenthesis:
        case ExpressionNode::RParenthesis:
        case ExpressionNode::Power:
        case ExpressionNode::Not:
        case ExpressionNode::Numeric:
        default:
            return 0;
    }
}

long Expression::CalculateUnary(ExpressionNode::ExpressionNodeType type, long value)
{
    switch (type)
    {
        case ExpressionNode::Plus:
            return value;
        case ExpressionNode::Subtract:
            return 0 - value;
        case ExpressionNode::Not:
            return !value;
        case ExpressionNode::Unknown:
        case ExpressionNode::Multiply:
        case ExpressionNode::Divide:
        case ExpressionNode::LParenthesis:
        case ExpressionNode::RParenthesis:
        case ExpressionNode::Mod:
        case ExpressionNode::Power:
        case ExpressionNode::BitwiseAnd:
        case ExpressionNode::BitwiseOr:
        case ExpressionNode::And:
        case ExpressionNode::Or:
        case ExpressionNode::Equal:
        case ExpressionNode::Unequal:
        case ExpressionNode::GT:
        case ExpressionNode::LT:
        case ExpressionNode::GTOrEqual:
        case ExpressionNode::LTOrEqual:
        case ExpressionNode::LShift:
        case ExpressionNode::RShift:
        case ExpressionNode::Numeric:
        default:
            return 0;
    }
}
