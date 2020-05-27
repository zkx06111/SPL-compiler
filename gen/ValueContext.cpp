#include "ValueContext.h"

#include "gen.h"
#include "TypeContext.h"
#include "GeneratorContext.h"

namespace gen {

void ValueContext::NewVariable(const std::string &name, const sem::Type &type) {
    llvm::Type *ty = TypeContext::Type(type);
    llvm::Value *value;
    if (gen_c.IsGlobal()) {
        value = new llvm::GlobalVariable(llvm_module, ty, false,
            llvm::GlobalValue::InternalLinkage,
            llvm::ConstantAggregateZero::get(ty), name);
    } else {
        value = ir_builder.CreateAlloca(ty, nullptr, name);
    }
    values[name] = value;
}

bool ValueContext::HasName(const std::string &name) const {
    return values.count(name) != 0;
}

llvm::Value *ValueContext::GetVariable(const std::string &name) const {
    return values.at(name);
}

}