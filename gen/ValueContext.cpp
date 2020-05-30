#include "ValueContext.h"

#include "gen.h"
#include "TypeContext.h"
#include "GeneratorContext.h"

namespace gen {

void ValueContext::NewVariable(const std::string &name, const sem::Type &type,
        bool is_global, bool is_ref) {
    llvm::Type *ty;
    if (is_ref) {
        ty = TypeContext::PtrType(type);
    } else {
        ty = TypeContext::Type(type);
    }
    llvm::Value *value;
    if (is_global) {
        llvm::Constant *init;
        if (is_ref) {
            init = llvm::ConstantPointerNull::get(
                llvm::dyn_cast<llvm::PointerType>(ty));
        } else if (type.type == sem::Type::INT) {
            init = llvm::dyn_cast<llvm::Constant>(ConstContext::Const(0));
        } else if (type.type == sem::Type::REAL) {
            init = llvm::dyn_cast<llvm::Constant>(ConstContext::Const(0.0));
        } else if (type.type == sem::Type::CHAR) {
            init = llvm::dyn_cast<llvm::Constant>(ConstContext::Const('\0'));
        } else if (type.type == sem::Type::BOOL) {
            init = llvm::dyn_cast<llvm::Constant>(ConstContext::Const(false));
        } else {
            init = llvm::ConstantAggregateZero::get(ty);
        }
        value = new llvm::GlobalVariable(llvm_module, ty, false,
            llvm::GlobalValue::CommonLinkage, init, name);
    } else {
        value = ir_builder.CreateAlloca(ty, nullptr, name);
    }
    ExValue eval;
    eval.addr = value;
    eval.type = type;
    eval.is_ref = is_ref;
    values[name] = eval;
}

bool ValueContext::HasName(const std::string &name) const {
    return values.count(name) != 0;
}

ExValue ValueContext::GetVariable(const std::string &name) const {
    return values.at(name);
}

void ValueContext::ModifyVariable(const std::string &name, const ExValue &eval) {
    values[name] = eval;
}

}