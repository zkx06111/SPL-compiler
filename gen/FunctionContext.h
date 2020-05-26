#pragma once

#include <string>
#include <unordered_map>

#include "llvm/IR/Type.h"
#include "llvm/IR/Function.h"

#include "Func.h"

namespace gen {

class FunctionContext {
  public:
    void NewFunction(const std::string &name, const sem::Func &func);

    bool HasName(const std::string &name) const;
    llvm::Function *GetFunction(const std::string &name) const;

    static void Return(llvm::Value *val);

    static llvm::Function *CFunction(const std::string &name, llvm::Type *ret,
        const std::vector<llvm::Type *> &args);

  private:
    std::unordered_map<std::string, llvm::Function *> funcs;
};

}