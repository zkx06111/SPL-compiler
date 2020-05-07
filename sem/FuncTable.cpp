#include "FuncTable.h"

namespace sem {

void FuncTable::AddFunc(const std::string &name, const Func &func) {
    funcs[name] = func;
}

bool FuncTable::CheckFunc(const std::string &name) const {
    return funcs.count(name) != 0;
}

Func FuncTable::GetFunc(const std::string &name) const {
    return funcs.at(name);
}

}