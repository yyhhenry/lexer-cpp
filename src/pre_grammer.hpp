// Header Only
#pragma once
#include "lex.hpp"
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
namespace pre_grammar {
    using std::cerr;
    using std::ifstream;
    using std::optional;
    using std::set;
    using std::string;
    using std::string_view;
    using std::vector;
    using std::literals::string_literals::operator""s;
    using lex::Token;
    using lex::TokenType;
    using lex_error::LexerException;

    struct PreGrammar {
        set<string> used;
        vector<Token> tokens;
        int pos;
        PreGrammar(vector<Token> tokens) : tokens(tokens), pos(), used() {}
        void parse_var_block() {
            if (pos < tokens.size() && tokens[pos].type == TokenType::Var) {
                pos++;
            } else {
                auto start = pos < tokens.size() ? tokens[pos].start : 0;
                throw LexerException(start, "missing Var");
            }
            while (pos < tokens.size()) {

                while (pos < tokens.size()) {

                    if (tokens[pos].type != TokenType::Ident) {
                        throw LexerException(tokens[pos].start, "missing Ident");
                    } else {
                        if (used.count(tokens[pos].content)) {
                            throw LexerException(tokens[pos].start, "redefined Ident");
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
                if (tokens[pos].type == TokenType::Ident) {
                    if (!used.count(tokens[pos].content)) {
                        throw LexerException(tokens[pos].start, "undefined Ident");
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
