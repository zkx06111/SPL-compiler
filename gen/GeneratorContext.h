#pragma once

#include <stack>

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
    
    void NewVariable(const std::string &name, const sem::Type &ty,
      bool is_ref = false);
    bool HasVariable(const std::string &name) const;
    ExValue GetVariable(const std::string &name) const;
    void ModifyVariable(const std::string &name, const ExValue &eval);

    void NewConstant(const std::string &name, int val);
    void NewConstant(const std::string &name, double val);
    void NewConstant(const std::string &name, bool val);
    void NewConstant(const std::string &name, char val);
    bool HasConst(const std::string &name) const;
    ExValue GetConst(const std::string &name) const;

    void NewFunc(const std::string &name, const sem::Type &ret,
        const std::vector<std::pair<std::string, sem::Type>> &args,
        const std::vector<int> mut_args);
    void BeginFunc();
    void EndFunc();
    bool HasFunction(const std::string &name) const;
    FuncSign GetFunction(const std::string &name) const;
    FuncSign GetCurrentFunction() const;

    void NewType(const std::string &name, const sem::Type &type);
    llvm::Type *GetType(const std::string &name) const;

    void NewLabel(int label);
    llvm::BasicBlock *GetBlock(int label) const;

  private:
    std::vector<ConstContext> const_c;
    std::vector<FunctionContext> func_c;
    std::vector<LabelContext> label_c;
    std::vector<TypeContext> type_c;
    std::vector<ValueContext> val_c;
    std::stack<FuncSign> scope_stk;

    std::vector<std::vector<std::pair<std::string, ExValue>>> pregs;
};

extern GeneratorContext gen_c;

}