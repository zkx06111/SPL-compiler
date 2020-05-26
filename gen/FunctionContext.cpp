#include "FunctionContext.h"

#include "llvm/IR/BasicBlock.h"

#include "gen.h"
#include "TypeContext.h"
#include "ValueContext.h"

namespace gen {

void
FunctionContext::NewFunction(const std::string &name, const sem::Func &func) {
    llvm::Type *ret = TypeContext::Type(func.ret);
    std::vector<llvm::Type *> arg_types;
    for (const auto &ty : func.args) {
        arg_types.emplace_back(TypeContext::Type(ty));
    }
    bool has_mut_arg = !func.mut_args.empty(); // TODO
    llvm::FunctionType *func_type =
        llvm::FunctionType::get(ret, arg_types, has_mut_arg);
    llvm::Function *fn = llvm::Function::Create(func_type,
        llvm::Function::ExternalLinkage, name, &llvm_module);
    funcs[name] = fn;
    llvm::BasicBlock *block =
        llvm::BasicBlock::Create(llvm_context, "fn_" + name, fn);
    ir_builder.SetInsertPoint(block);
}

bool FunctionContext::HasName(const std::string &name) const {
    return funcs.count(name) != 0;
}

llvm::Function *FunctionContext::GetFunction(const std::string &name) const {
    return funcs.at(name);
}

void FunctionContext::Return(llvm::Value *val) {
    ir_builder.CreateRet(val);
}

llvm::Function *FunctionContext::CFunction(const std::string &name,
        llvm::Type *ret, const std::vector<llvm::Type *> &args) {
    llvm::FunctionType *func_type = llvm::FunctionType::get(ret, args, false);
    llvm::Function *func = llvm::Function::Create(func_type,
        llvm::Function::ExternalLinkage, name, &llvm_module);
    func->setCallingConv(llvm::CallingConv::C);
    return func;
}

}