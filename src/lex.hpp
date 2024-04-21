// Header Only
#pragma once
#include "lex_error.hpp"
#include <algorithm>
#include <exception>
#include <format>
#include <fstream>
#include <iostream>
#include <iterator>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <vector>
namespace lex {
    using std::cerr;
    using std::ifstream;
    using std::optional;
    using std::set;
    using std::string;
    using std::string_view;
    using std::vector;
    using std::literals::string_literals::operator""s;
    using lex_error::LexerException;

    const auto token_display_name = vector<string>{
        // struct keywords
        "Var"s,
        "If"s,
        "Then"s,
        "Else"s,
        "While"s,
        "Do"s,
        "Begin"s,
        "End"s,

        // operator keywords
        "And"s,
        "Or"s,

        // type keywords
        "Integer"s,
        "Longint"s,
        "Bool"s,
        "Real"s,

        // operators +|-|*|/|:=|<|>|<>|>=|<=|=|:|(|)

        "Add"s,
        "Sub"s,
        "Mul"s,
        "Div"s,
        "Assign"s,
        "Lt"s,
        "Gt"s,
        "Ne"s,
        "Ge"s,
        "Le"s,
        "Eq"s,
        "Colon"s,
        "LParen"s,
        "RParen"s,
        // struct symbols
        "Comma"s,
        "SemiColon"s,
        // Literals and Identifiers
        "Ident"s,
        "IntLiteral"s,
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
        Ident,
        /// Integer literal [1-9][0-9]*|0, no leading 0
        IntLiteral,
    };
    struct Token {
        TokenType type;
        int start;
        string content;
        std::string to_string() const {
            return std::format("{} {} {}", start, token_display_name[type], content);
        }
    };
#define match_keyword(s, ty) \
    if (lett == s)           \
        return Token { ty, start, lett }
    struct Lexer {
        string src;
        int pos;
        optional<Token> next_token() {
            while (pos < src.size() && isspace(src[pos])) {
                pos++;
            }
            if (pos == src.size()) {
                return {};
            }

            int start = pos;
            if (isalpha(src[pos])) {

                string lett = "";
                while (pos < src.size() && isalnum(src[pos])) {
                    lett += src[pos];
                    pos++;
                }
                transform(lett.begin(), lett.end(), lett.begin(), ::tolower);
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
                return Token{Ident, start, lett};
            } else if (isdigit(src[pos])) {
                string lett = "";
                while (pos < src.size() && isdigit(src[pos])) {
                    lett += src[pos];
                    pos++;
                }
                if (lett[0] == '0' && lett.size() > 1) {
                    throw LexerException(pos, std::format("invalid integer literal {}", lett));
                }
                if (pos < src.size() && isalpha(src[pos])) {
                    throw LexerException(pos, "unexpected character");
                }
                return Token{IntLiteral, start, lett};
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

    vector<Token> lex(string src) {
        auto lexer = Lexer{src, 0};
        vector<Token> tokens;
        while (1) {
            auto token = lexer.next_token();
            if (!token.has_value()) {
                return tokens;
            }
            tokens.push_back(token.value());
        }
    }
}
