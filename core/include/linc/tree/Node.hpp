#pragma once
#include <linc/lexer/Token.hpp>
#include <linc/tree/NodeInfo.hpp>
#include <linc/Include.hpp>
#define LINC_NODE_ASSERT(node, token_type) if((node).type != (token_type)) throw LINC_EXCEPTION_ILLEGAL_VALUE(node);
#define LINC_NODE_ASSERT_BINARY(node) if(!(node).isBinaryOperator()) throw LINC_EXCEPTION_ILLEGAL_VALUE(node);
#define LINC_NODE_ASSERT_UNARY(node) if(!(node).isUnaryOperator()) throw LINC_EXCEPTION_ILLEGAL_VALUE(node);
#define LINC_NODE_ASSERT_LITERAL(node) if(!(node).isLiteral() || !(node).value.has_value()) throw LINC_EXCEPTION_ILLEGAL_VALUE(node);
#define LINC_NODE_ASSERT_IDENTIFIER(node) if((node).type != Token::Type::Identifier || !(node).value.has_value()) throw LINC_EXCEPTION_ILLEGAL_VALUE(node);

namespace linc
{
    /// @brief Base AST node.
    class Node
    {
    public:
        Node(const NodeInfo& info)
            :m_info(info)
        {}

        Node(const Token::Info& token_info)
            :m_info(NodeInfo{.tokenList = std::vector<Token>{}, .info = token_info})
        {}

        virtual ~Node() = default;
        [[nodiscard]] inline const Token::Info& getTokenInfo() const { return m_info.info; }
        [[nodiscard]] inline const std::vector<Token>& getTokens() const { return m_info.tokenList; }
        [[nodiscard]] inline const NodeInfo& getInfo() const { return m_info; }

        inline void setLineNumber(size_t line_number) const { m_info.info.line = line_number; }
        inline void setTokens(std::vector<Token> tokens) const { m_info.tokenList = std::move(tokens); }
    
        inline void addToken(const Token& token) const { m_info.tokenList.push_back(token); }
        inline void addTokens(const std::vector<Token>& tokens) const { m_info.tokenList.insert(m_info.tokenList.end(), tokens.begin(), tokens.end()); }
        
        std::unique_ptr<const Node> clone() const;
    private:
        mutable NodeInfo m_info;
    };
}