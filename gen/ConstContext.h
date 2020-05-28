#pragma once

#include "llvm/IR/Value.h"

#include "ExValue.h"

namespace gen {

class ConstContext {
  public:
    void NewConstant(const std::string &name, int val);
    void NewConstant(const std::string &name, double val);
    void NewConstant(const std::string &name, bool val);
    void NewConstant(const std::string &name, char val);

    bool HasName(const std::string &name) const;
    ExValue GetConst(const std::string &name) const;

    static llvm::Value *Const(int val);
    static llvm::Value *Const(double val);
    static llvm::Value *Const(char val);
    static llvm::Value *Const(bool val);
    static ExValue ConstEVal(int val);
    static ExValue ConstEVal(double val);
    static ExValue ConstEVal(char val);
    static ExValue ConstEVal(bool val);

  private:
    std::unordered_map<std::string, ExValue> consts;
};

}