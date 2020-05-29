#include "ValueContext.h"

#include "gen.h"
#include "TypeContext.h"
#include "GeneratorContext.h"

namespace gen {

void ValueContext::NewVariable(const std::string &name, const sem::Type &type,
        bool is_global) {
    llvm::Type *ty = TypeContext::Type(type);
    llvm::Value *value;
    if (is_global || gen_c.IsGlobal()) {
        llvm::Constant *init;
        if (type.type == sem::Type::INT) {
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