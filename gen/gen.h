#pragma

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"

#include "tree.h"

namespace gen {

extern llvm::LLVMContext llvm_context;
extern llvm::Module llvm_module;
extern llvm::IRBuilder<> ir_builder;

void GenCode(const TreeNode *u, const std::string &file_name);

}