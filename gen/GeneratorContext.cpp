#include "GeneratorContext.h"

namespace gen {

GeneratorContext gen_c;

GeneratorContext::GeneratorContext() {
    func_c.emplace_back();
    val_c.emplace_back();
}

void GeneratorContext::NewScope() {
    func_c.emplace_back();
    val_c.emplace_back();
}

void GeneratorContext::EndScope() {
    func_c.pop_back();
    val_c.pop_back();
}

bool GeneratorContext::IsGlobal() const {
    return val_c.size() == 1;
}

void
GeneratorContext::NewVariable(const std::string &name, const sem::Type &type) {
    val_c.back().NewVariable(name, type);
}

void GeneratorContext::NewConstant(const std::string &name, int val) {
    val_c.back().NewConstant(name, val);
}
void GeneratorContext::NewConstant(const std::string &name, double val) {
    val_c.back().NewConstant(name, val);
}
void GeneratorContext::NewConstant(const std::string &name, bool val) {
    val_c.back().NewConstant(name, val);
}
void GeneratorContext::NewConstant(const std::string &name, char val) {
    val_c.back().NewConstant(name, val);
}

llvm::Value *GeneratorContext::GetValue(const std::string &name) const {
    for (auto it = val_c.rbegin(); it != val_c.rend(); it++) {
        if (it->HasName(name)) {
            return it->GetValue(name);
        }
    }
    return nullptr;
}

void
GeneratorContext::NewFunction(const std::string &name, const sem::Func &func) {
    func_c.back().NewFunction(name, func);
}

llvm::Function *GeneratorContext::GetFunction(const std::string &name) const {
    for (auto it = func_c.rbegin(); it != func_c.rend(); it++) {
        if (it->HasName(name)) {
            return it->GetFunction(name);
        }
    }
    return nullptr;
}

void GeneratorContext::NewType(const std::string &name, const sem::Type &type) {
    type_c.back().NameType(name, type);
}

llvm::Type *GeneratorContext::GetType(const std::string &name) const {
    for (auto it = type_c.rbegin(); it != type_c.rend(); it++) {
        if (it->HasName(name)) {
            return it->GetType(name);
        }
    }
    return nullptr;
}

}