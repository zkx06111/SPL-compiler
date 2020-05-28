#include "SysFunc.h"

#include "llvm/IR/Function.h"

#include "gen.h"
#include "Operations.h"
#include "ConstContext.h"
#include "TypeContext.h"
#include "FunctionContext.h"

namespace gen {

llvm::Value *DoPred(const sem::Type &ty, llvm::Value *val) {
    return ir_builder.CreateSub(val, ConstContext::Const(1), "sysfn_pred");
}

llvm::Value *DoSucc(const sem::Type &ty, llvm::Value *val) {
    return ir_builder.CreateAdd(val, ConstContext::Const(1), "sysfn_succ");
}

llvm::Value *DoSqr(const sem::Type &ty, llvm::Value *val) {
    if (ty.type == sem::Type::REAL) {
        return ir_builder.CreateFMul(val, val, "sysfn_sqr");
    } else {
        return ir_builder.CreateMul(val, val, "sysfn_sqr");
    }
}

llvm::Value *DoSqrt(const sem::Type &ty, llvm::Value *val) {
    static llvm::Function *func_sqrt = FunctionContext::CFunction("sqrt",
        TypeContext::Real(), { TypeContext::Real() });
    return ir_builder.CreateCall(func_sqrt, { val }, "sysfn_sqrt");
}

llvm::Value *DoAbs(const sem::Type &ty, llvm::Value *val) {
    if (ty.type == sem::Type::REAL) {
        static llvm::Function *func_fabs = FunctionContext::CFunction("fabs",
            TypeContext::Real(), { TypeContext::Real() });
        std::vector<llvm::Value *> args = { val };
        return ir_builder.CreateCall(func_fabs, args, "sysfn_abs");
    } else {
        static llvm::Function *func_abs = FunctionContext::CFunction("abs",
            TypeContext::Int(), { TypeContext::Int() });
        std::vector<llvm::Value *> args = { val };
        return ir_builder.CreateCall(func_abs, args, "sysfn_abs");
    }
}

llvm::Value *DoChr(const sem::Type &ty, llvm::Value *val) {
    return Cast(sem::Type::Char(), sem::Type::Int(), val);
}

llvm::Value *DoOrd(const sem::Type &ty, llvm::Value *val) {
    return val;
}

llvm::Value *DoOdd(const sem::Type &ty, llvm::Value *val) {
    return Cast(sem::Type::Bool(), sem::Type::Int(),
        ir_builder.CreateAnd(val, ConstContext::Const(1), "sysfn_odd"));
}

void Write(const std::vector<sem::Type> &arg_types,
        const std::vector<llvm::Value *> &arg_vals, bool newline) {
    // TODO
}

void Read(const std::vector<sem::Type> &arg_types,
        const std::vector<llvm::Value *> &arg_vals, bool newline) {
    // TODO
}

}