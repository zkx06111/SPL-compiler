#include "ConstTable.h"

namespace sem {

Const::Const() {
    type = Type::Int();
    val_i = 0;
}

Const::~Const() {
    if (type.type == Type::STRING) {
        val_s.~basic_string();
    }
}

Const::Const(const Const &rhs) {
    type = rhs.type;
    if (type.type == Type::INT || type.type == Type::ENUM) {
        val_i = rhs.val_i;
    } else if (type.type == Type::REAL) {
        val_r = rhs.val_r;
    } else if (type.type == Type::CHAR) {
        val_c = rhs.val_c;
    } else if (type.type == Type::BOOL) {
        val_b = rhs.val_b;
    } else {
        val_s = rhs.val_s;
    }
}
Const &Const::operator=(const Const &rhs) {
    type = rhs.type;
    if (type.type == Type::INT || type.type == Type::ENUM) {
        val_i = rhs.val_i;
    } else if (type.type == Type::REAL) {
        val_r = rhs.val_r;
    } else if (type.type == Type::CHAR) {
        val_c = rhs.val_c;
    } else if (type.type == Type::BOOL) {
        val_b = rhs.val_b;
    } else {
        val_s = rhs.val_s;
    }
    return *this;
}

void ConstTable::AddConst(const std::string &name, int val) {
    Const c;
    c.type = Type::Int();
    c.val_i = val;
    consts[name] = c;
}

void ConstTable::AddConst(const std::string &name, double val) {
    Const c;
    c.type = Type::Real();
    c.val_r = val;
    consts[name] = c;
}

void ConstTable::AddConst(const std::string &name, bool val) {
    Const c;
    c.type = Type::Bool();
    c.val_b = val;
    consts[name] = c;
}

void ConstTable::AddConst(const std::string &name, char val) {
    Const c;
    c.type = Type::Char();
    c.val_c = val;
    consts[name] = c;
}

void ConstTable::AddConst(const std::string &name, const std::string &val) {
    Const c;
    c.type = Type::String();
    c.val_s = val;
    consts[name] = c;
}

void ConstTable::AddConst(const std::string &name, const Type &type,
        const Enum &enm) {
    Const c;
    c.type = type;
    c.val_i = enm.GetEnumVal(name);
    consts[name] = c;
}

bool ConstTable::CheckConst(const std::string &name) const {
    return consts.count(name) != 0;
}

bool ConstTable::CheckEnum(const std::string &name) const {
    if (consts.count(name) == 0) {
        return false;
    }
    Const c = consts.at(name);
    return c.type.type == Type::ENUM;
}

Type ConstTable::GetConstType(const std::string &name) const {
    return consts.at(name).type;
}

int ConstTable::GetConstValI(const std::string &name) const {
    return consts.at(name).val_i;
}

double ConstTable::GetConstValR(const std::string &name) const {
    return consts.at(name).val_r;
}

bool ConstTable::GetConstValB(const std::string &name) const {
    return consts.at(name).val_b;
}

char ConstTable::GetConstValC(const std::string &name) const {
    return consts.at(name).val_c;
}

std::string ConstTable::GetConstValS(const std::string &name) const {
    return consts.at(name).val_s;
}

}