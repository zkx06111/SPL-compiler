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
    return Cast(sem::Type::Int(), ty, val);
}

llvm::Value *DoOdd(const sem::Type &ty, llvm::Value *val) {
    return Cast(sem::Type::Bool(), sem::Type::Int(),
        ir_builder.CreateAnd(val, ConstContext::Const(1), "sysfn_odd"));
}

void Write(const std::vector<ExValue> &params, bool newline) {
    static llvm::Function *func_printf = FunctionContext::CFunction("printf",
        TypeContext::Int(), { TypeContext::CharPtr() }, true);
    
    std::vector<llvm::Value *> args = { nullptr };
    std::string fmt_str;
    bool fir = true;
    for (const auto &eval : params) {
        if (fir) {
            fir = false;
        } else {
            fmt_str += ", ";
        }
        auto ty = eval.type.type;
        if (ty == sem::Type::INT || ty == sem::Type::BOOL) {
            fmt_str += "%d";
            args.emplace_back(Cast(sem::Type::Int(), eval).Value());
        } else if (ty == sem::Type::REAL) {
            fmt_str += "%f";
            args.emplace_back(eval.Value());
        } else if (ty == sem::Type::CHAR) {
            fmt_str += "%c";
            args.emplace_back(eval.Value());
        }
    }
    if (newline) {
        fmt_str += "\n";
    }

    args[0] = ir_builder.CreateGlobalStringPtr(fmt_str, "printf_fmt_str");
    ir_builder.CreateCall(func_printf, args, "sysfn_write");
}

void Read(const std::vector<ExValue> &params, bool newline) {
    static llvm::Function *func_scanf = FunctionContext::CFunction("scanf",
        TypeContext::Int(), { TypeContext::CharPtr() }, true);
    
    std::vector<llvm::Value *> args = { nullptr };
    std::string fmt_str;
    for (const auto &eval : params) {
        auto ty = eval.type.type;
        if (ty == sem::Type::INT) {
            fmt_str += "%d";
            args.emplace_back(eval.addr);
        } else if (ty == sem::Type::REAL) {
            fmt_str += "%f";
            args.emplace_back(eval.addr);
        } else if (ty == sem::Type::CHAR) {
            fmt_str += "%c";
            args.emplace_back(eval.addr);
        }
    }
    if (newline) {
        fmt_str += "\n";
    }

    args[0] = ir_builder.CreateGlobalStringPtr(fmt_str, "scanf_fmt_str");
    ir_builder.CreateCall(func_scanf, args, "sysfn_read");
}

}