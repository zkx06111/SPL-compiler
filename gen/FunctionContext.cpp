#include "FunctionContext.h"

#include "llvm/IR/BasicBlock.h"

#include "gen.h"
#include "TypeContext.h"
#include "ValueContext.h"
#include "GeneratorContext.h"

namespace gen {

void FuncSign::Call(const std::vector<ExValue> &params) {
    std::vector<llvm::Value *> params_val;
    for (int i = 0; i < params.size(); i++) {
        ExValue cast_param = Cast(args[i].second, params[i]);
        params_val.emplace_back(cast_param.Value());
    }
    ir_builder.CreateCall(fn, params_val, "call_fn_" + fn_name);
    
    for (int i : mut_args) {
        std::string tmp_name = args[i].first + "_@_" + fn_name;
        ExValue mut_arg = gen_c.GetVariable(tmp_name);
        ExValue param = params[i];
        Assign(param, mut_arg);
    }
}

void FuncSign::Return() {
    for (int i : mut_args) {
        std::string tmp_name = args[i].first + "_@_" + fn_name;
        ExValue tmpe = gen_c.GetVariable(tmp_name);
        ExValue arge = gen_c.GetVariable(args[i].first);
        Assign(tmpe, arge);
    }

    ir_builder.CreateRetVoid();

    gen_c.EndScope();
}

void FunctionContext::NewFunc(const std::string &name, const sem::Type &ret,
        const std::vector<std::pair<std::string, sem::Type>> &args,
        const std::vector<int> mut_args) {
    FuncSign sign;
    sign.fn_name = name;
    sign.ret = ret;
    sign.args = args;
    sign.mut_args = mut_args;

    if (ret.type != sem::Type::VOID) {
        gen_c.NewVariable(name, ret);
    }

    for (int i : mut_args) {
        std::string tmp_name = args[i].first + "_@_" + name;
        sem::Type tmp_ty = args[i].second;
        gen_c.NewVariable(tmp_name, tmp_ty);
    }

    gen_c.NewScope();

    std::vector<llvm::Type *> arg_types;
    for (const auto &[_, ty] : args) {
        arg_types.emplace_back(TypeContext::Type(ty));
    }
    llvm::FunctionType *func_type =
        llvm::FunctionType::get(TypeContext::Void(), arg_types, false);
    llvm::Function *func = llvm::Function::Create(func_type,
        llvm::Function::ExternalLinkage, name, &llvm_module);
    sign.fn = func;
    llvm::BasicBlock *block =
        llvm::BasicBlock::Create(llvm_context, "fn_" + name, func);
    ir_builder.SetInsertPoint(block);

    auto arg_it = func->arg_begin();
    for (const auto &[name, ty] : args) {
        gen_c.NewVariable(name, ty);
        arg_it->setName(name);
        ir_builder.CreateStore(arg_it, gen_c.GetVariable(name).addr);
        ++arg_it;
    }

    funcs[name] = sign;
}

bool FunctionContext::HasName(const std::string &name) const {
    return funcs.count(name) != 0;
}

FuncSign FunctionContext::GetFunction(const std::string &name) const {
    return funcs.at(name);
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