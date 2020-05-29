#include "FunctionContext.h"

#include "llvm/IR/BasicBlock.h"

#include "gen.h"
#include "TypeContext.h"
#include "ValueContext.h"
#include "GeneratorContext.h"

namespace gen {

ExValue FuncSign::Call(const std::vector<ExValue> &params) {
    std::vector<llvm::Value *> params_val;
    for (int i = 0; i < params.size(); i++) {
        ExValue cast_param = Cast(args[i].second, params[i]);
        params_val.emplace_back(cast_param.Value());
    }
    ExValue rete;
    rete.type = ret;
    if (ret.type == sem::Type::VOID) {
        ir_builder.CreateCall(fn, params_val);
    } else {
        rete.value = ir_builder.CreateCall(fn, params_val, "call_fn_" + fn_name);
    }
    for (int i : mut_args) {
        std::string tmp_name = args[i].first + "#" + prefix + fn_name;
        ExValue mut_arg = gen_c.GetVariable(tmp_name);
        ExValue param = params[i];
        Assign(param, mut_arg);
    }
    return rete;
}

void FuncSign::Return() {
    for (int i : mut_args) {
        std::string tmp_name = args[i].first + "#" + prefix + fn_name;
        ExValue tmpe = gen_c.GetVariable(tmp_name);
        ExValue arge = gen_c.GetVariable(args[i].first);
        Assign(tmpe, arge);
    }

    if (ret.type == sem::Type::VOID) {
        ir_builder.CreateRetVoid();
    } else {
        ExValue rete = gen_c.GetVariable(fn_name);
        ir_builder.CreateRet(rete.Value());
    }

    gen_c.EndScope();
}

FuncSign FunctionContext::GetSign(const std::string &name, const sem::Type &ret,
        const std::vector<std::pair<std::string, sem::Type>> &args,
        const std::vector<int> mut_args, const std::string &prefix) {
    FuncSign sign;
    sign.prefix = prefix;
    sign.fn_name = name;
    sign.ret = ret;
    sign.args = args;
    sign.mut_args = mut_args;

    for (int i : mut_args) {
        std::string tmp_name = args[i].first + "#" + prefix + name;
        sem::Type tmp_ty = args[i].second;
        gen_c.NewVariable(tmp_name, tmp_ty, true);
    }

    gen_c.NewScope();
    return sign;
}

void FunctionContext::NewFunc(FuncSign &sign) {
    std::vector<llvm::Type *> arg_types;
    for (const auto &[_, ty] : sign.args) {
        arg_types.emplace_back(TypeContext::Type(ty));
    }
    llvm::FunctionType *func_type =
        llvm::FunctionType::get(TypeContext::Type(sign.ret), arg_types, false);
    llvm::Function *func = llvm::Function::Create(func_type,
        llvm::Function::ExternalLinkage, "fn_" + sign.fn_name, &llvm_module);
    sign.fn = func;
    llvm::BasicBlock *block =
        llvm::BasicBlock::Create(llvm_context, "fn_" + sign.fn_name, func);
    ir_builder.SetInsertPoint(block);

    auto arg_it = func->arg_begin();
    for (const auto &[name, ty] : sign.args) {
        gen_c.NewVariable(name, ty);
        arg_it->setName(name);
        ir_builder.CreateStore(arg_it, gen_c.GetVariable(name).addr);
        ++arg_it;
    }
    if (sign.ret.type != sem::Type::VOID) {
        gen_c.NewVariable(sign.fn_name, sign.ret);
    }

    funcs[sign.fn_name] = sign;
}

bool FunctionContext::HasName(const std::string &name) const {
    return funcs.count(name) != 0;
}

FuncSign FunctionContext::GetFunction(const std::string &name) const {
    return funcs.at(name);
}

llvm::Function *FunctionContext::CFunction(const std::string &name,
        llvm::Type *ret, const std::vector<llvm::Type *> &args, bool var_len) {
    llvm::FunctionType *func_type = llvm::FunctionType::get(ret, args, var_len);
    llvm::Function *func = llvm::Function::Create(func_type,
        llvm::Function::ExternalLinkage, name, &llvm_module);
    func->setCallingConv(llvm::CallingConv::C);
    return func;
}

void FunctionContext::MainFunction() {
    llvm::FunctionType *func_type =
        llvm::FunctionType::get(TypeContext::Int(), false);
    llvm::Function *fn_main = llvm::Function::Create(func_type,
        llvm::Function::ExternalLinkage, "main", &llvm_module);
    llvm::BasicBlock *block =
        llvm::BasicBlock::Create(llvm_context, "main", fn_main);
    ir_builder.SetInsertPoint(block);
}

}