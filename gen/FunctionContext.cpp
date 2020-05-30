#include "FunctionContext.h"

#include "llvm/IR/BasicBlock.h"

#include "gen.h"
#include "TypeContext.h"
#include "ValueContext.h"
#include "GeneratorContext.h"

namespace gen {

ExValue FuncSign::Call(std::vector<ExValue> params) {
    std::vector<llvm::Value *> params_val;
    for (int i = 0, j = 0; i < params.size(); i++) {
        if (j < mut_args.size() && mut_args[j] == i) {
            params[i].is_const = false;
            params_val.emplace_back(params[i].Addr());
            ++j;
        } else {
            ExValue cast_param = Cast(args[i].second, params[i]);
            params_val.emplace_back(cast_param.Value());
        }
    }
    ExValue rete;
    rete.type = ret;
    if (ret.type == sem::Type::VOID) {
        ir_builder.CreateCall(fn, params_val);
    } else {
        rete.value = ir_builder.CreateCall(fn, params_val, "call_fn_" + fn_name);
    }
    return rete;
}

void FuncSign::Return() {
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

    return sign;
}

void FunctionContext::NewFunc(FuncSign &sign) {
    std::vector<llvm::Type *> arg_types;
    for (int i = 0, j = 0; i < sign.args.size(); i++) {
        if (j < sign.mut_args.size() && i == sign.mut_args[j]) {
            arg_types.emplace_back(TypeContext::PtrType(sign.args[i].second));
            ++j;
        } else {
            arg_types.emplace_back(TypeContext::Type(sign.args[i].second));
        }
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
    for (int i = 0, j = 0; i < sign.args.size(); i++) {
        const auto &[name, ty] = sign.args[i];
        if (j < sign.mut_args.size() && i == sign.mut_args[j]) {
            gen_c.NewVariable(name, ty, false, true);
            ++j;
        } else {
            gen_c.NewVariable(name, ty, false);
        }
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