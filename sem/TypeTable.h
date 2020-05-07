#pragma once

#include <optional>
#include <unordered_map>

#include "Type.h"

namespace sem {

class TypeTable {
  public:
    TypeTable(int depth) : depth(depth) {}

    Type AddEnum(const Enum &enm);
    Type AddSubrange(const Subrange &rng);
    Type AddArray(const Array &arr);
    Type AddRecord(const Record &rec);
    void NameType(const std::string &name, const Type &type);

    bool CheckType(const std::string &name) const;
    Type GetType(const std::string &name) const;
    
    Enum GetEnum(int ind) const;
    Enum GetEnum(const std::string &name) const;
    Subrange GetSubrange(int ind) const;
    Subrange GetSubrange(const std::string &name) const;
    Array GetArray(int ind) const;
    Array GetArray(const std::string &name) const;
    Record GetRecord(int ind) const;
    Record GetRecord(const std::string &name) const;

  private:
    std::unordered_map<std::string, Type> types;
    std::vector<Enum> enums;
    std::vector<Subrange> subranges;
    std::vector<Array> arrays;
    std::vector<Record> records;
    int depth;
};

}