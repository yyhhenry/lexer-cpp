// Header Only
#pragma once
#include "common.hpp"
#include "lex.hpp"
#include "lex_error.hpp"

namespace pre_grammar {
    using lex::Token;
    using lex::TokenType;
    using lex_error::LexerException;

    struct PreGrammar {
        std::set<std::string> used;
        std::vector<Token> tokens;
        int pos;
        PreGrammar(const std::vector<Token> &tokens) : tokens(tokens), pos(), used() {}
        void parse_var_block() {
            if (pos < tokens.size() && tokens[pos].type == TokenType::Var) {
                pos++;
            } else {
                auto start = pos < tokens.size() ? tokens[pos].start : 0;
                throw LexerException(start, "missing Var");
            }
            while (pos < tokens.size()) {

                while (pos < tokens.size()) {

                    if (tokens[pos].type != TokenType::Identifier) {
                        throw LexerException(tokens[pos].start, "missing Identifier");
                    } else {
                        if (used.count(tokens[pos].content)) {
                            throw LexerException(tokens[pos].start, "redefined Identifier");
                        }
                        used.insert(tokens[pos].content);
                        pos++;
                    }
                    if (pos < tokens.size() && tokens[pos].type == TokenType::Comma) {
                        pos++;
                        continue;
                    } else if (pos < tokens.size() && tokens[pos].type == TokenType::Colon) {
                        pos++;
                        break;
                    } else {
                        throw LexerException(tokens[pos].start, "missing , or :");
                    }
                }
                if (pos < tokens.size() && (tokens[pos].type == TokenType::Integer || tokens[pos].type == TokenType::Bool || tokens[pos].type == TokenType::Real || tokens[pos].type == TokenType::Longint)) {
                    pos++;
                    if (pos < tokens.size() && tokens[pos].type == TokenType::SemiColon) {
                        pos++;
                    } else {
                        throw LexerException(tokens[pos].start, "missing ;");
                    }
                } else {
                    throw LexerException(tokens[pos].start, "missing type");
                }

                if (pos < tokens.size() && tokens[pos].type == TokenType::Begin) {
                    pos++;
                    break;
                } else if (pos >= tokens.size()) {
                    throw LexerException(tokens[pos - 1].start, "missing Begin");
                }
            }
        }
        void parse_program_block() {
            int matchBE = 1;

            while (pos < tokens.size()) {
                if (tokens[pos].type == TokenType::Identifier) {
                    if (!used.count(tokens[pos].content)) {
                        throw LexerException(tokens[pos].start, "undefined Identifier");
                    }
                }
                if (tokens[pos].type == TokenType::End) {
                    matchBE--;
                }
                pos++;
            }
        }
        void parse_code() {

            parse_var_block();

            parse_program_block();
        }
    };
}
