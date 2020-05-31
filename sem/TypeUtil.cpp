#include "TypeUtil.h"

namespace sem {

// TODO: deal with subrange

bool IsVoid(const Type &type) {
    return type.type == Type::VOID;
}

Type RemoveSubrange(const Type &type) {
    if (type.type != Type::SUBRANGE) {
        return type;
    }
    return sym_t.GetSubrange(type).type;
}

bool IsAlmostSame(const Type &a, const Type &b) {
    auto ta = RemoveSubrange(a);
    auto tb = RemoveSubrange(b);
    if (ta == tb) {
        return true;
    } else if (ta.type == sem::Type::ARRAY && tb.type == sem::Type::ARRAY) {
        auto arr_a = sym_t.GetArray(a);
        auto arr_b = sym_t.GetArray(b);
        auto ind_a = arr_a.ind_type;
        auto ind_b = arr_b.ind_type;
        if (ind_a.type == sem::Type::BOOL) {
            if (ind_b.type != sem::Type::BOOL) {
                return false;
            }
        } else if (ind_a.type == sem::Type::ENUM) {
            if (ind_a != ind_b) {
                return false;
            }
        } else if (ind_a.type == sem::Type::SUBRANGE) {
            if (ind_b.type != sem::Type::SUBRANGE) {
                return false;
            }
            auto rng_a = sym_t.GetSubrange(ind_a);
            auto rng_b = sym_t.GetSubrange(ind_b);
            if (rng_a.type != rng_b.type || rng_a.l != rng_b.l ||
                    rng_a.r != rng_b.r) {
                return false;
            }
        }
        return IsAlmostSame(arr_a.ele_type, arr_b.ele_type);
    } else {
        return false;
    }
}

bool IsValidFunc(const Func &func) {
    for (const auto &type : func.args) {
        if (type.type == Type::VOID) {
            return true;
        }
    }
    return false;
}

bool CanBeArrayIndType(const Type &type) {
    return type.type == Type::SUBRANGE || type.type == Type::BOOL ||
        type.type == Type::ENUM;
}

Type DoAdd(const Type &a, const Type &b) {
    if (a == Type::Void() || b == Type::Void()) {
        throw SemError("lhs type and rhs type don't match");
    }
    Type ta = RemoveSubrange(a);
    Type tb = RemoveSubrange(b);
    ta.is_lval = tb.is_lval = false;
    if (ta == tb) {
        if (ta.type == Type::INT || ta.type == Type::REAL ||
                ta.type == Type::STRING) {
            return ta;
        }
    } else {
        if ((ta.type == Type::INT && tb.type == Type::REAL) ||
            (ta.type == Type::REAL && tb.type == Type::INT)) {
            return Type::Real();
        }
        if ((ta.type == Type::STRING && tb.type == Type::CHAR) ||
            (ta.type == Type::CHAR && tb.type == Type::STRING)) {
            return Type::String();
        }
    }
    throw SemError("lhs type and rhs type don't match");
}
Type DoSub(const Type &a, const Type &b) {
    if (a == Type::Void() || b == Type::Void()) {
        throw SemError("lhs type and rhs type don't match");
    }
    Type ta = RemoveSubrange(a);
    Type tb = RemoveSubrange(b);
    ta.is_lval = tb.is_lval = false;
    if (ta == tb) {
        if (ta.type == Type::INT || ta.type == Type::REAL) {
            return ta;
        }
    } else {
        if ((ta.type == Type::INT && tb.type == Type::REAL) ||
            (ta.type == Type::REAL && tb.type == Type::INT)) {
            return Type::Real();
        }
    }
    throw SemError("lhs type and rhs type don't match");
}
Type DoMod(const Type &a, const Type &b) {
    if (a == Type::Void() || b == Type::Void()) {
        throw SemError("lhs type and rhs type don't match");
    }
    Type ta = RemoveSubrange(a);
    Type tb = RemoveSubrange(b);
    ta.is_lval = tb.is_lval = false;
    if (ta == tb && ta.type == Type::INT) {
        return ta;
    } else {
        throw SemError("lhs type and rhs type don't match");
    }
}
Type DoAnd(const Type &a, const Type &b) {
    if (a == Type::Void() || b == Type::Void()) {
        throw SemError("lhs type and rhs type don't match");
    }
    Type ta = RemoveSubrange(a);
    Type tb = RemoveSubrange(b);
    ta.is_lval = tb.is_lval = false;
    if (ta == tb && (ta.type == Type::INT || ta.type == Type::BOOL)) {
        return ta;
    } else {
        throw SemError("lhs type and rhs type don't match");
    }
}
Type DoCmp(const Type &a, const Type &b) {
    if (a == Type::Void() || b == Type::Void()) {
        throw SemError("lhs type and rhs type don't match");
    }
    Type ta = RemoveSubrange(a);
    Type tb = RemoveSubrange(b);
    ta.is_lval = tb.is_lval = false;
    if (ta == tb) {
        if (ta.type == Type::INT || ta.type == Type::REAL ||
                ta.type == Type::STRING || ta.type == Type::CHAR ||
                ta.type == Type::ENUM || ta.type == Type::BOOL) {
            return Type::Bool();
        }
    } else {
        if ((ta.type == Type::INT && tb.type == Type::REAL) ||
            (ta.type == Type::REAL && tb.type == Type::INT)) {
            return Type::Bool();
        }
        if ((ta.type == Type::STRING && tb.type == Type::CHAR) ||
            (ta.type == Type::CHAR && tb.type == Type::STRING)) {
            return Type::Bool();
        }
    }
    throw SemError("lhs type and rhs type don't match");
}

Type DoNot(const Type &type) {
    Type t = RemoveSubrange(type);
    t.is_lval = false;
    if (t.type == Type::INT || t.type == Type::BOOL) {
        return t;
    } else {
        throw SemError("type can't do not operation");
    }
}
Type DoNeg(const Type &type) {
    Type t = RemoveSubrange(type);
    t.is_lval = false;
    if (t.type == Type::INT || t.type == Type::REAL) {
        return t;
    } else {
        throw SemError("type can't do negtive operation");
    }
}

bool CanAssign(const Type &dst, const Type &src) {
    if (dst.type == Type::VOID || src.type == Type::VOID) {
        return false;
    }
    if (IsAlmostSame(dst, src)) {
        return true;
    }
    Type dt = RemoveSubrange(dst);
    Type st = RemoveSubrange(src);
    if (dt.type == Type::REAL && st.type == Type::INT) {
        return true;
    } else {
        return false;
    }
}
Type DoAssign(const Type &dst, const Type &src) {
    if (CanAssign(dst, src)) {
        return dst;
    } else {
        throw SemError("can't do assignment from rhs type to lhs type");
    }
}

Type DoAbs(const Type &type) {
    Type t = RemoveSubrange(type);
    t.is_lval = false;
    if (t.type == Type::INT || t.type == Type::REAL) {
        return t;
    } else {
        throw SemError("can't assign the 1st parameter");
    }
}
Type DoChr(const Type &type) {
    Type t = RemoveSubrange(type);
    t.is_lval = false;
    if (t.type == Type::INT) {
        return Type::Char();
    } else {
        throw SemError("can't assign the 1st parameter");
    }
}
Type DoOdd(const Type &type) {
    Type t = RemoveSubrange(type);
    t.is_lval = false;
    if (t.type == Type::INT) {
        return Type::Bool();
    } else {
        throw SemError("can't assign the 1st parameter");
    }
}
Type DoOrd(const Type &type) {
    Type t = RemoveSubrange(type);
    t.is_lval = false;
    if (t.type == Type::INT || t.type == Type::ENUM || t.type == Type::CHAR) {
        return Type::Int();
    } else {
        throw SemError("can't assign the 1st parameter");
    }
}
Type DoPred(const Type &type) {
    Type t = RemoveSubrange(type);
    t.is_lval = false;
    if (t.type == Type::INT || t.type == Type::ENUM) {
        return t;
    } else {
        throw SemError("can't assign the 1st parameter");
    }
}
Type DoSqrt(const Type &type) {
    Type t = RemoveSubrange(type);
    t.is_lval = false;
    if (t.type == Type::INT || t.type == Type::REAL) {
        return Type::Real();
    } else {
        throw SemError("can't assign the 1st parameter");
    }
}

}