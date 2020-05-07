#pragma once

#include "IdTable.h"
#include "TypeTable.h"
#include "VarTable.h"
#include "ConstTable.h"
#include "FuncTable.h"

namespace sem {

template <typename T>
using Result = std::pair<T, std::string>;

class SymbolTable {
  public:
    SymbolTable();

    void NewScope();
    void EndScope();

    bool CheckId(const std::string &name) const;
    bool CheckType(const std::string &name) const;
    bool CheckEnum(const std::string &name) const;
    bool CheckVar(const std::string &name) const;
    bool CheckConst(const std::string &name) const;
    bool CheckFunc(const std::string &name) const;

    Type GetType(const std::string &name) const;
    Type GetEnumType(const std::string &name) const;
    Type GetVarType(const std::string &name) const;
    Type GetConstType(const std::string &name) const;
    Func GetFunc(const std::string &name) const;

    int GetConstValI(const std::string &name) const;
    double GetConstValR(const std::string &name) const;
    char GetConstValC(const std::string &name) const;
    bool GetConstValB(const std::string &name) const;
    std::string GetConstValS(const std::string &name) const;

    Enum GetEnum(const Type &type) const;
    Subrange GetSubrange(const Type &type) const;
    Array GetArray(const Type &type) const;
    Record GetRecord(const Type &type) const;

    Result<Type> NewEnum(const std::vector<std::string> &names);
    Result<Type> NewIntSubrange(int l, int r);
    Result<Type> NewCharSubrange(char l, char r);
    Result<Type> NewBoolSubrange(bool l, bool r);
    Result<Type> NewEnumSubrange(const std::string &l, const std::string &r);
    Result<Type> NewArray(const Type &ind_type, const Type &ele_type);
    Result<Type>
        NewRecord(const std::vector<std::pair<std::string, Type>> &data);
    std::string NameType(const std::string &name, const Type &type);

    std::string NewVariable(const std::string &name, const Type &type);

    std::string NewConst(const std::string &name, int val);
    std::string NewConst(const std::string &name, double val);
    std::string NewConst(const std::string &name, bool val);
    std::string NewConst(const std::string &name, char val);
    std::string NewConst(const std::string &name, const std::string &val);

    std::string NewProc(const std::string &name,
        const std::vector<std::pair<std::string, Type>> &args);
    std::string NewFunc(const std::string &name, const Type &ret,
        const std::vector<std::pair<std::string, Type>> &args);

  private:
    std::string
        NewConst(const std::string &name, const Type &type, const Enum &enm);

    std::vector<IdTable> id_t;
    std::vector<TypeTable> type_t;
    std::vector<VarTable> var_t;
    std::vector<ConstTable> const_t;
    std::vector<FuncTable> func_t;
};

extern SymbolTable sym_t;

}