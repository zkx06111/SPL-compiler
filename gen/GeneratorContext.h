#pragma once

#include "ConstContext.h"
#include "FunctionContext.h"
#include "LabelContext.h"
#include "TypeContext.h"
#include "ValueContext.h"

namespace gen {

class GeneratorContext {
  public:
    GeneratorContext();

    void NewScope();
    void EndScope();
    bool IsGlobal() const;
    
    void NewVariable(const std::string &name, const sem::Type &ty);
    bool HasVariable(const std::string &name) const;
    ExValue GetVariable(const std::string &name) const;
    // void ModifyVariable(const std::string &name, const ExValue &eval);

    void NewConstant(const std::string &name, int val);
    void NewConstant(const std::string &name, double val);
    void NewConstant(const std::string &name, bool val);
    void NewConstant(const std::string &name, char val);
    bool HasConst(const std::string &name) const;
    ExValue GetConst(const std::string &name) const;

    void NewFunction(const std::string &name, const sem::Func &func);
    llvm::Function *GetFunction(const std::string &name) const;

    void NewType(const std::string &name, const sem::Type &type);
    llvm::Type *GetType(const std::string &name) const;

    void NewLabel(int label);
    // bool HasLabel() const;
    llvm::BasicBlock *GetBlock(int label) const;

  private:
    std::vector<ConstContext> const_c;
    std::vector<FunctionContext> func_c;
    std::vector<LabelContext> label_c;
    std::vector<TypeContext> type_c;
    std::vector<ValueContext> val_c;
};

extern GeneratorContext gen_c;

}