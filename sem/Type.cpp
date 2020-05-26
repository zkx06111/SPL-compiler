#include "Type.h"

#include "TypeUtil.h"

namespace sem {

bool Type::operator==(const Type &rhs) const {
    return type == rhs.type && ind == rhs.ind && depth == rhs.depth;
}

bool Type::operator!=(const Type &rhs) const {
    return !(*this == rhs);
}

Type Type::Void() {
    return Type { Type::VOID, 0, 0 };
}
Type Type::Int() {
    return Type { Type::INT, 0, 0 };
}
Type Type::Real() {
    return Type { Type::REAL, 0, 0 };
}
Type Type::Char() {
    return Type { Type::CHAR, 0, 0 };
}
Type Type::Bool() {
    return Type { Type::BOOL, 0, 0 };
}
Type Type::String() {
    return Type { Type::STRING, 0, 0 };
}

bool Enum::Contains(const std::string &name) const {
    return ele.count(name) != 0;
}

int Enum::GetEnumVal(const std::string &name) const {
    if (!Contains(name)) {
        throw SemError("'" + name + "' is not a member of this enum");
    }
    return ele.at(name);
}

bool Subrange::Contains(int val) const {
    return val >= l && val <= r;
}

Type Subrange::GetType() const {
    return type;
}

Type Array::GetIndexType() const {
    return ind_type;
}

Type Array::GetElementType() const {
    return ele_type;
}

bool Array::CanTakeSub(const Type &type) const {
    return IsAlmostSame(type, ind_type);
}

void Array::TakeSub(const Type &type) const {
    if (!CanTakeSub(type)) {
        throw SemError("invalid subscript type");
    }
}

int Array::GetSize() const {
    if (ind_type.type == Type::SUBRANGE) {
        Subrange rng = sym_t.GetSubrange(ind_type);
        return rng.r - rng.l + 1;
    } else if (ind_type.type == Type::ENUM) {
        Enum enm = sym_t.GetEnum(ind_type);
        return enm.ele.size();
    } else {
        return 2;
    }
}

int Array::GetBase() const {
    if (ind_type.type == Type::SUBRANGE) {
        Subrange rng = sym_t.GetSubrange(ind_type);
        return rng.l;
    } else {
        return 0;
    }
}

Record::Record(const std::vector<std::pair<std::string, Type>> &data) {
    this->data = data;
    int ind = 0;
    for (const auto &[name, type] : data) {
        name2ind[name] = ind;
        ++ind;
    }
}

bool Record::Contains(const std::string &name) const {
    return name2ind.count(name) != 0;
}

Type Record::GetDataType(const std::string &name) const {
    if (!Contains(name)) {
        throw SemError("'" + name + "' is not a member of this record");
    }
    return data[name2ind.at(name)].second;
}

}