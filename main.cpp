#include <iostream>

#include "tree.h"
#include "sem.h"
#include "gen.h"

enum class OutputType {
    EXE, OBJ, ASM, LL
} output_type = OutputType::EXE;
std::string input_name = "", output_name = "";
bool debug = false, keep = false;

std::string get_args(int argc, char **argv) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-o") == 0) {
            if (i + 1 < argc) {
                ++i;
                output_name = argv[i];
            } else {
                return "unknown output file name";
            }
        } else if (strcmp(argv[i], "-d") == 0) {
            debug = true;
        } else if (strcmp(argv[i], "-k") == 0) {
            keep = true;
        } else if (strcmp(argv[i], "-t") == 0) {
            if (i + 1 < argc) {
                ++i;
                if (strcmp(argv[i], "exe") == 0) {
                    output_type = OutputType::EXE;
                } else if (strcmp(argv[i], "obj") == 0) {
                    output_type = OutputType::OBJ;
                } else if (strcmp(argv[i], "asm") == 0) {
                    output_type = OutputType::ASM;
                } else if (strcmp(argv[i], "ll") == 0) {
                    output_type = OutputType::LL;
                } else {
                    return "unknown output type";
                }
            }
        } else if (strcmp(argv[i], "-h") == 0) {
            return "help";
        } else {
            if (input_name == "") {
                input_name = argv[i];
            } else {
                return "more than one input file is specified";
            }
        }
    }
    if (input_name == "") {
        return "no input file is specified";
    }
    return "";
}

static const std::string help_str = 
"SPL Compiler\n"
"Usage: SPL-compiler [options] file...\n"
"Options:\n"
"  -o <name>  Specify the name of output file.\n"
"  -t <type>  Specity the type of output file. It can be exe, obj, asm and ll.\n"
"             By default, executable file will be generated.\n"
"  -d         Print debug message.\n"
"  -k         Keep intermediate file.\n"
"  -h         Show usage\n";

int main(int argc, char **argv) {
    auto ret_str = get_args(argc, argv);
    if (ret_str != "") {
        bool is_help = (ret_str == "help");
        if (!is_help) {
            std::cout << ret_str << std::endl;
        }
        std::cout << help_str << std::endl;
        return is_help ? 0 : -1;
    }

    auto file_ret = freopen(input_name.c_str(), "r", stdin);
    if (file_ret == nullptr) {
        std::cout << "can't open file '" << input_name << "'" << std::endl;
        return -2;
    }

    TreeNode *root = buildTree();
    if (debug) {
        print(root);
    }
    if (root == nullptr) {
        return -3;
    }

    bool sem_ret = sem::CheckSem(root, argv[1]);
    if (!sem_ret) {
        std::cout << "sematic error occurred" << std::endl;
        return -4;
    }

    gen::GenCode(root, argv[1]);

    if (output_type == OutputType::EXE) {
        gen::GenExe(output_name, keep);
    } else if (output_type == OutputType::OBJ) {
        gen::GenObj(output_name, keep);
    } else if (output_type == OutputType::ASM) {
        gen::GenAsm(output_name, keep);
    } else {
        gen::GenLlvm(output_name, keep);
    }

    return 0;
}