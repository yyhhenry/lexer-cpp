#include "common.hpp"
#include "lex.hpp"
#include "lex_error.hpp"
#include "pre_grammer.hpp"

std::string read_file(const std::string &filename) {
    using in_iter = std::istreambuf_iterator<char>;

    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error(std::format("Error: could not open file {}", filename));
    }
    auto content = std::string(in_iter(file), in_iter());
    file.close();
    return content;
}

/// $ lex [Options] <input_file>
/// Options:
///   -o <output_file> Output file. Optional.
///   -h,--help        Display this information.
struct Args {
    std::string input_file;
    std::optional<std::string> output_file;
    static std::vector<std::string> to_args(int argc, char *argv[]) {
        std::vector<std::string> args;
        for (int i = 0; i < argc; i++) {
            args.push_back(argv[i]);
        }
        return args;
    }
    Args(int argc, char *argv[]) : Args(to_args(argc, argv)) {}
    Args(const std::vector<std::string> &args) {
        if (args.size() < 2) {
            throw std::runtime_error("Error: missing input file");
        }
        auto free_args = std::vector<std::string>();
        for (int i = 1; i < args.size(); i++) {
            if (args[i] == "-o") {
                if (i + 1 >= args.size()) {
                    throw std::runtime_error("Error: missing output file");
                }
                output_file = args[i + 1];
                i++;
            } else if (args[i] == "-h" || args[i] == "--help") {
                std::cout << "Usage: lex [Options] <input_file>" << std::endl;
                std::cout << "Options:" << std::endl;
                std::cout << "  -o <output_file> Output file. Optional." << std::endl;
                std::cout << "  -h,--help        Display this information." << std::endl;
                exit(0);
            } else {
                free_args.push_back(args[i]);
            }
        }
        if (free_args.empty()) {
            throw std::runtime_error("Error: missing input file");
        } else if (free_args.size() > 1) {
            throw std::runtime_error("Error: too many free arguments");
        }
        input_file = free_args[0];
    }
};

int main(int argc, char *argv[]) {
    try {
        auto args = Args(argc, argv);
        auto content = read_file(args.input_file);
        auto line_pos = lex_error::LinePos(content);

        try {
            auto tokens = lex::lex(content);
            auto pre_grammar = pre_grammar::PreGrammar(tokens);
            pre_grammar.parse_code();
            auto output_file = args.output_file.value_or("a.out");
            for (auto &token : tokens) {
                std::cout << token.to_string() << std::endl;
            }
            return 0;
        } catch (lex::LexerException &e) {
            line_pos.print_err(args.input_file, e.offset, e.msg);
            return 1;
        }
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
