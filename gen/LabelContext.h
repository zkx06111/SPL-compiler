#pragma once

#include <unordered_map>

#include "llvm/IR/BasicBlock.h"

namespace gen {

class LabelContext {
  public:
    void NewLabel(int label);
    bool HasLabel() const;
    bool HasLabel(int label) const;
    llvm::BasicBlock *GetBlock(int label) const;

    static llvm::BasicBlock *NewBlock(const std::string &name = "");

  private:
    std::unordered_map<int, llvm::BasicBlock *> blocks;
};

}