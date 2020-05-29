#pragma once

#include "llvm/IR/Type.h"
#include "llvm/IR/DerivedTypes.h"

#include "Type.h"

namespace gen {

class TypeContext {
  public:
    static llvm::Type *Type(const sem::Type &type);
    static llvm::Type *Int();
    static llvm::Type *Real();
    static llvm::Type *Char();
    static llvm::Type *Bool();
    static llvm::Type *CharPtr();
    static llvm::Type *Void();
    static llvm::ArrayType *Array(const sem::Array &arr);
    static llvm::StructType *Record(const sem::Record &rec);

    void NameType(const std::string &name, const sem::Type &type);
    bool HasName(const std::string &name) const;
    llvm::Type *GetType(const std::string &name) const;

  private:
    std::unordered_map<std::string, llvm::Type *> types;
};

}