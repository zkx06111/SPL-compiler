#include "TypeContext.h"

#include "gen.h"
#include "SymbolTable.h"
#include "TypeUtil.h"

namespace gen {

llvm::Type *TypeContext::Type(const sem::Type &type) {
    if (type.type == sem::Type::INT) {
        return Int();
    } else if (type.type == sem::Type::REAL) {
        return Real();
    } else if (type.type == sem::Type::CHAR) {
        return Char();
    } else if (type.type == sem::Type::BOOL) {
        return Bool();
    } else if (type.type == sem::Type::ENUM) {
        return Int();
    } else if (type.type == sem::Type::ARRAY) {
        sem::Array arr = sem::sym_t.GetArray(type);
        return Array(arr);
    } else if (type.type == sem::Type::RECORD) {
        sem::Record rec = sem::sym_t.GetRecord(type);
        return Record(rec);
    } else if (type.type == sem::Type::SUBRANGE) {
        return Type(sem::RemoveSubrange(type));
    }
    return Void();
}

llvm::PointerType *TypeContext::PtrType(const sem::Type &type) {
    return Type(type)->getPointerTo();
}

llvm::Type *TypeContext::Int() {
    return llvm::Type::getInt32Ty(llvm_context);
}

llvm::Type *TypeContext::Real() {
    return llvm::Type::getDoubleTy(llvm_context);
}

llvm::Type *TypeContext::Char() {
    return llvm::Type::getInt8Ty(llvm_context);
}

llvm::Type *TypeContext::Bool() {
    return llvm::Type::getInt1Ty(llvm_context);
}

llvm::Type *TypeContext::CharPtr() {
    return llvm::Type::getInt8PtrTy(llvm_context);
}

llvm::Type *TypeContext::Void() {
    return llvm::Type::getVoidTy(llvm_context);
}

llvm::ArrayType *TypeContext::Array(const sem::Array &arr) {
    return llvm::ArrayType::get(Type(arr.ele_type), arr.GetSize());
}

llvm::StructType *TypeContext::Record(const sem::Record &rec) {
    std::vector<llvm::Type *> vec;
    for (const auto &[_, ty] : rec.data) {
        vec.emplace_back(Type(ty));
    }
    return llvm::StructType::get(llvm_context, vec);
}

void TypeContext::NameType(const std::string &name, const sem::Type &type) {
    types[name] = Type(type);
}

bool TypeContext::HasName(const std::string &name) const {
    return types.count(name) != 0;
}

llvm::Type *TypeContext::GetType(const std::string &name) const {
    return types.at(name);
}

}