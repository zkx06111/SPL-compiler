#pragma once

#include "llvm/IR/Value.h"

#include "Type.h"

namespace gen {

class ValueContext {
  public:
    void NewVariable(const std::string &name, const sem::Type &type);

    bool HasName(const std::string &name) const;
    llvm::Value *GetVariable(const std::string &name) const;

  private:
    std::unordered_map<std::string, llvm::Value *> values;
};

}