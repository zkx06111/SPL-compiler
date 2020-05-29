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
    scope_stk.pop();
}

bool GeneratorContext::IsGlobal() const {
    return val_c.size() == 1;
}

void
GeneratorContext::NewVariable(const std::string &name, const sem::Type &type) {
    val_c.back().NewVariable(name, type);
}

bool GeneratorContext::HasVariable(const std::string &name) const {
    for (auto it = val_c.rbegin(); it != val_c.rend(); it++) {
        if (it->HasName(name)) {
            return true;
        }
    }
    return false;
}

ExValue GeneratorContext::GetVariable(const std::string &name) const {
    for (auto it = val_c.rbegin(); it != val_c.rend(); it++) {
        if (it->HasName(name)) {
            return it->GetVariable(name);
        }
    }
    return ExValue {};
}

void
GeneratorContext::ModifyVariable(const std::string &name, const ExValue &eval) {
    for (auto it = val_c.rbegin(); it != val_c.rend(); it++) {
        if (it->HasName(name)) {
            it->ModifyVariable(name, eval);
            return;
        }
    }
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

bool GeneratorContext::HasConst(const std::string &name) const {
    for (auto it = const_c.rbegin(); it != const_c.rend(); it++) {
        if (it->HasName(name)) {
            return true;
        }
    }
    return false;
}

ExValue GeneratorContext::GetConst(const std::string &name) const {
    for (auto it = const_c.rbegin(); it != const_c.rend(); it++) {
        if (it->HasName(name)) {
            return it->GetConst(name);
        }
    }
    return ExValue {};
}

void GeneratorContext::NewFunc(const std::string &name, const sem::Type &ret,
        const std::vector<std::pair<std::string, sem::Type>> &args,
        const std::vector<int> mut_args) {
    func_c.back().NewFunc(name, ret, args, mut_args);
    FuncSign sign = func_c.back().GetFunction(name);
    scope_stk.push(sign);
}

bool GeneratorContext::HasFunction(const std::string &name) const {
    for (auto it = func_c.rbegin(); it != func_c.rend(); it++) {
        if (it->HasName(name)) {
            return true;
        }
    }
    return false;
}

FuncSign GeneratorContext::GetFunction(const std::string &name) const {
    for (auto it = func_c.rbegin(); it != func_c.rend(); it++) {
        if (it->HasName(name)) {
            return it->GetFunction(name);
        }
    }
    return FuncSign { nullptr };
}

FuncSign GeneratorContext::GetCurrentFunction() const {
    return scope_stk.top();
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

bool GeneratorContext::HasLabel() const {
    return label_c.back().HasLabel();
}

void GeneratorContext::DeclLabel(int label) {
    label_c.back().DeclLabel(label);
}

llvm::BasicBlock *GeneratorContext::GetBlock(int label) const {
    if (label_c.back().HasLabel(label)) {
        return label_c.back().GetBlock(label);
    } else {
        return nullptr;
    }
}

}