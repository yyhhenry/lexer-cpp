#include "lex.hpp"
#include "lex_error.hpp"
#include "pre_grammer.hpp"
#include <fstream>

std::string read_file(const std::string &filename) {
    using i_itr = std::istreambuf_iterator<char>;

    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error(std::format("Error: could not open file {}", filename));
    }
    auto content = std::string(i_itr(file), i_itr());
    file.close();
    return content;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        throw std::runtime_error(std::format("Usage: {} <filename>", argv[0]));
    }
    auto filename = std::string(argv[1]);
    auto content = read_file(filename);
    auto line_pos = lex_error::LinePos(content);

    try {
        auto tokens = lex::lex(content);
        auto pre_grammar = pre_grammar::PreGrammar(tokens);
        pre_grammar.parse_code();
        for (auto &token : tokens) {
            std::cout << token.to_string() << std::endl;
        }
        return 0;
    } catch (lex::LexerException &e) {
        line_pos.print_err(filename, e.offset, e.msg);
        return 1;
    }
}
