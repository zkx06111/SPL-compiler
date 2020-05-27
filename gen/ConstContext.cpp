#include "ConstContext.h"

#include "TypeContext.h"

namespace gen {

void ConstContext::NewConstant(const std::string &name, int val) {
    values[name] = Const(val);
}
void ConstContext::NewConstant(const std::string &name, double val) {
    values[name] = Const(val);
}
void ConstContext::NewConstant(const std::string &name, bool val) {
    values[name] = Const(val);
}
void ConstContext::NewConstant(const std::string &name, char val) {
    values[name] = Const(val);
}

llvm::Constant *ConstContext::Const(int val) {
    return llvm::ConstantInt::get(TypeContext::Int(), val, true);
}
llvm::Constant *ConstContext::Const(double val) {
    return llvm::ConstantFP::get(TypeContext::Real(), val);
}
llvm::Constant *ConstContext::Const(char val) {
    return llvm::ConstantInt::get(TypeContext::Char(), val, true);
}
llvm::Constant *ConstContext::Const(bool val) {
    return llvm::ConstantInt::get(TypeContext::Bool(), val, true);
}

}