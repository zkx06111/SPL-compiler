#pragma once

#include "llvm/IR/Value.h"

#include "gen.h"
#include "ConstTable.h"

namespace gen {

class ValueContext {
  public:
    void NewVariable(const std::string &name, const sem::Type &type);
    void NewConstant(const std::string &name, int val);
    void NewConstant(const std::string &name, double val);
    void NewConstant(const std::string &name, bool val);
    void NewConstant(const std::string &name, char val);

    bool HasName(const std::string &name) const;
    llvm::Value *GetValue(const std::string &name) const;

    static llvm::Constant *Const(int val);
    static llvm::Constant *Const(double val);
    static llvm::Constant *Const(char val);
    static llvm::Constant *Const(bool val);

  private:
    std::unordered_map<std::string, llvm::Value *> values;
};

}