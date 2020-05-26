#pragma once

#include "FunctionContext.h"
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
    void NewConstant(const std::string &name, int val);
    void NewConstant(const std::string &name, double val);
    void NewConstant(const std::string &name, bool val);
    void NewConstant(const std::string &name, char val);
    llvm::Value *GetValue(const std::string &name) const;

    void NewFunction(const std::string &name, const sem::Func &func);
    llvm::Function *GetFunction(const std::string &name) const;

    void NewType(const std::string &name, const sem::Type &type);
    llvm::Type *GetType(const std::string &name) const;

  private:
    std::vector<FunctionContext> func_c;
    std::vector<TypeContext> type_c;
    std::vector<ValueContext> val_c;
};

extern GeneratorContext gen_c;

}