#pragma once

#include <unordered_map>

#include "Func.h"

namespace sem {

class FuncTable {
  public:
    void AddFunc(const std::string &name, const Func &func);
    bool CheckFunc(const std::string &name) const;
    Func GetFunc(const std::string &name) const;

  private:
    std::unordered_map<std::string, Func> funcs;
};

}