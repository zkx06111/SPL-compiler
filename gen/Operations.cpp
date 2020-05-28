#include "Operations.h"

#include "gen.h"
#include "TypeContext.h"

namespace gen {

llvm::Value *
Cast(const sem::Type &dst_ty, const sem::Type &src_ty, llvm::Value *src_val) {
    if (dst_ty == src_ty) {
        return src_val;
    }
    auto sty = src_ty.type;
    auto dty = dst_ty.type;
    if (sty == sem::Type::INT && dty == sem::Type::REAL) {
        return ir_builder.CreateSIToFP(src_val, TypeContext::Real(), "cast_i2r");
    } else if (sty == sem::Type::BOOL && dty == sem::Type::INT) {
        return ir_builder.CreateSExt(src_val, TypeContext::Int(), "cast_b2i");
    } else if (sty == sem::Type::CHAR && dty == sem::Type::INT) {
        return ir_builder.CreateSExt(src_val, TypeContext::Int(), "cast_c2i");
    }
    return src_val;
}

llvm::Value *GetPointer(llvm::Value *val) {
    auto *load_inst = llvm::dyn_cast<llvm::LoadInst>(val);
    if (load_inst) {
        return load_inst->getPointerOperand();
    } else {
        return nullptr;
    }
}
llvm::Value *Deref(llvm::Value *ptr) {
    return ir_builder.CreateLoad(ptr, "deref");
}

void Assign(llvm::Value *dst, llvm::Value *src) {
    ir_builder.CreateStore(src, dst);
}
void Assign(const sem::Type &dst_ty, llvm::Value *dst_val,
        const sem::Type &src_ty, llvm::Value *src_val) {
    ir_builder.CreateStore(Cast(dst_ty, src_ty, src_val), dst_val);
}

llvm::Value *DoAdd(const sem::Type &ty, llvm::Value *lhs, llvm::Value *rhs) {
    if (ty.type == sem::Type::REAL) {
        return ir_builder.CreateFAdd(lhs, rhs, "binop_fadd");
    } else {
        return ir_builder.CreateAdd(lhs, rhs, "binop_add");
    }
}
llvm::Value *DoSub(const sem::Type &ty, llvm::Value *lhs, llvm::Value *rhs) {
    if (ty.type == sem::Type::REAL) {
        return ir_builder.CreateFSub(lhs, rhs, "binop_fsub");
    } else {
        return ir_builder.CreateSub(lhs, rhs, "binop_sub");
    }
}
llvm::Value *DoMul(const sem::Type &ty, llvm::Value *lhs, llvm::Value *rhs) {
    if (ty.type == sem::Type::REAL) {
        return ir_builder.CreateFMul(lhs, rhs, "binop_fmul");
    } else {
        return ir_builder.CreateMul(lhs, rhs, "binop_mul");
    }
}
llvm::Value *DoDiv(const sem::Type &ty, llvm::Value *lhs, llvm::Value *rhs) {
    if (ty.type == sem::Type::REAL) {
        return ir_builder.CreateFDiv(lhs, rhs, "binop_fdiv");
    } else {
        return ir_builder.CreateSDiv(lhs, rhs, "binop_div");
    }
}
llvm::Value *DoMod(const sem::Type &ty, llvm::Value *lhs, llvm::Value *rhs) {
    return ir_builder.CreateSRem(lhs, rhs, "binop_mod");
}
llvm::Value *DoNeg(const sem::Type &ty, llvm::Value *val) {
    if (ty.type == sem::Type::REAL) {
        return ir_builder.CreateFNeg(val, "unop_fneg");
    } else {
        return ir_builder.CreateNeg(val, "unop_neg");
    }
}

llvm::Value *DoAnd(const sem::Type &ty, llvm::Value *lhs, llvm::Value *rhs) {
    return ir_builder.CreateAnd(lhs, rhs, "binop_and");
}
llvm::Value *DoOr(const sem::Type &ty, llvm::Value *lhs, llvm::Value *rhs) {
    return ir_builder.CreateOr(lhs, rhs, "binop_and");
}
llvm::Value *DoXor(const sem::Type &ty, llvm::Value *lhs, llvm::Value *rhs) {
    return ir_builder.CreateXor(lhs, rhs, "binop_and");
}
llvm::Value *DoNot(const sem::Type &ty, llvm::Value *val) {
    return ir_builder.CreateNot(val, "unop_not");
}

llvm::Value *CmpEqual(const sem::Type &ty, llvm::Value *lhs, llvm::Value *rhs) {
    if (ty.type == sem::Type::REAL) {
        return ir_builder.CreateFCmpOEQ(lhs, rhs, "binop_cmp_eq");
    } else {
        return ir_builder.CreateICmpEQ(lhs, rhs, "binop_cmp_eq");
    }
}
llvm::Value *CmpNEqual(const sem::Type &ty, llvm::Value *lhs, llvm::Value *rhs) {
    if (ty.type == sem::Type::REAL) {
        return ir_builder.CreateFCmpONE(lhs, rhs, "binop_cmp_neq");
    } else {
        return ir_builder.CreateICmpNE(lhs, rhs, "binop_cmp_neq");
    }
}
llvm::Value *CmpLess(const sem::Type &ty, llvm::Value *lhs, llvm::Value *rhs) {
    if (ty.type == sem::Type::REAL) {
        return ir_builder.CreateFCmpOLT(lhs, rhs, "binop_cmp_lt");
    } else {
        return ir_builder.CreateICmpSLT(lhs, rhs, "binop_cmp_lt");
    }
}
llvm::Value *CmpLessEq(const sem::Type &ty, llvm::Value *lhs, llvm::Value *rhs) {
    if (ty.type == sem::Type::REAL) {
        return ir_builder.CreateFCmpOLE(lhs, rhs, "binop_cmp_leq");
    } else {
        return ir_builder.CreateICmpSLE(lhs, rhs, "binop_cmp_leq");
    }
}
llvm::Value *CmpGreat(const sem::Type &ty, llvm::Value *lhs, llvm::Value *rhs) {
    if (ty.type == sem::Type::REAL) {
        return ir_builder.CreateFCmpOGT(lhs, rhs, "binop_cmp_gt");
    } else {
        return ir_builder.CreateICmpSGT(lhs, rhs, "binop_cmp_gt");
    }
}
llvm::Value *CmpGreatEq(const sem::Type &ty, llvm::Value *lhs, llvm::Value *rhs) {
    if (ty.type == sem::Type::REAL) {
        return ir_builder.CreateFCmpOGE(lhs, rhs, "binop_cmp_geq");
    } else {
        return ir_builder.CreateICmpSGE(lhs, rhs, "binop_cmp_geq");
    }
}

}