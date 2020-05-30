#pragma once

#include <string>
#include <unordered_map>

#include "llvm/IR/Type.h"
#include "llvm/IR/Function.h"

#include "ExValue.h"
#include "Func.h"

namespace gen {

struct FuncSign {
    llvm::Function *fn;
    llvm::BasicBlock *block;
    std::string fn_name;
    sem::Type ret;
    std::vector<std::pair<std::string, sem::Type>> args;
    std::vector<int> mut_args;
    std::vector<std::pair<std::string, ExValue>> pregs;

    ExValue Call(std::vector<ExValue> params);
    void Return();
    void Leave();
    void Restart();
};

class FunctionContext {
  public:
    FuncSign GetSign(const std::string &name, const sem::Type &ret,
        const std::vector<std::pair<std::string, sem::Type>> &args,
        const std::vector<int> mut_args);
    void NewFunc(FuncSign &sign);

    bool HasName(const std::string &name) const;
    FuncSign GetFunction(const std::string &name) const;

    static llvm::Function *CFunction(const std::string &name, llvm::Type *ret,
        const std::vector<llvm::Type *> &args, bool var_len = false);
    static void MainFunction();

  private:
    std::unordered_map<std::string, FuncSign> funcs;
};

}