#pragma once

#include "llvm/IR/Value.h"

#include "ExValue.h"

namespace gen {

class ValueContext {
  public:
    void NewVariable(const std::string &name, const sem::Type &type,
        bool is_global = false);

    bool HasName(const std::string &name) const;
    ExValue GetVariable(const std::string &name) const;
    void ModifyVariable(const std::string &name, const ExValue &eval);

  private:
    std::unordered_map<std::string, ExValue> values;
};

}