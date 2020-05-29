#pragma once

#include "llvm/IR/Value.h"

#include "Type.h"

namespace gen {

llvm::Value *
Cast(const sem::Type &dst_ty, const sem::Type &src_ty, llvm::Value *src_val);

llvm::Value *GetPointer(llvm::Value *val);
llvm::Value *Deref(llvm::Value *ptr);

void Assign(llvm::Value *dst, llvm::Value *src);
void Assign(const sem::Type &dst_ty, llvm::Value *dst_val,
    const sem::Type &src_ty, llvm::Value *src_val);

llvm::Value *DoAdd(const sem::Type &ty, llvm::Value *lhs, llvm::Value *rhs);
llvm::Value *DoSub(const sem::Type &ty, llvm::Value *lhs, llvm::Value *rhs);
llvm::Value *DoMul(const sem::Type &ty, llvm::Value *lhs, llvm::Value *rhs);
llvm::Value *DoDiv(const sem::Type &ty, llvm::Value *lhs, llvm::Value *rhs);
llvm::Value *DoMod(const sem::Type &ty, llvm::Value *lhs, llvm::Value *rhs);
llvm::Value *DoNeg(const sem::Type &ty, llvm::Value *val);

llvm::Value *DoAnd(const sem::Type &ty, llvm::Value *lhs, llvm::Value *rhs);
llvm::Value *DoOr(const sem::Type &ty, llvm::Value *lhs, llvm::Value *rhs);
llvm::Value *DoXor(const sem::Type &ty, llvm::Value *lhs, llvm::Value *rhs);
llvm::Value *DoNot(const sem::Type &ty, llvm::Value *val);

llvm::Value *CmpEqual(const sem::Type &ty, llvm::Value *lhs, llvm::Value *rhs);
llvm::Value *CmpNEqual(const sem::Type &ty, llvm::Value *lhs, llvm::Value *rhs);
llvm::Value *CmpLess(const sem::Type &ty, llvm::Value *lhs, llvm::Value *rhs);
llvm::Value *CmpLessEq(const sem::Type &ty, llvm::Value *lhs, llvm::Value *rhs);
llvm::Value *CmpGreat(const sem::Type &ty, llvm::Value *lhs, llvm::Value *rhs);
llvm::Value *CmpGreatEq(const sem::Type &ty, llvm::Value *lhs, llvm::Value *rhs);

}