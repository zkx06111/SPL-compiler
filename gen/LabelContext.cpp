#include "LabelContext.h"

#include <string>

#include "gen.h"

namespace gen {

void LabelContext::NewLabel(int label) {
    blocks[label] = NewBlock("label_" + std::to_string(label));
}

bool LabelContext::HasLabel(int label) const {
    return blocks.count(label) != 0;
}

llvm::BasicBlock *LabelContext::GetBlock(int label) const {
    return blocks.at(label);
}

llvm::BasicBlock *LabelContext::NewBlock(const std::string &name) {
    llvm::Function *func = ir_builder.GetInsertBlock()->getParent();
    llvm::BaiscBlock *block = llvm::BasicBlock::Create(llvm_context, name, func);
    return block;
}

}