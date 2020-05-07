#pragma once

#include "SymbolTable.h"

namespace sem {

bool IsVoid(const Type &type);

Type RemoveSubrange(const Type &type);
bool IsAlmostSame(const Type &a, const Type &b);

bool IsValidFunc(const Func &func);

bool CanBeArrayIndType(const Type &type);

Type CanDoAdd(const Type &a, const Type &b);
Type CanDoSub(const Type &a, const Type &b);
inline Type CanDoMul(const Type &a, const Type &b) {
    return CanDoSub(a, b);
}
inline Type CanDoDiv(const Type &a, const Type &b) {
    return CanDoSub(a, b);
}
Type CanDoMod(const Type &a, const Type &b);
Type CanDoAnd(const Type &a, const Type &b);
inline Type CanDoOr(const Type &a, const Type &b) {
    return CanDoAnd(a, b);
}
inline Type CanDoXor(const Type &a, const Type &b) {
    return CanDoMod(a, b);
}
Type CanDoCmp(const Type &a, const Type &b);

bool CanDoNot(const Type &type);
bool CanDoNeg(const Type &type);

bool CanAssign(const Type &dst, const Type &src);

// maybe TODO

}