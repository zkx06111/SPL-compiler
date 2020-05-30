#pragma once

#include <unordered_set>
#include <unordered_map>
#include <string>
#include <vector>

namespace sem {

struct Type {
    enum {
        VOID,
        INT,
        REAL,
        CHAR,
        BOOL,
        STRING,
        ENUM,
        SUBRANGE,
        ARRAY,
        RECORD
    } type = VOID;
    int ind = 0;
    int depth = 0;
    bool is_lhs = false;

    bool operator==(const Type &rhs) const;
    bool operator!=(const Type &rhs) const;

    static Type Void();
    static Type Int();
    static Type Real();
    static Type Char();
    static Type Bool();
    static Type String();
};

struct Enum {
    Enum(const std::unordered_map<std::string, int> &ele) : ele(ele) {}

    bool Contains(const std::string &name) const;
    int GetEnumVal(const std::string &name) const;

    std::unordered_map<std::string, int> ele;
};

struct Subrange {
    Subrange(const Type &type, int l, int r) : type(type), l(l), r(r) {}

    bool Contains(int val) const;
    Type GetType() const;

    Type type;
    int l, r;
};

struct Array {
    Array(const Type &ind_type, const Type &ele_type) :
        ind_type(ind_type), ele_type(ele_type) {}

    Type GetIndexType() const;
    Type GetElementType() const;
    bool CanTakeSub(const Type &type) const;
    void TakeSub(const Type &type) const;
    int GetSize() const;
    int GetBase() const;

    Type ind_type, ele_type;
};

struct Record {
    Record(const std::vector<std::pair<std::string, Type>> &data);

    bool Contains(const std::string &name) const;
    Type GetDataType(const std::string &name) const;

    std::vector<std::pair<std::string, Type>> data;
    std::unordered_map<std::string, int> name2ind;
};

}