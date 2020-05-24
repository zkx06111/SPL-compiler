#include "SymbolTable.h"

#include "TypeUtil.h"

namespace sem {

SymbolTable::SymbolTable() {
    id_t.emplace_back();
    type_t.emplace_back(0);
    var_t.emplace_back();
    const_t.emplace_back();
    func_t.emplace_back();
    label_t.emplace_back();

    NameType("integer", Type::Int());
    NameType("real", Type::Real());
    NameType("boolean", Type::Bool());
    NameType("char", Type::Char());
    NameType("string", Type::String());

    id_t.back().Insert("abs");
    id_t.back().Insert("chr");
    id_t.back().Insert("odd");
    id_t.back().Insert("ord");
    id_t.back().Insert("pred");
    id_t.back().Insert("sqr");
    id_t.back().Insert("sqrt");
    id_t.back().Insert("succ");
    id_t.back().Insert("write");
    id_t.back().Insert("writeln");
}

void SymbolTable::NewScope() {
    id_t.emplace_back();
    type_t.emplace_back(type_t.size());
    var_t.emplace_back();
    const_t.emplace_back();
    func_t.emplace_back();
    label_t.emplace_back();
}

void SymbolTable::EndScope() {
    id_t.pop_back();
    type_t.pop_back();
    var_t.pop_back();
    const_t.pop_back();
    func_t.pop_back();
    label_t.pop_back();
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

bool SymbolTable::CheckLabel(int label) const {
    for (auto it = label_t.rbegin(); it != label_t.rend(); it++) {
        if (it->Check(label)) {
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
    throw SemError("no type named '" + name + "'");
}

Type SymbolTable::GetEnumType(const std::string &name) const {
    for (auto it = const_t.rbegin(); it != const_t.rend(); it++) {
        if (it->CheckEnum(name)) {
            return it->GetConstType(name);
        }
    }
    throw SemError("no enum string named '" + name + "'");
}

Type SymbolTable::GetVarType(const std::string &name) const {
    for (auto it = var_t.rbegin(); it != var_t.rend(); it++) {
        if (it->CheckVar(name)) {
            return it->GetVarType(name);
        }
    }
    throw SemError("no variable named '" + name + "'");
}

Type SymbolTable::GetConstType(const std::string &name) const {
    for (auto it = const_t.rbegin(); it != const_t.rend(); it++) {
        if (it->CheckConst(name)) {
            return it->GetConstType(name);
        }
    }
    throw SemError("no constant named '" + name + "'");
}

Func SymbolTable::GetFunc(const std::string &name) const {
    for (auto it = func_t.rbegin(); it != func_t.rend(); it++) {
        if (it->CheckFunc(name)) {
            return it->GetFunc(name);
        }
    }
    throw SemError("no function named '" + name + "'");
}

int SymbolTable::GetConstValI(const std::string &name) const {
    for (auto it = const_t.rbegin(); it != const_t.rend(); it++) {
        if (it->CheckConst(name)) {
            return it->GetConstValI(name);
        }
    }
    throw SemError("no constant named '" + name + "'");
}

double SymbolTable::GetConstValR(const std::string &name) const {
    for (auto it = const_t.rbegin(); it != const_t.rend(); it++) {
        if (it->CheckConst(name)) {
            return it->GetConstValR(name);
        }
    }
    throw SemError("no constant named '" + name + "'");
}

char SymbolTable::GetConstValC(const std::string &name) const {
    for (auto it = const_t.rbegin(); it != const_t.rend(); it++) {
        if (it->CheckConst(name)) {
            return it->GetConstValC(name);
        }
    }
    throw SemError("no constant named '" + name + "'");
}

bool SymbolTable::GetConstValB(const std::string &name) const {
    for (auto it = const_t.rbegin(); it != const_t.rend(); it++) {
        if (it->CheckConst(name)) {
            return it->GetConstValB(name);
        }
    }
    throw SemError("no constant named '" + name + "'");
}

std::string SymbolTable::GetConstValS(const std::string &name) const {
    for (auto it = const_t.rbegin(); it != const_t.rend(); it++) {
        if (it->CheckConst(name)) {
            return it->GetConstValS(name);
        }
    }
    throw SemError("no constant named '" + name + "'");
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

Type SymbolTable::NewEnum(const std::vector<std::string> &ids) {
    std::unordered_map<std::string, int> ele;
    int val = 0;
    for (const auto &id : ids) {
        if (ele.count(id) != 0) {
            throw SemError(
                "duplicated identifier '" + id + "' in record definition");
        }
        ele[id] = val;
        ++val;
    }

    Enum enm(ele);
    Type type = type_t.back().AddEnum(enm);

    for (const auto &[name, _] : ele) {
        NewConst(name, type, enm);
    }
    return type;
}

Type SymbolTable::NewIntSubrange(int l, int r) {
    if (l > r) {
        throw SemError("lower bound is greater than upper bound");
    }
    Type type = type_t.back().AddSubrange(Subrange { Type::Int(), l, r });
    return type;
}

Type SymbolTable::NewCharSubrange(char l, char r) {
    if (l > r) {
        throw SemError("lower bound is greater than upper bound");
    }
    Type type = type_t.back().AddSubrange(Subrange { Type::Char(), l, r });
    return type;
}

Type SymbolTable::NewBoolSubrange(bool l, bool r) {
    int lv = l, rv = r;
    if (lv > rv) {
        throw SemError("lower bound is greater than upper bound");
    }
    Type type = type_t.back().AddSubrange(Subrange { Type::Bool(), lv, rv });
    return type;
}

Type SymbolTable::NewEnumSubrange(const std::string &l, const std::string &r) {
    if (!CheckEnum(l)) {
        throw SemError("'" + l + "' is not an enum element");
    }
    if (!CheckEnum(r)) {
        throw SemError("'" + r + "' is not an enum element");
    }
    auto le = GetEnumType(l), re = GetEnumType(r);
    if (le != re) {
        throw SemError(
            "'" + l + "' and '" + r + "' are not in the same enum type");
    }
    int lv = GetConstValI(l);
    int rv = GetConstValI(r);
    if (lv > rv) {
        throw SemError("lower bound is greater than upper bound");
    }
    Type type = type_t.back().AddSubrange(Subrange(le, lv, rv));
    return type;
}

Type SymbolTable::NewArray(const Type &ind_type, const Type &ele_type) {
    if (!CanBeArrayIndType(ind_type)) {
        throw SemError("invalid subscript type");
    }
    if (IsVoid(ele_type)) {
        throw SemError("unknown element type");
    }
    Type type = type_t.back().AddArray(Array(ind_type, ele_type));
    return type;
}

Type
SymbolTable::NewRecord(const std::vector<std::pair<std::string, Type>> &data) {
    std::unordered_set<std::string> names;
    for (const auto &[name, type] : data) {
        if (names.count(name) != 0) {
            throw SemError(
                "duplicated identifier '" + name + "' in record definition");
        } else {
            names.insert(name);
        }

        if (IsVoid(type)) {
            throw SemError("'" + name + "' has an unknown type");
        }
    }
    Type type = type_t.back().AddRecord(Record(data));
    return type;
}

void SymbolTable::NameType(const std::string &name, const Type &type) {
    if (CheckId(name)) {
        throw SemError("duplicated identifier '" + name + "' in this scope");
    }
    id_t.back().Insert(name);
    if (IsVoid(type)) {
        throw SemError("'" + name + "' refers to an unknown type");
    }

    type_t.back().NameType(name, type);
}

void SymbolTable::NewVariable(const std::string &name,
        const Type &type) {
    if (CheckId(name)) {
        throw SemError("duplicated identifier '" + name + "' in this scope");
    }
    id_t.back().Insert(name);
    if (IsVoid(type)) {
        throw SemError("'" + name + "' has an unknown type");
    }

    var_t.back().AddVar(name, type);
}

void SymbolTable::NewConst(const std::string &name, int val) {
    if (CheckId(name)) {
        throw SemError("duplicated identifier '" + name + "' in this scope");
    }

    id_t.back().Insert(name);
    const_t.back().AddConst(name, val);
}

void SymbolTable::NewConst(const std::string &name, double val) {
    if (CheckId(name)) {
        throw SemError("duplicated identifier '" + name + "' in this scope");
    }

    id_t.back().Insert(name);
    const_t.back().AddConst(name, val);
}

void SymbolTable::NewConst(const std::string &name, bool val) {
    if (CheckId(name)) {
        throw SemError("duplicated identifier '" + name + "' in this scope");
    }

    id_t.back().Insert(name);
    const_t.back().AddConst(name, val);
}

void SymbolTable::NewConst(const std::string &name, char val) {
    if (CheckId(name)) {
        throw SemError("duplicated identifier '" + name + "' in this scope");
    }

    id_t.back().Insert(name);
    const_t.back().AddConst(name, val);
}

void
SymbolTable::NewConst(const std::string &name, const std::string &val) {
    if (CheckId(name)) {
        throw SemError("duplicated identifier '" + name + "' in this scope");
    }

    id_t.back().Insert(name);
    const_t.back().AddConst(name, val);
}

void SymbolTable::NewConst(const std::string &name, const Type &type,
        const Enum &enm) {
    if (CheckId(name)) {
        throw SemError("duplicated identifier '" + name + "' in this scope");
    }

    id_t.back().Insert(name);
    const_t.back().AddConst(name, type, enm);
}

void SymbolTable::NewProc(const std::string &name,
        const std::vector<std::pair<std::string, Type>> &args,
        const std::vector<int> mut_args) {
    if (CheckId(name)) {
        NewScope();
        throw SemError("duplicated identifier '" + name + "' in this scope");
    }
    id_t.back().Insert(name);

    std::unordered_set<std::string> names;
    for (const auto &[arg_name, arg_type] : args) {
        if (names.count(arg_name) != 0) {
            NewScope();
            throw SemError("duplicated identifier '" + arg_name +
                "' in procedure definition");
        } else {
            names.insert(arg_name);
        }

        if (IsVoid(arg_type)) {
            NewScope();
            throw SemError("'" + arg_name + "' has an unknown type");
        }
    }

    Func func;
    func.ret = Type::Void();
    func.mut_args = mut_args;
    for (const auto &[_, arg_type] : args) {
        func.args.push_back(arg_type);
    }
    func_t.back().AddFunc(name, func);

    NewScope();
    for (const auto &[arg_name, arg_type] : args) {
        NewVariable(arg_name, arg_type);
    }
}

void SymbolTable::NewFunc(const std::string &name, const Type &ret,
        const std::vector<std::pair<std::string, Type>> &args,
        const std::vector<int> mut_args) {
    if (CheckId(name)) {
        NewScope();
        throw SemError("duplicated identifier '" + name + "' in this scope");
    }
    id_t.back().Insert(name);
    if (IsVoid(ret)) {
        NewScope();
        throw SemError("return type of '" + name + "' is an unknown type");
    }

    std::unordered_set<std::string> names;
    for (const auto &[arg_name, arg_type] : args) {
        if (names.count(arg_name) != 0 || name == arg_name) {
            NewScope();
            throw SemError("duplicated identifier '" + arg_name +
                "' in function definition");
        } else {
            names.insert(arg_name);
        }

        if (IsVoid(arg_type)) {
            NewScope();
            throw SemError("'" + arg_name + "' has an unknown type");
        }
    }

    Func func;
    func.ret = ret;
    func.mut_args = mut_args;
    for (const auto &[_, arg_type] : args) {
        func.args.push_back(arg_type);
    }
    func_t.back().AddFunc(name, func);

    NewScope();
    NewVariable(name, ret);
    for (const auto &[arg_name, arg_type] : args) {
        NewVariable(arg_name, arg_type);
    }
}

void SymbolTable::NewLabel(int label) {
    if (label_t.back().Check(label)) {
        throw SemError("duplicated label declaration '" + std::to_string(label)
            + "'in this scope");
    }
    label_t.back().Insert(label);
}

void SymbolTable::NeedLabel(int label) const {
    if (!CheckLabel(label)) {
        throw SemError(
            "no label called '" + std::to_string(label) + "' is declared");
    }
}

SymbolTable sym_t;

}