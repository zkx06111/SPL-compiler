#include "ExValue.h"

#include "ConstContext.h"
#include "Operations.h"
#include "SysFunc.h"
#include "TypeUtil.h"

namespace gen {

llvm::Value *ExValue::Value() const {
    if (is_ref) {
        return Deref(Deref(addr));
    }
    if (addr != nullptr && !is_const) {
        return Deref(addr);
    }
    if (value == nullptr && is_const) {
        if (type.type == sem::Type::INT) {
            value = ConstContext::Const(val_i);
        } else if (type.type == sem::Type::REAL) {
            value = ConstContext::Const(val_r);
        } else if (type.type == sem::Type::CHAR) {
            value = ConstContext::Const(val_c);
        } else if (type.type == sem::Type::BOOL) {
            value = ConstContext::Const(val_b);
        }
    }
    return value;
}

llvm::Value *ExValue::Addr() const {
    if (is_ref) {
        return Deref(addr);
    } else {
        return addr;
    }
}

ExValue Cast(const sem::Type &type, const ExValue &val) {
    if (val.type == type) {
        return val;
    } else {
        ExValue ret;
        ret.type = type;
        ret.value = Cast(type, val.type, val.Value());
        if (val.is_const) {
            ret.is_const = true;
            auto ty = type.type;
            auto vty = val.type.type;
            if (ty == sem::Type::INT && vty == sem::Type::CHAR) {
                ret.val_i = val.val_c;
            } else if (ty == sem::Type::INT && vty == sem::Type::BOOL) {
                ret.val_i = val.val_b;
            } else if (ty == sem::Type::INT && vty == sem::Type::ENUM) {
                ret.val_i = val.val_i;
            } else if (ty == sem::Type::REAL && vty == sem::Type::INT) {
                ret.val_r = val.val_i;
            } else if (ty == sem::Type::INT) {
                ret.val_i = val.val_i;
            } else if (ty == sem::Type::REAL) {
                ret.val_r = val.val_r;
            } else if (ty == sem::Type::CHAR) {
                ret.val_c = val.val_c;
            } else if (ty == sem::Type::BOOL) {
                ret.val_b = val.val_b;
            }
        }
        return ret;
    }
}

void Assign(ExValue &dst, const ExValue &src) {
    if (src.is_const) {
        auto dty = dst.type.type;
        auto sty = src.type.type;
        if (dty == sem::Type::REAL && sty == sem::Type::INT) {
            dst.val_r = src.val_i;
        } else {
            dst.type = src.type;
            if (dty == sem::Type::INT || dty == sem::Type::ENUM) {
                dst.val_i = src.val_i;
            } else if (dty == sem::Type::REAL) {
                dst.val_r = src.val_r;
            } else if (dty == sem::Type::CHAR) {
                dst.val_c = src.val_c;
            } else if (dty == sem::Type::BOOL) {
                dst.val_b = src.val_b;
            }
        }
        dst.is_const = true;
    } else {
        dst.is_const = false;
    }

    Assign(dst.type, dst.Addr(), src.type, src.Value());
}

ExValue DoAdd(const ExValue &lhs, const ExValue &rhs) {
    ExValue ret;
    ret.type = sem::DoAdd(lhs.type, rhs.type);

    if (lhs.is_const && rhs.is_const) {
        ret.is_const = true;
        auto ty = ret.type.type;
        auto lty = lhs.type.type;
        auto rty = rhs.type.type;
        if (ty == sem::Type::INT) {
            ret.val_i = lhs.val_i + rhs.val_i;
        } else {
            ret.val_r = 0;
            if (lty == sem::Type::REAL) {
                ret.val_r += lhs.val_r;
            } else {
                ret.val_r += lhs.val_i;
            }
            if (rty == sem::Type::REAL) {
                ret.val_r += rhs.val_r;
            } else {
                ret.val_r += rhs.val_i;
            }
        }
    } else {
        ret.value = DoAdd(ret.type, Cast(ret.type, lhs.type, lhs.Value()),
            Cast(ret.type, rhs.type, rhs.Value()));
    }
    return ret;
}

ExValue DoSub(const ExValue &lhs, const ExValue &rhs) {
    ExValue ret;
    ret.type = sem::DoSub(lhs.type, rhs.type);

    if (lhs.is_const && rhs.is_const) {
        ret.is_const = true;
        auto ty = ret.type.type;
        auto lty = lhs.type.type;
        auto rty = rhs.type.type;
        if (ty == sem::Type::INT) {
            ret.val_i = lhs.val_i - rhs.val_i;
        } else {
            ret.val_r = 0;
            if (lty == sem::Type::REAL) {
                ret.val_r += lhs.val_r;
            } else {
                ret.val_r += lhs.val_i;
            }
            if (rty == sem::Type::REAL) {
                ret.val_r -= rhs.val_r;
            } else {
                ret.val_r -= rhs.val_i;
            }
        }
    } else {
        ret.value = DoSub(ret.type, Cast(ret.type, lhs.type, lhs.Value()),
            Cast(ret.type, rhs.type, rhs.Value()));
    }
    return ret;
}

ExValue DoMul(const ExValue &lhs, const ExValue &rhs) {
    ExValue ret;
    ret.type = sem::DoMul(lhs.type, rhs.type);

    if (lhs.is_const && rhs.is_const) {
        ret.is_const = true;
        auto ty = ret.type.type;
        auto lty = lhs.type.type;
        auto rty = rhs.type.type;
        if (ty == sem::Type::INT) {
            ret.val_i = lhs.val_i * rhs.val_i;
        } else {
            ret.val_r = 1;
            if (lty == sem::Type::REAL) {
                ret.val_r *= lhs.val_r;
            } else {
                ret.val_r *= lhs.val_i;
            }
            if (rty == sem::Type::REAL) {
                ret.val_r *= rhs.val_r;
            } else {
                ret.val_r *= rhs.val_i;
            }
        }
    } else {
        ret.value = DoMul(ret.type, Cast(ret.type, lhs.type, lhs.Value()),
            Cast(ret.type, rhs.type, rhs.Value()));
    }
    return ret;
}

ExValue DoDiv(const ExValue &lhs, const ExValue &rhs) {
    ExValue ret;
    ret.type = sem::DoDiv(lhs.type, rhs.type);

    if (lhs.is_const && rhs.is_const) {
        ret.is_const = true;
        auto ty = ret.type.type;
        auto lty = lhs.type.type;
        auto rty = rhs.type.type;
        if (ty == sem::Type::INT) {
            if (rhs.val_i == 0) {
                ret.is_const = false;
                ret.value = DoDiv(ret.type, lhs.Value(), rhs.Value());
            } else {
                ret.val_i = lhs.val_i / rhs.val_i;
            }
        } else {
            ret.val_r = 1;
            if (lty == sem::Type::REAL) {
                ret.val_r *= lhs.val_r;
            } else {
                ret.val_r *= lhs.val_i;
            }
            if (rty == sem::Type::REAL) {
                ret.val_r /= rhs.val_r;
            } else {
                ret.val_r /= rhs.val_i;
            }
        }
    } else {
        ret.value = DoDiv(ret.type, Cast(ret.type, lhs.type, lhs.Value()),
            Cast(ret.type, rhs.type, rhs.Value()));
    }
    return ret;
}

ExValue DoMod(const ExValue &lhs, const ExValue &rhs) {
    ExValue ret;
    ret.type = sem::DoMod(lhs.type, rhs.type);

    if (lhs.is_const && rhs.is_const && rhs.val_i != 0) {
        ret.is_const = true;
        ret.val_i = lhs.val_i % rhs.val_i;
    } else {
        ret.value = DoMod(ret.type, Cast(ret.type, lhs.type, lhs.Value()),
            Cast(ret.type, rhs.type, rhs.Value()));
    }
    return ret;
}

ExValue DoNeg(const ExValue &val) {
    ExValue ret;
    ret.type = sem::DoNeg(val.type);

    if (val.is_const) {
        ret.is_const = true;
        auto ty = ret.type.type;
        if (ty == sem::Type::INT) {
            ret.val_i = -val.val_i;
        } else {
            ret.val_r = -val.val_r;
        }
    } else {
        ret.value = DoNeg(ret.type, val.Value());
    }
    return ret;
}

ExValue DoAnd(const ExValue &lhs, const ExValue &rhs) {
    ExValue ret;
    ret.type = sem::DoAnd(lhs.type, rhs.type);

    if (lhs.is_const && rhs.is_const) {
        ret.is_const = true;
        auto ty = ret.type.type;
        if (ty == sem::Type::INT) {
            ret.val_i = lhs.val_i & rhs.val_i;
        } else {
            ret.val_b = lhs.val_b && rhs.val_b;
        }
    } else {
        ret.value = DoAnd(ret.type, Cast(ret.type, lhs.type, lhs.Value()),
            Cast(ret.type, rhs.type, rhs.Value()));
    }
    return ret;
}

ExValue DoOr(const ExValue &lhs, const ExValue &rhs) {
    ExValue ret;
    ret.type = sem::DoOr(lhs.type, rhs.type);

    if (lhs.is_const && rhs.is_const) {
        ret.is_const = true;
        auto ty = ret.type.type;
        if (ty == sem::Type::INT) {
            ret.val_i = lhs.val_i | rhs.val_i;
        } else {
            ret.val_b = lhs.val_b || rhs.val_b;
        }
    } else {
        ret.value = DoOr(ret.type, Cast(ret.type, lhs.type, lhs.Value()),
            Cast(ret.type, rhs.type, rhs.Value()));
    }
    return ret;
}

ExValue DoXor(const ExValue &lhs, const ExValue &rhs) {
    ExValue ret;
    ret.type = sem::DoXor(lhs.type, rhs.type);

    if (lhs.is_const && rhs.is_const) {
        ret.is_const = true;
        ret.val_i = lhs.val_i ^ rhs.val_i;
    } else {
        ret.value = DoXor(ret.type, Cast(ret.type, lhs.type, lhs.Value()),
            Cast(ret.type, rhs.type, rhs.Value()));
    }
    return ret;
}

ExValue DoNot(const ExValue &val) {
    ExValue ret;
    ret.type = sem::DoNot(val.type);

    if (val.is_const) {
        ret.is_const = true;
        auto ty = ret.type.type;
        if (ty == sem::Type::INT) {
            ret.val_i = ~val.val_i;
        } else {
            ret.val_b = !val.val_b;
        }
    } else {
        ret.value = DoNot(ret.type, val.Value());
    }
    return ret;
}

static sem::Type GetCmpType(const sem::Type &lhs, const sem::Type &rhs) {
    if (lhs.type == sem::Type::INT) {
        if (rhs.type == sem::Type::INT) {
            return sem::Type::Int();
        } else {
            return sem::Type::Real();
        }
    }
    return lhs;
}

ExValue CmpEqual(const ExValue &lhs, const ExValue &rhs) {
    ExValue ret;
    ret.type = sem::Type::Bool();
    sem::Type mid_ty = GetCmpType(lhs.type, rhs.type);

    if (lhs.is_const && rhs.is_const) {
        ret.is_const = true;
        auto lty = lhs.type.type;
        auto rty = rhs.type.type;
        if (lty == sem::Type::INT && rty == sem::Type::INT) {
            ret.val_b = (lhs.val_i == rhs.val_i);
        } else if (lty == sem::Type::ENUM && rty == sem::Type::ENUM) {
            ret.val_b = (lhs.val_i == rhs.val_i);
        } else if (lty == sem::Type::INT && rty == sem::Type::REAL) {
            ret.val_b = (lhs.val_i == rhs.val_r);
        } else if (lty == sem::Type::REAL && rty == sem::Type::INT) {
            ret.val_b = (lhs.val_r == rhs.val_i);
        } else if (lty == sem::Type::REAL && rty == sem::Type::REAL) {
            ret.val_b = (lhs.val_r == rhs.val_r);
        } else if (lty == sem::Type::BOOL && rty == sem::Type::BOOL) {
            ret.val_b = (lhs.val_b == rhs.val_b);
        } else if (lty == sem::Type::CHAR && rty == sem::Type::CHAR) {
            ret.val_b = (lhs.val_c == rhs.val_c);
        }
    } else {
        ret.value = CmpEqual(ret.type, Cast(mid_ty, lhs.type, lhs.Value()),
            Cast(mid_ty, rhs.type, rhs.Value()));
    }
    return ret;
}

ExValue CmpNEqual(const ExValue &lhs, const ExValue &rhs) {
    ExValue ret;
    ret.type = sem::Type::Bool();
    sem::Type mid_ty = GetCmpType(lhs.type, rhs.type);

    if (lhs.is_const && rhs.is_const) {
        ret.is_const = true;
        auto lty = lhs.type.type;
        auto rty = rhs.type.type;
        if (lty == sem::Type::INT && rty == sem::Type::INT) {
            ret.val_b = (lhs.val_i != rhs.val_i);
        } else if (lty == sem::Type::ENUM && rty == sem::Type::ENUM) {
            ret.val_b = (lhs.val_i != rhs.val_i);
        } else if (lty == sem::Type::INT && rty == sem::Type::REAL) {
            ret.val_b = (lhs.val_i != rhs.val_r);
        } else if (lty == sem::Type::REAL && rty == sem::Type::INT) {
            ret.val_b = (lhs.val_r != rhs.val_i);
        } else if (lty == sem::Type::REAL && rty == sem::Type::REAL) {
            ret.val_b = (lhs.val_r != rhs.val_r);
        } else if (lty == sem::Type::BOOL && rty == sem::Type::BOOL) {
            ret.val_b = (lhs.val_b != rhs.val_b);
        } else if (lty == sem::Type::CHAR && rty == sem::Type::CHAR) {
            ret.val_b = (lhs.val_c != rhs.val_c);
        }
    } else {
        ret.value = CmpNEqual(ret.type, Cast(mid_ty, lhs.type, lhs.Value()),
            Cast(mid_ty, rhs.type, rhs.Value()));
    }
    return ret;
}

ExValue CmpLess(const ExValue &lhs, const ExValue &rhs) {
    ExValue ret;
    ret.type = sem::Type::Bool();
    sem::Type mid_ty = GetCmpType(lhs.type, rhs.type);

    if (lhs.is_const && rhs.is_const) {
        ret.is_const = true;
        auto lty = lhs.type.type;
        auto rty = rhs.type.type;
        if (lty == sem::Type::INT && rty == sem::Type::INT) {
            ret.val_b = (lhs.val_i < rhs.val_i);
        } else if (lty == sem::Type::ENUM && rty == sem::Type::ENUM) {
            ret.val_b = (lhs.val_i < rhs.val_i);
        } else if (lty == sem::Type::INT && rty == sem::Type::REAL) {
            ret.val_b = (lhs.val_i < rhs.val_r);
        } else if (lty == sem::Type::REAL && rty == sem::Type::INT) {
            ret.val_b = (lhs.val_r < rhs.val_i);
        } else if (lty == sem::Type::REAL && rty == sem::Type::REAL) {
            ret.val_b = (lhs.val_r < rhs.val_r);
        } else if (lty == sem::Type::BOOL && rty == sem::Type::BOOL) {
            ret.val_b = (lhs.val_b < rhs.val_b);
        } else if (lty == sem::Type::CHAR && rty == sem::Type::CHAR) {
            ret.val_b = (lhs.val_c < rhs.val_c);
        }
    } else {
        ret.value = CmpLess(ret.type, Cast(mid_ty, lhs.type, lhs.Value()),
            Cast(mid_ty, rhs.type, rhs.Value()));
    }
    return ret;
}

ExValue CmpLessEq(const ExValue &lhs, const ExValue &rhs) {
    ExValue ret;
    ret.type = sem::Type::Bool();
    sem::Type mid_ty = GetCmpType(lhs.type, rhs.type);

    if (lhs.is_const && rhs.is_const) {
        ret.is_const = true;
        auto lty = lhs.type.type;
        auto rty = rhs.type.type;
        if (lty == sem::Type::INT && rty == sem::Type::INT) {
            ret.val_b = (lhs.val_i <= rhs.val_i);
        } else if (lty == sem::Type::ENUM && rty == sem::Type::ENUM) {
            ret.val_b = (lhs.val_i <= rhs.val_i);
        } else if (lty == sem::Type::INT && rty == sem::Type::REAL) {
            ret.val_b = (lhs.val_i <= rhs.val_r);
        } else if (lty == sem::Type::REAL && rty == sem::Type::INT) {
            ret.val_b = (lhs.val_r <= rhs.val_i);
        } else if (lty == sem::Type::REAL && rty == sem::Type::REAL) {
            ret.val_b = (lhs.val_r <= rhs.val_r);
        } else if (lty == sem::Type::BOOL && rty == sem::Type::BOOL) {
            ret.val_b = (lhs.val_b <= rhs.val_b);
        } else if (lty == sem::Type::CHAR && rty == sem::Type::CHAR) {
            ret.val_b = (lhs.val_c <= rhs.val_c);
        }
    } else {
        ret.value = CmpLessEq(ret.type, Cast(mid_ty, lhs.type, lhs.Value()),
            Cast(mid_ty, rhs.type, rhs.Value()));
    }
    return ret;
}

ExValue CmpGreat(const ExValue &lhs, const ExValue &rhs) {
    ExValue ret;
    ret.type = sem::Type::Bool();
    sem::Type mid_ty = GetCmpType(lhs.type, rhs.type);

    if (lhs.is_const && rhs.is_const) {
        ret.is_const = true;
        auto lty = lhs.type.type;
        auto rty = rhs.type.type;
        if (lty == sem::Type::INT && rty == sem::Type::INT) {
            ret.val_b = (lhs.val_i > rhs.val_i);
        } else if (lty == sem::Type::ENUM && rty == sem::Type::ENUM) {
            ret.val_b = (lhs.val_i > rhs.val_i);
        } else if (lty == sem::Type::INT && rty == sem::Type::REAL) {
            ret.val_b = (lhs.val_i > rhs.val_r);
        } else if (lty == sem::Type::REAL && rty == sem::Type::INT) {
            ret.val_b = (lhs.val_r > rhs.val_i);
        } else if (lty == sem::Type::REAL && rty == sem::Type::REAL) {
            ret.val_b = (lhs.val_r > rhs.val_r);
        } else if (lty == sem::Type::BOOL && rty == sem::Type::BOOL) {
            ret.val_b = (lhs.val_b > rhs.val_b);
        } else if (lty == sem::Type::CHAR && rty == sem::Type::CHAR) {
            ret.val_b = (lhs.val_c > rhs.val_c);
        }
    } else {
        ret.value = CmpGreat(ret.type, Cast(mid_ty, lhs.type, lhs.Value()),
            Cast(mid_ty, rhs.type, rhs.Value()));
    }
    return ret;
}

ExValue CmpGreatEq(const ExValue &lhs, const ExValue &rhs) {
    ExValue ret;
    ret.type = sem::Type::Bool();
    sem::Type mid_ty = GetCmpType(lhs.type, rhs.type);

    if (lhs.is_const && rhs.is_const) {
        ret.is_const = true;
        auto lty = lhs.type.type;
        auto rty = rhs.type.type;
        if (lty == sem::Type::INT && rty == sem::Type::INT) {
            ret.val_b = (lhs.val_i >= rhs.val_i);
        } else if (lty == sem::Type::ENUM && rty == sem::Type::ENUM) {
            ret.val_b = (lhs.val_i >= rhs.val_i);
        } else if (lty == sem::Type::INT && rty == sem::Type::REAL) {
            ret.val_b = (lhs.val_i >= rhs.val_r);
        } else if (lty == sem::Type::REAL && rty == sem::Type::INT) {
            ret.val_b = (lhs.val_r >= rhs.val_i);
        } else if (lty == sem::Type::REAL && rty == sem::Type::REAL) {
            ret.val_b = (lhs.val_r >= rhs.val_r);
        } else if (lty == sem::Type::BOOL && rty == sem::Type::BOOL) {
            ret.val_b = (lhs.val_b >= rhs.val_b);
        } else if (lty == sem::Type::CHAR && rty == sem::Type::CHAR) {
            ret.val_b = (lhs.val_c >= rhs.val_c);
        }
    } else {
        ret.value = CmpGreatEq(ret.type, Cast(mid_ty, lhs.type, lhs.Value()),
            Cast(mid_ty, rhs.type, rhs.Value()));
    }
    return ret;
}

ExValue DoPred(const ExValue &val) {
    ExValue ret;
    ret.type = val.type;

    if (val.is_const) {
        ret.is_const = true;
        ret.val_i = val.val_i - 1;
    } else {
        ret.value = DoPred(ret.type, val.Value());
    }
    return ret;
}

ExValue DoSucc(const ExValue &val) {
    ExValue ret;
    ret.type = val.type;

    if (val.is_const) {
        ret.is_const = true;
        ret.val_i = val.val_i + 1;
    } else {
        ret.value = DoSucc(ret.type, val.Value());
    }
    return ret;
}

ExValue DoSqr(const ExValue &val) {
    ExValue ret;
    ret.type = val.type;

    if (val.is_const) {
        ret.is_const = true;
        auto ty = ret.type.type;
        if (ty == sem::Type::INT) {
            ret.val_i = val.val_i * val.val_i;
        } else {
            ret.val_r = val.val_r * val.val_r;
        }
    } else {
        ret.value = DoSqr(ret.type, val.Value());
    }
    return ret;
}

ExValue DoSqrt(const ExValue &val) {
    ExValue ret;
    ret.type = sem::Type::Real();

    if (val.is_const) {
        ret.is_const = true;
        ret.val_r = std::sqrt(val.val_r);
    } else {
        ret.value = DoSqrt(ret.type, Cast(ret.type, val).Value());
    }
    return ret;
}

ExValue DoAbs(const ExValue &val) {
    ExValue ret;
    ret.type = val.type;

    if (val.is_const) {
        ret.is_const = true;
        auto ty = ret.type.type;
        if (ty == sem::Type::INT) {
            ret.val_i = std::abs(val.val_i);
        } else {
            ret.val_r = std::abs(val.val_r);
        }
    } else {
        ret.value = DoAbs(ret.type, val.Value());
    }
    return ret;
}

ExValue DoChr(const ExValue &val) {
    ExValue ret;
    ret.type = sem::Type::Char();

    if (val.is_const) {
        ret.is_const = true;
        ret.val_c = char(val.val_i);
    } else {
        ret.value = DoChr(ret.type, val.Value());
    }
    return ret;
}

ExValue DoOrd(const ExValue &val) {
    ExValue ret;
    ret.type = sem::Type::Int();

    if (val.is_const) {
        ret.is_const = true;
        ret.val_i = val.val_i;
    } else {
        ret.value = DoOrd(ret.type, Cast(ret.type, val).Value());
    }
    return ret;
}

ExValue DoOdd(const ExValue &val) {
    ExValue ret;
    ret.type = sem::Type::Bool();

    if (val.is_const) {
        ret.is_const = true;
        ret.val_b = val.val_i & 1;
    } else {
        ret.value = DoOdd(ret.type, val.Value());
    }
    return ret;
}

}