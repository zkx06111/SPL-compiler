#include "VarTable.h"

namespace sem {

void VarTable::AddVar(const std::string &name, const Type &type) {
    vars[name] = type;
}

bool VarTable::CheckVar(const std::string &name) const {
    return vars.count(name) != 0;
}

Type VarTable::GetVarType(const std::string &name) const {
    return vars.at(name);
}

}