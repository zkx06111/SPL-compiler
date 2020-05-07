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
    return RemoveSubrange(a) == RemoveSubrange(b);
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

Type CanDoAdd(const Type &a, const Type &b) {
    if (a == Type::Void() || b == Type::Void()) {
        return Type::Void();
    }
    Type ta = RemoveSubrange(a);
    Type tb = RemoveSubrange(b);
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
    return Type::Void();
}
Type CanDoSub(const Type &a, const Type &b) {
    if (a == Type::Void() || b == Type::Void()) {
        return Type::Void();
    }
    Type ta = RemoveSubrange(a);
    Type tb = RemoveSubrange(b);
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
    return Type::Void();
}
Type CanDoMod(const Type &a, const Type &b) {
    if (a == Type::Void() || b == Type::Void()) {
        return Type::Void();
    }
    Type ta = RemoveSubrange(a);
    Type tb = RemoveSubrange(b);
    if (ta == tb && ta.type == Type::INT) {
        return ta;
    } else {
        return Type::Void();
    }
}
Type CanDoAnd(const Type &a, const Type &b) {
    if (a == Type::Void() || b == Type::Void()) {
        return Type::Void();
    }
    Type ta = RemoveSubrange(a);
    Type tb = RemoveSubrange(b);
    if (ta == tb && (ta.type == Type::INT || ta.type == Type::BOOL)) {
        return ta;
    } else {
        return Type::Void();
    }
}
Type CanDoCmp(const Type &a, const Type &b) {
    if (a == Type::Void() || b == Type::Void()) {
        return Type::Void();
    }
    Type ta = RemoveSubrange(a);
    Type tb = RemoveSubrange(b);
    if (ta == tb) {
        if (ta.type == Type::INT || ta.type == Type::REAL ||
                ta.type == Type::STRING || ta.type == Type::CHAR ||
                ta.type == Type::ENUM || ta.type == Type::BOOL) {
            return a;
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
    return Type::Void();
}

bool CanDoNot(const Type &type) {
    Type t = RemoveSubrange(type);
    return t.type == Type::INT || t.type == Type::BOOL;
}
bool CanDoNeg(const Type &type) {
    Type t = RemoveSubrange(type);
    return t.type == Type::INT || t.type == Type::REAL;
}

bool CanAssign(const Type &dst, const Type &src) {
    if (dst.type == Type::VOID || src.type == Type::VOID) {
        return false;
    }
    Type dt = RemoveSubrange(dst);
    Type st = RemoveSubrange(src);
    return dt == st || (dt.type == Type::REAL && st.type == Type::INT);
}

}