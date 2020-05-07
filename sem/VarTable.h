#pragma once

#include <optional>
#include <unordered_map>

#include "Type.h"

namespace sem {

class VarTable {
  public:
    void AddVar(const std::string &name, const Type &type);
    bool CheckVar(const std::string &name) const;
    Type GetVarType(const std::string &name) const;

  private:
    std::unordered_map<std::string, Type> vars;
};

}