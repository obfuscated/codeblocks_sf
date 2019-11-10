/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <wx/string.h>
#include <vector>

class ExpressionNode
{
public:
    enum ExpressionNodeType
    {
        Unknown,        // Unknown
        Plus,           // +
        Subtract,       // -
        Multiply,       // *
        Divide,         // /
        LParenthesis,   // (
        RParenthesis,   // )
        Mod,            // %
        Power,          // ^
        BitwiseAnd,     // &
        BitwiseOr,      // |
        And,            // &&
        Or,             // ||
        Not,            // !
        Equal,          // ==
        Unequal,        // !=
        GT,             // >
        LT,             // <
        GTOrEqual,      // >=
        LTOrEqual,      // <=
        LShift,         // <<
        RShift,         // >>
        Numeric         // Numeric
    };

    ExpressionNode();
    void Initialize(wxString token);

    ExpressionNodeType GetType() const { return m_Type; }

    bool IsUnaryOperator() const { return m_UnaryOperator; }
    void SetUnaryOperator(bool unary = true) { m_UnaryOperator = unary; }

    long GetPriority() const { return m_Priority; }
    wxString GetToken() const { return m_Token; }
    long GetTokenValue() const
    {
        long r;
        m_Token.ToLong(&r);
        return r;
    }

    static ExpressionNodeType ParseNodeType(wxString token);
    static long GetNodeTypePriority(ExpressionNodeType type);
    static bool IsUnaryNode(ExpressionNodeType type);
    // check to see the two char token can be merged to create a single operator
    // e.g.  if the first is '&' and the second is '&', then the function will return true since
    // "&&" is a valid operator.
    static bool IsBinaryOperator(wxString first, wxString second);

private:
    wxString           m_Token;
    ExpressionNodeType m_Type;
    bool               m_UnaryOperator;
    long               m_Priority;
};

class Expression
{
public:
    // constructor
    Expression() :
        m_Status(false),
        m_Result(0)
    {
    };

    void AddToInfixExpression(wxString token);
    void ConvertInfixToPostfix();
    bool CalcPostfix();
    long GetResult() const { return m_Result; }
    bool GetStatus() const { return m_Status; }
    void Clear();

private:
    long Calculate(ExpressionNode::ExpressionNodeType type, long first, long second);
    long CalculateUnary(ExpressionNode::ExpressionNodeType type, long value);

private:
    typedef std::vector<ExpressionNode> PostfixVector;
    typedef std::vector<wxString> InfixVector;

    PostfixVector m_PostfixExpression;
    InfixVector   m_InfixExpression;
    bool          m_Status;
    long          m_Result; /// the calculated value
};

#endif // EXPRESSION_H
