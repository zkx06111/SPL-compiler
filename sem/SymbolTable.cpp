#include "SymbolTable.h"

#include "TypeUtil.h"

namespace sem {

SymbolTable::SymbolTable() {
    id_t.emplace_back();
    type_t.emplace_back(0);
    var_t.emplace_back();
    const_t.emplace_back();
    func_t.emplace_back();

    NameType("integer", Type::Int());
    NameType("real", Type::Real());
    NameType("boolean", Type::Bool());
    NameType("char", Type::Char());
    NameType("string", Type::String());

    // TODO - add sys-func
}

void SymbolTable::NewScope() {
    id_t.emplace_back();
    type_t.emplace_back(type_t.size());
    var_t.emplace_back();
    const_t.emplace_back();
    func_t.emplace_back();
}

void SymbolTable::EndScope() {
    id_t.pop_back();
    type_t.pop_back();
    var_t.pop_back();
    const_t.pop_back();
    func_t.pop_back();
}

bool SymbolTable::CheckId(const std::string &name) const {
    return id_t.back().Check(name);
}

bool SymbolTable::CheckType(const std::string &name) const {
    for (auto it = type_t.rbegin(); it != type_t.rend(); it++) {
        if (it->CheckType(name)) {
            return true;
        }
    }
    return false;
}

bool SymbolTable::CheckEnum(const std::string &name) const {
    for (auto it = const_t.rbegin(); it != const_t.rend(); it++) {
        if (it->CheckEnum(name)) {
            return true;
        }
    }
    return false;
}

bool SymbolTable::CheckVar(const std::string &name) const {
    for (auto it = var_t.rbegin(); it != var_t.rend(); it++) {
        if (it->CheckVar(name)) {
            return true;
        }
    }
    return false;
}

bool SymbolTable::CheckConst(const std::string &name) const {
    for (auto it = const_t.rbegin(); it != const_t.rend(); it++) {
        if (it->CheckConst(name)) {
            return true;
        }
    }
    return false;
}

bool SymbolTable::CheckFunc(const std::string &name) const {
    for (auto it = func_t.rbegin(); it != func_t.rend(); it++) {
        if (it->CheckFunc(name)) {
            return true;
        }
    }
    return false;
}

Type SymbolTable::GetType(const std::string &name) const {
    for (auto it = type_t.rbegin(); it != type_t.rend(); it++) {
        if (it->CheckType(name)) {
            return it->GetType(name);
        }
    }
    return Type::Void();
}

Type SymbolTable::GetEnumType(const std::string &name) const {
    for (auto it = const_t.rbegin(); it != const_t.rend(); it++) {
        if (it->CheckEnum(name)) {
            return it->GetConstType(name);
        }
    }
    return Type::Void();
}

Type SymbolTable::GetVarType(const std::string &name) const {
    for (auto it = var_t.rbegin(); it != var_t.rend(); it++) {
        if (it->CheckVar(name)) {
            return it->GetVarType(name);
        }
    }
    return Type::Void();
}

Type SymbolTable::GetConstType(const std::string &name) const {
    for (auto it = const_t.rbegin(); it != const_t.rend(); it++) {
        if (it->CheckConst(name)) {
            return it->GetConstType(name);
        }
    }
    return Type::Void();
}

Func SymbolTable::GetFunc(const std::string &name) const {
    for (auto it = func_t.rbegin(); it != func_t.rend(); it++) {
        if (it->CheckFunc(name)) {
            return it->GetFunc(name);
        }
    }
    return Func { Type::Void(), { Type::Void() } };
}

int SymbolTable::GetConstValI(const std::string &name) const {
    for (auto it = const_t.rbegin(); it != const_t.rend(); it++) {
        if (it->CheckConst(name)) {
            return it->GetConstValI(name);
        }
    }
    return 0;
}

double SymbolTable::GetConstValR(const std::string &name) const {
    for (auto it = const_t.rbegin(); it != const_t.rend(); it++) {
        if (it->CheckConst(name)) {
            return it->GetConstValR(name);
        }
    }
    return 0;
}

char SymbolTable::GetConstValC(const std::string &name) const {
    for (auto it = const_t.rbegin(); it != const_t.rend(); it++) {
        if (it->CheckConst(name)) {
            return it->GetConstValC(name);
        }
    }
    return 0;
}

bool SymbolTable::GetConstValB(const std::string &name) const {
    for (auto it = const_t.rbegin(); it != const_t.rend(); it++) {
        if (it->CheckConst(name)) {
            return it->GetConstValB(name);
        }
    }
    return false;
}

std::string SymbolTable::GetConstValS(const std::string &name) const {
    for (auto it = const_t.rbegin(); it != const_t.rend(); it++) {
        if (it->CheckConst(name)) {
            return it->GetConstValS(name);
        }
    }
    return "";
}

Enum SymbolTable::GetEnum(const Type &type) const {
    return type_t[type.depth].GetEnum(type.ind);
}
Subrange SymbolTable::GetSubrange(const Type &type) const {
    return type_t[type.depth].GetSubrange(type.ind);
}
Array SymbolTable::GetArray(const Type &type) const {
    return type_t[type.depth].GetArray(type.ind);
}
Record SymbolTable::GetRecord(const Type &type) const {
    return type_t[type.depth].GetRecord(type.ind);
}

Result<Type> SymbolTable::NewEnum(const std::vector<std::string> &ids) {
    std::unordered_map<std::string, int> ele;
    int val = 0;
    for (const auto &id : ids) {
        ele[id] = val;
        ++val;
    }

    Enum enm(ele);
    Type type = type_t.back().AddEnum(enm);

    for (const auto &[name, _] : ele) {
        NewConst(name, type, enm);
    }
    return { type, "" };
}

Result<Type> SymbolTable::NewIntSubrange(int l, int r) {
    if (l > r) {
        return { Type::Void(), "lower bound is greater than upper bound" };
    }
    Type type = type_t.back().AddSubrange(Subrange { Type::Int(), l, r });
    return { type, "" };
}

Result<Type> SymbolTable::NewCharSubrange(char l, char r) {
    if (l > r) {
        return { Type::Void(), "lower bound is greater than upper bound" };
    }
    Type type = type_t.back().AddSubrange(Subrange { Type::Char(), l, r });
    return { type, "" };
}

Result<Type> SymbolTable::NewBoolSubrange(bool l, bool r) {
    int lv = l, rv = r;
    if (lv > rv) {
        return { Type::Void(), "lower bound is greater than upper bound" };
    }
    Type type = type_t.back().AddSubrange(Subrange { Type::Bool(), lv, rv });
    return { type, "" };
}

Result<Type>
SymbolTable::NewEnumSubrange(const std::string &l, const std::string &r) {
    if (!CheckEnum(l)) {
        return { Type::Void(), "'" + l + "' is not an enum element" };
    }
    if (!CheckEnum(r)) {
        return { Type::Void(), "'" + r + "' is not an enum element" };
    }
    auto le = GetEnumType(l), re = GetEnumType(r);
    if (le != re) {
        return { Type::Void(),
            "'" + l + "' and '" + r + "' are not in the same enum type"};
    }
    int lv = GetConstValI(l);
    int rv = GetConstValI(r);
    Type type = type_t.back().AddSubrange(Subrange(le, lv, rv));
    return { type, "" };
}

Result<Type> SymbolTable::NewArray(const Type &ind_type, const Type &ele_type) {
    if (!CanBeArrayIndType(ind_type)) {
        return { Type::Void(), "not a valid subscript type" };
    }
    if (IsVoid(ele_type)) {
        return { Type::Void(), "unknown element type" };
    }
    Type type = type_t.back().AddArray(Array(ind_type, ele_type));
    return { type, "" };
}

Result<Type>
SymbolTable::NewRecord(const std::vector<std::pair<std::string, Type>> &data) {
    std::unordered_set<std::string> names;
    for (const auto &[name, type] : data) {
        if (names.count(name) != 0) {
            return { Type::Void(),
                "duplicated identifier '" + name + "' in record definition" };
        } else {
            names.insert(name);
        }

        if (IsVoid(type)) {
            return { Type::Void(), "'" + name + "' has an unknown type" };
        }
    }
    Type type = type_t.back().AddRecord(Record(data));
    return { type, "" };
}

std::string SymbolTable::NameType(const std::string &name, const Type &type) {
    if (CheckId(name)) {
        return "duplicated identifier '" + name + "' in this scope";
    }
    id_t.back().Insert(name);
    if (IsVoid(type)) {
        return "'" + name + "' refers to an unknown type";
    }

    type_t.back().NameType(name, type);
    return "";
}

std::string SymbolTable::NewVariable(const std::string &name,
        const Type &type) {
    if (CheckId(name)) {
        return "duplicated identifier '" + name + "' in this scope";
    }
    id_t.back().Insert(name);
    if (IsVoid(type)) {
        return "'" + name + "' has an unknown type";
    }

    var_t.back().AddVar(name, type);
    return "";
}

std::string SymbolTable::NewConst(const std::string &name, int val) {
    if (CheckId(name)) {
        return "duplicated identifier '" + name + "' in this scope";
    }

    id_t.back().Insert(name);
    const_t.back().AddConst(name, val);
    return "";
}

std::string SymbolTable::NewConst(const std::string &name, double val) {
    if (CheckId(name)) {
        return "duplicated identifier '" + name + "' in this scope";
    }

    id_t.back().Insert(name);
    const_t.back().AddConst(name, val);
    return "";
}

std::string SymbolTable::NewConst(const std::string &name, bool val) {
    if (CheckId(name)) {
        return "duplicated identifier '" + name + "' in this scope";
    }

    id_t.back().Insert(name);
    const_t.back().AddConst(name, val);
    return "";
}

std::string SymbolTable::NewConst(const std::string &name, char val) {
    if (CheckId(name)) {
        return "duplicated identifier '" + name + "' in this scope";
    }

    id_t.back().Insert(name);
    const_t.back().AddConst(name, val);
    return "";
}

std::string
SymbolTable::NewConst(const std::string &name, const std::string &val) {
    if (CheckId(name)) {
        return "duplicated identifier '" + name + "' in this scope";
    }

    id_t.back().Insert(name);
    const_t.back().AddConst(name, val);
    return "";
}

std::string SymbolTable::NewConst(const std::string &name, const Type &type,
        const Enum &enm) {
    if (CheckId(name)) {
        return "duplicated identifier '" + name + "' in this scope";
    }

    id_t.back().Insert(name);
    const_t.back().AddConst(name, type, enm);
    return "";
}

std::string SymbolTable::NewProc(const std::string &name,
        const std::vector<std::pair<std::string, Type>> &args) {
    if (CheckId(name)) {
        return "duplicated identifier '" + name + "' in this scope";
    }
    id_t.back().Insert(name);

    std::unordered_set<std::string> names;
    for (const auto &[arg_name, arg_type] : args) {
        if (names.count(arg_name) != 0) {
            return "duplicated identifier '" + arg_name +
                "' in procedure definition";
        } else {
            names.insert(arg_name);
        }

        if (IsVoid(arg_type)) {
            return "'" + arg_name + "' has an unknown type";
        }
    }

    Func func;
    func.ret = Type::Void();
    for (const auto &[_, arg_type] : args) {
        func.args.push_back(arg_type);
    }
    func_t.back().AddFunc(name, func);

    NewScope();
    for (const auto &[arg_name, arg_type] : args) {
        NewVariable(arg_name, arg_type);
    }

    return "";
}

std::string SymbolTable::NewFunc(const std::string &name, const Type &ret,
        const std::vector<std::pair<std::string, Type>> &args) {
    if (CheckId(name)) {
        return "duplicated identifier '" + name + "' in this scope";
    }
    id_t.back().Insert(name);
    if (IsVoid(ret)) {
        return "return type of '" + name + "' is an unknown type";
    }

    std::unordered_set<std::string> names;
    for (const auto &[arg_name, arg_type] : args) {
        if (names.count(arg_name) != 0 || name == arg_name) {
            return "duplicated identifier '" + arg_name +
                "' in function definition";
        } else {
            names.insert(arg_name);
        }

        if (IsVoid(arg_type)) {
            return "'" + arg_name + "' has an unknown type";
        }
    }

    Func func;
    func.ret = ret;
    for (const auto &[_, arg_type] : args) {
        func.args.push_back(arg_type);
    }
    func_t.back().AddFunc(name, func);

    NewScope();
    NewVariable(name, ret);
    for (const auto &[arg_name, arg_type] : args) {
        NewVariable(arg_name, arg_type);
    }

    return "";
}

SymbolTable sym_t;

}