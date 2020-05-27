#include "GeneratorContext.h"

namespace gen {

GeneratorContext gen_c;

GeneratorContext::GeneratorContext() {
    const_c.emplace_back();
    func_c.emplace_back();
    label_c.emplace_back();
    type_c.emplace_back();
    val_c.emplace_back();
}

void GeneratorContext::NewScope() {
    const_c.emplace_back();
    func_c.emplace_back();
    label_c.emplace_back();
    type_c.emplace_back();
    val_c.emplace_back();
}

void GeneratorContext::EndScope() {
    const_c.pop_back();
    func_c.pop_back();
    label_c.pop_back();
    type_c.pop_back();
    val_c.pop_back();
}

bool GeneratorContext::IsGlobal() const {
    return val_c.size() == 1;
}

void
GeneratorContext::NewVariable(const std::string &name, const sem::Type &type) {
    val_c.back().NewVariable(name, type);
}

llvm::Value *GeneratorContext::GetVariable(const std::string &name) const {
    for (auto it = val_c.rbegin(); it != val_c.rend(); it++) {
        if (it->HasName(name)) {
            return it->GetVariable(name);
        }
    }
    return nullptr;
}

void GeneratorContext::NewConstant(const std::string &name, int val) {
    const_c.back().NewConstant(name, val);
}
void GeneratorContext::NewConstant(const std::string &name, double val) {
    const_c.back().NewConstant(name, val);
}
void GeneratorContext::NewConstant(const std::string &name, bool val) {
    const_c.back().NewConstant(name, val);
}
void GeneratorContext::NewConstant(const std::string &name, char val) {
    const_c.back().NewConstant(name, val);
}

llvm::Constant *GeneratorContext::GetConst(const std::string &name) const {
    for (auto it = const_c.rbegin(); it != const_c.rend(); it++) {
        if (it->HasName(name)) {
            return it->GetConst(name);
        }
    }
    return nullptr;
}

void
GeneratorContext::NewFunction(const std::string &name, const sem::Func &func) {
    func_c.back().NewFunction(name, func);
    NewScope();
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

void GeneratorContext::NewLabel(int label) {
    label_c.back().NewLabel(label);
}

llvm::BasicBlock *GeneratorContext::GetBlock(int label) const {
    for (auto it = label_c.rbegin(); it != label_c.rend(); it++) {
        if (it->HasLabel(label)) {
            return it->GetBlock(label);
        }
    }
    return nullptr;
}

}