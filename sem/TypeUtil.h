#pragma once

#include "SymbolTable.h"
#include "SemError.h"

namespace sem {

bool IsVoid(const Type &type);

Type RemoveSubrange(const Type &type);
bool IsAlmostSame(const Type &a, const Type &b);

bool IsValidFunc(const Func &func);

bool CanBeArrayIndType(const Type &type);

Type DoAdd(const Type &a, const Type &b);
Type DoSub(const Type &a, const Type &b);
inline Type DoMul(const Type &a, const Type &b) {
    return DoSub(a, b);
}
inline Type DoDiv(const Type &a, const Type &b) {
    return DoSub(a, b);
}
Type DoMod(const Type &a, const Type &b);
Type DoAnd(const Type &a, const Type &b);
inline Type DoOr(const Type &a, const Type &b) {
    return DoAnd(a, b);
}
inline Type DoXor(const Type &a, const Type &b) {
    return DoMod(a, b);
}
Type DoCmp(const Type &a, const Type &b);

Type DoNot(const Type &type);
Type DoNeg(const Type &type);

bool CanAssign(const Type &dst, const Type &src);
Type DoAssign(const Type &dst, const Type &src);

Type DoAbs(const Type &type);
Type DoChr(const Type &type);
Type DoOdd(const Type &type);
Type DoOrd(const Type &type);
Type DoPred(const Type &type);
inline Type DoSqr(const Type &type) {
    return DoAbs(type);
}
Type DoSqrt(const Type &type);
inline Type DoSucc(const Type &type) {
    return DoPred(type);
}

}