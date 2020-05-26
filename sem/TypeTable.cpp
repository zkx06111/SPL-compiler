#include "TypeTable.h"

namespace sem
{

    Type TypeTable::AddEnum(const Enum &enm)
    {
        enums.push_back(enm);
        return {Type::ENUM, static_cast<int>(enums.size() - 1), depth};
    }

    Type TypeTable::AddSubrange(const Subrange &rng)
    {
        subranges.push_back(rng);
        return {Type::SUBRANGE, static_cast<int>(subranges.size() - 1), depth};
    }

    Type TypeTable::AddArray(const Array &arr)
    {
        arrays.push_back(arr);
        return {Type::ARRAY, static_cast<int>(arrays.size() - 1), depth};
    }

    Type TypeTable::AddRecord(const Record &rec)
    {
        records.push_back(rec);
        return {Type::RECORD, static_cast<int>(records.size() - 1), depth};
    }

    void TypeTable::NameType(const std::string &name, const Type &type)
    {
        types[name] = type;
    }

    bool TypeTable::CheckType(const std::string &name) const
    {
        return types.count(name) != 0;
    }

    Type TypeTable::GetType(const std::string &name) const
    {
        return types.at(name);
    }

    Enum TypeTable::GetEnum(int ind) const
    {
        return enums[ind];
    }

    Enum TypeTable::GetEnum(const std::string &name) const
    {
        return enums[types.at(name).ind];
    }

    Subrange TypeTable::GetSubrange(int ind) const
    {
        return subranges[ind];
    }

    Subrange TypeTable::GetSubrange(const std::string &name) const
    {
        return subranges[types.at(name).ind];
    }

    Array TypeTable::GetArray(int ind) const
    {
        return arrays[ind];
    }

    Array TypeTable::GetArray(const std::string &name) const
    {
        return arrays[types.at(name).ind];
    }

    Record TypeTable::GetRecord(int ind) const
    {
        return records[ind];
    }

    Record TypeTable::GetRecord(const std::string &name) const
    {
        return records[types.at(name).ind];
    }

} // namespace sem