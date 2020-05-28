#include "ConstContext.h"

#include "llvm/IR/Constants.h"

#include "TypeContext.h"

namespace gen {

void ConstContext::NewConstant(const std::string &name, int val) {
    ExValue eval;
    eval.is_const = true;
    eval.type = sem::Type::Int();
    eval.value = Const(val);
    eval.val_i = val;
    consts[name] = eval;
}
void ConstContext::NewConstant(const std::string &name, double val) {
    ExValue eval;
    eval.is_const = true;
    eval.type = sem::Type::Real();
    eval.value = Const(val);
    eval.val_r = val;
    consts[name] = eval;
}
void ConstContext::NewConstant(const std::string &name, bool val) {
    ExValue eval;
    eval.is_const = true;
    eval.type = sem::Type::Bool();
    eval.value = Const(val);
    eval.val_b = val;
    consts[name] = eval;
}
void ConstContext::NewConstant(const std::string &name, char val) {
    ExValue eval;
    eval.is_const = true;
    eval.type = sem::Type::Char();
    eval.value = Const(val);
    eval.val_c = val;
    consts[name] = eval;
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