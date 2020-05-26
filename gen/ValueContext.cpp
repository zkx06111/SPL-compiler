#include "ValueContext.h"

#include "gen.h"
#include "TypeContext.h"
#include "SymbolTable.h"

namespace gen {

void ValueContext::NewVariable(const std::string &name, const sem::Type &type) {
    llvm::Type *ty = TypeContext::Type(type);
    llvm::Value *value;
    if (sem::sym_t.IsGlobal()) {
        value = new llvm::GlobalVariable(llvm_module, ty, false,
            llvm::GlobalValue::InternalLinkage,
            llvm::ConstantAggregateZero::get(ty), name);
    } else {
        value = ir_builder.CreateAlloca(ty, nullptr, name);
    }
    values[name] = value;
}

void ValueContext::NewConstant(const std::string &name, int val) {
    values[name] = Const(val);
}
void ValueContext::NewConstant(const std::string &name, double val) {
    values[name] = Const(val);
}
void ValueContext::NewConstant(const std::string &name, bool val) {
    values[name] = Const(val);
}
void ValueContext::NewConstant(const std::string &name, char val) {
    values[name] = Const(val);
}

bool ValueContext::HasName(const std::string &name) const {
    return values.count(name) != 0;
}

llvm::Value *ValueContext::GetValue(const std::string &name) const {
    return values.at(name);
}

llvm::Constant *ValueContext::Const(int val) {
    return llvm::ConstantInt::get(TypeContext::Int(), val, true);
}
llvm::Constant *ValueContext::Const(double val) {
    return llvm::ConstantFP::get(TypeContext::Real(), val);
}
llvm::Constant *ValueContext::Const(char val) {
    return llvm::ConstantInt::get(TypeContext::Char(), val, true);
}
llvm::Constant *ValueContext::Const(bool val) {
    return llvm::ConstantInt::get(TypeContext::Bool(), val, true);
}

}