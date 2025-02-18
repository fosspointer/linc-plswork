#pragma once
#include <linc/tree/Statement.hpp>
#include <linc/Include.hpp>

namespace linc
{
    class ReturnStatement final : public Statement
    {
    public:
        ReturnStatement(const Token& terminator, const Token& return_keyword, std::unique_ptr<const Expression> expression)
            :Statement(terminator, return_keyword.info), m_returnKeyword(return_keyword),
            m_expression(std::move(expression))
        {
            addToken(m_returnKeyword);
            if(m_expression)
                addTokens(m_expression->getTokens());
        }

        inline const Token& getReturnKeyword() const { return m_returnKeyword; }
        inline const Expression* const getExpression() const { return m_expression.get(); }
    
        virtual std::unique_ptr<const Statement> clone() const final override
        {
            return std::make_unique<const ReturnStatement>(getTerminator(), m_returnKeyword, m_expression? m_expression->clone(): nullptr);
        } 
    private:
        const Token m_returnKeyword;
        const std::unique_ptr<const Expression> m_expression;
    };
}