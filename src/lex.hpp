// Header Only
#pragma once
#include "common.hpp"
#include "lex_error.hpp"

namespace lex {
    using lex_error::LexerException;

    const auto token_display_name = std::vector<std::string>{
        // struct keywords
        "Var",
        "If",
        "Then",
        "Else",
        "While",
        "Do",
        "Begin",
        "End",

        // operator keywords
        "And",
        "Or",

        // type keywords
        "Integer",
        "Longint",
        "Bool",
        "Real",

        // operators +|-|*|/|:=|<|>|<>|>=|<=|=|:|(|)

        "Add",
        "Sub",
        "Mul",
        "Div",
        "Assign",
        "Lt",
        "Gt",
        "Ne",
        "Ge",
        "Le",
        "Eq",
        "Colon",
        "LParen",
        "RParen",
        // struct symbols
        "Comma",
        "SemiColon",
        // Literals and Identifiers
        "Identifier",
        "IntLiteral",
    };
    enum TokenType {
        // struct keywords
        Var,
        If,
        Then,
        Else,
        While,
        Do,
        Begin,
        End,

        // operator keywords
        And,
        Or,

        // type keywords
        Integer,
        Longint,
        Bool,
        Real,

        // operators +|-|*|/|:=|<|>|<>|>=|<=|=|:|(|)
        /// +
        Add,
        /// -
        Sub,
        /// *
        Mul,
        /// /
        Div,
        /// :=
        Assign,
        /// <
        Lt,
        /// >
        Gt,
        /// <>
        Ne,
        /// >=
        Ge,
        /// <=
        Le,
        /// ==
        Eq,
        /// :
        Colon,
        /// (
        LParen,
        /// )
        RParen,

        // struct symbols
        /// ,
        Comma,
        /// ;
        SemiColon,

        // Literals and Identifiers
        /// Identifier [a-zA-Z][a-zA-Z0-9]*, case insensitive
        Identifier,
        /// Integer literal [1-9][0-9]*|0, no leading 0
        IntLiteral,
    };
    struct Token {
        TokenType type;
        int start;
        std::string content;
        std::string to_string() const {
            return std::format("{} {} {}", start, token_display_name[type], content);
        }
    };
#define match_keyword(s, ty) \
    if (word == s)           \
        return Token { ty, start, word }
    struct Lexer {
        std::string src;
        int pos;
        std::optional<Token> next_token() {
            while (pos < src.size() && isspace(src[pos])) {
                pos++;
            }
            if (pos == src.size()) {
                return {};
            }

            int start = pos;
            if (isalpha(src[pos])) {

                std::string word = "";
                while (pos < src.size() && isalnum(src[pos])) {
                    word += src[pos];
                    pos++;
                }
                transform(word.begin(), word.end(), word.begin(), ::tolower);
                match_keyword("var", Var);
                match_keyword("if", If);
                match_keyword("then", Then);
                match_keyword("else", Else);
                match_keyword("while", While);
                match_keyword("do", Do);
                match_keyword("begin", Begin);
                match_keyword("end", End);
                match_keyword("and", And);
                match_keyword("or", Or);
                match_keyword("integer", Integer);
                match_keyword("longint", Longint);
                match_keyword("bool", Bool);
                match_keyword("real", Real);
                return Token{Identifier, start, word};
            } else if (isdigit(src[pos])) {
                std::string word = "";
                while (pos < src.size() && isdigit(src[pos])) {
                    word += src[pos];
                    pos++;
                }
                if (word[0] == '0' && word.size() > 1) {
                    throw LexerException(pos, std::format("invalid integer literal {}", word));
                }
                if (pos < src.size() && isalpha(src[pos])) {
                    throw LexerException(pos, "unexpected character");
                }
                return Token{IntLiteral, start, word};
            } else if (src[pos] == ':' && pos < src.size() + 1 && src[pos + 1] == '=') {
                pos += 2;
                return Token{Assign, start, ":="};
            } else if (src[pos] == '<' && pos < src.size() + 1 && src[pos + 1] == '=') {
                pos += 2;
                return Token{Le, start, "<="};
            } else if (src[pos] == '>' && pos < src.size() + 1 && src[pos + 1] == '=') {
                pos += 2;
                return Token{Ge, start, ">="};
            } else if (src[pos] == '<' && pos < src.size() + 1 && src[pos + 1] == '>') {
                pos += 2;
                return Token{Ne, start, "<>"};
            } else if (src[pos] == ':') {
                pos++;
                return Token{Colon, start, ":"};
            } else if (src[pos] == ',') {
                pos++;
                return Token{Comma, start, ","};
            } else if (src[pos] == ';') {
                pos++;
                return Token{SemiColon, start, ";"};
            } else if (src[pos] == '+') {
                pos++;
                return Token{Add, start, "+"};
            } else if (src[pos] == '-') {
                pos++;
                return Token{Sub, start, "-"};
            } else if (src[pos] == '*') {
                pos++;
                return Token{Mul, start, "*"};
            } else if (src[pos] == '/') {
                pos++;
                return Token{Div, start, "/"};
            } else if (src[pos] == '<') {
                pos++;
                return Token{Lt, start, "<"};
            } else if (src[pos] == '>') {
                pos++;
                return Token{Gt, start, ">"};
            } else if (src[pos] == '=' && pos < src.size() + 1 && src[pos + 1] == '=') {
                pos += 2;
                return Token{Eq, start, "=="};
            } else if (src[pos] == '=') {
                throw LexerException(pos, "Unexpected =, consider := or ==");
            } else if (src[pos] == '(') {
                pos++;
                return Token{LParen, start, "("};
            } else if (src[pos] == ')') {
                pos++;
                return Token{RParen, start, ")"};
            } else {
                throw LexerException(pos, "unexpected character");
            }
        }
    };
#undef match_keyword

    inline std::vector<Token> lex(const std::string &src) {
        auto lexer = Lexer{src, 0};
        std::vector<Token> tokens;
        while (1) {
            auto token = lexer.next_token();
            if (!token.has_value()) {
                return tokens;
            }
            tokens.push_back(token.value());
        }
    }
}
