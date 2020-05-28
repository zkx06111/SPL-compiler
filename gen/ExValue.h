#pragma once

#include "llvm/IR/Value.h"
#include "Type.h"
#include "ConstTable.h"

namespace gen {

struct ExValue {
    mutable llvm::Value *value = nullptr;
    sem::Type type = sem::Type::Void();
    union {
        int val_i;
        double val_r;
        bool val_b;
        char val_c;
    };
    bool is_const = false;
    llvm::Value *addr = nullptr;

    llvm::Value *Value() const;
};

ExValue Cast(const sem::Type &type, const ExValue &val);
void Assign(ExValue &dst, const ExValue &src);

ExValue DoAdd(const ExValue &lhs, const ExValue &rhs);
ExValue DoSub(const ExValue &lhs, const ExValue &rhs);
ExValue DoMul(const ExValue &lhs, const ExValue &rhs);
ExValue DoDiv(const ExValue &lhs, const ExValue &rhs);
ExValue DoMod(const ExValue &lhs, const ExValue &rhs);
ExValue DoNeg(const ExValue &val);

ExValue DoAnd(const ExValue &lhs, const ExValue &rhs);
ExValue DoOr(const ExValue &lhs, const ExValue &rhs);
ExValue DoXor(const ExValue &lhs, const ExValue &rhs);
ExValue DoNot(const ExValue &val);

ExValue CmpEqual(const ExValue &lhs, const ExValue &rhs);
ExValue CmpNEqual(const ExValue &lhs, const ExValue &rhs);
ExValue CmpLess(const ExValue &lhs, const ExValue &rhs);
ExValue CmpLessEq(const ExValue &lhs, const ExValue &rhs);
ExValue CmpGreat(const ExValue &lhs, const ExValue &rhs);
ExValue CmpGreatEq(const ExValue &lhs, const ExValue &rhs);

ExValue DoPred(const ExValue &val);
ExValue DoSucc(const ExValue &val);
ExValue DoSqr(const ExValue &val);
ExValue DoSqrt(const ExValue &val);
ExValue DoAbs(const ExValue &val);
ExValue DoChr(const ExValue &val);
ExValue DoOrd(const ExValue &val);
ExValue DoOdd(const ExValue &val);

}