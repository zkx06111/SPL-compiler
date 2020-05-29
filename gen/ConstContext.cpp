#include "ConstContext.h"

#include "llvm/IR/Constants.h"

#include "TypeContext.h"

namespace gen {

void ConstContext::NewConstant(const std::string &name, int val) {
    consts[name] = ConstEVal(val);
}
void ConstContext::NewConstant(const std::string &name, double val) {
    consts[name] = ConstEVal(val);
}
void ConstContext::NewConstant(const std::string &name, bool val) {
    consts[name] = ConstEVal(val);
}
void ConstContext::NewConstant(const std::string &name, char val) {
    consts[name] = ConstEVal(val);
}

bool ConstContext::HasName(const std::string &name) const {
    return consts.count(name) != 0;
}

ExValue ConstContext::GetConst(const std::string &name) const {
    return consts.at(name);
}

llvm::Value *ConstContext::Const(int val) {
    return llvm::ConstantInt::get(TypeContext::Int(), val, true);
}
llvm::Value *ConstContext::Const(double val) {
    return llvm::ConstantFP::get(TypeContext::Real(), val);
}
llvm::Value *ConstContext::Const(char val) {
    return llvm::ConstantInt::get(TypeContext::Char(), val, true);
}
llvm::Value *ConstContext::Const(bool val) {
    return llvm::ConstantInt::get(TypeContext::Bool(), val, true);
}

ExValue ConstContext::ConstEVal(int val) {
    ExValue eval;
    eval.type = sem::Type::Int();
    eval.is_const = true;
    eval.val_i = val;
    return eval;
}
ExValue ConstContext::ConstEVal(double val) {
    ExValue eval;
    eval.type = sem::Type::Real();
    eval.is_const = true;
    eval.val_r = val;
    return eval;
}
ExValue ConstContext::ConstEVal(char val) {
    ExValue eval;
    eval.type = sem::Type::Char();
    eval.is_const = true;
    eval.val_c = val;
    return eval;
}
ExValue ConstContext::ConstEVal(bool val) {
    ExValue eval;
    eval.type = sem::Type::Bool();
    eval.is_const = true;
    eval.val_b = val;
    return eval;
}

}