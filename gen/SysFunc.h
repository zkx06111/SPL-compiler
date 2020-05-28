#pragma once

#include "llvm/IR/Value.h"

#include "ExValue.h"
#include "Type.h"

namespace gen {

llvm::Value *DoPred(const sem::Type &ty, llvm::Value *val);
llvm::Value *DoSucc(const sem::Type &ty, llvm::Value *val);
llvm::Value *DoSqr(const sem::Type &ty, llvm::Value *val);
llvm::Value *DoSqrt(const sem::Type &ty, llvm::Value *val);
llvm::Value *DoAbs(const sem::Type &ty, llvm::Value *val);
llvm::Value *DoChr(const sem::Type &ty, llvm::Value *val);
llvm::Value *DoOrd(const sem::Type &ty, llvm::Value *val);
llvm::Value *DoOdd(const sem::Type &ty, llvm::Value *val);

void Write(const std::vector<ExValue> &params, bool newline = false);
void Read(const std::vector<ExValue> &params, bool newline = false);

}