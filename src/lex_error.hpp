// Header Only
#pragma once
#include "common.hpp"

namespace lex_error {
    struct LexerException : public std::exception {
        int offset;
        std::string msg;

        LexerException(int offset, const std::string &msg) : offset(offset), msg(msg) {}

        const char *what() const noexcept override {
            return msg.c_str();
        }
    };
    class LinePos {
    public:
        std::string content;
        std::vector<int> start_offset;

        LinePos(const std::string &str) {
            content = "";
            for (auto c : str) {
                if (c == '\r') {
                    continue;
                }
                content.push_back(c);
            }
            if (!content.ends_with('\n')) {
                content.push_back('\n');
            }
            start_offset.push_back(0);
            for (int i = 0; i < content.size(); i++) {
                if (content[i] == '\n') {
                    start_offset.push_back(i + 1);
                }
            }
        }

        /// offset: 0-indexed
        /// (line, col): 1-indexed
        std::pair<int, int> line_col(int offset) const {
            offset = std::min(offset, (int)content.size());
            auto line = std::upper_bound(start_offset.begin(), start_offset.end(), offset) - start_offset.begin();
            return {line, offset - start_offset[line - 1] + 1};
        }
        /// Return the line content, 1-indexed
        std::string get_line(int line) const {
            if (line < 1 || line >= start_offset.size()) {
                return "";
            }
            // remove '\n'
            return content.substr(start_offset[line - 1], start_offset[line] - start_offset[line - 1] - 1);
        }
        void print_err(const std::string &filename, int offset, const std::string &msg) const {
            auto [line, col] = line_col(offset);
            std::cerr << std::format("[{}:{}:{}] Error: {}", filename, line, col, msg) << std::endl;
            std::cerr << get_line(line) << std::endl;
            std::cerr << std::string(col - 1, ' ') << '^' << std::endl;
        }
    };
}
