#include "gen.h"

#include "llvm/Support/Error.h"

#include "GeneratorContext.h"
#include "Operations.h"
#include "SysFunc.h"
#include "sem.h"
#include "SymbolTable.h"

namespace gen {

llvm::LLVMContext llvm_context;
llvm::Module llvm_module("SPL_module", llvm_context);
llvm::DataLayout llvm_data(&llvm_module);
llvm::IRBuilder<> ir_builder(llvm_context);

static void GenLabelPart(const TreeNode *u) {
    // TODO
}

static void GenConstPart(const TreeNode *u) {
    for (const TreeNode *v = u->child; v; v = v->sibling) {
        std::string name(v->vals);
        v = v->sibling;
        if (strcmp(v->type, "INTEGER") == 0) {
            int val = v->vali;
            gen_c.NewConstant(name, val);
        } else if (strcmp(v->type, "REAL") == 0) {
            double val = v->valf;
            gen_c.NewConstant(name, val);
        } else if (strcmp(v->type, "CHAR") == 0) {
            char val = v->valc;
            gen_c.NewConstant(name, val);
        } else if (strcmp(v->type, "BOOLEAN") == 0) {
            bool val = v->vali;
            gen_c.NewConstant(name, val);
        }
    }
}

static void GenTypePart(const TreeNode *u) {
    for (const TreeNode *v = u->child; v; v = v->sibling) {
        std::string name(v->child->vals);
        sem::Type type = sem::CheckTypeDecl(v->child->sibling);
        gen_c.NewType(name, type);
    }
}

static void GenVarPart(const TreeNode *u) {

}

static void GenRoutinePart(const TreeNode *u) {

}

static void GenRoutineHead(const TreeNode *u) {
    auto v = u->child;
    GenLabelPart(v);
    v = v->sibling;
    GenConstPart(v);
    v = v->sibling;
    GenTypePart(v);
    v = v->sibling;
    GenVarPart(v);
    v = v->sibling;
    GenRoutinePart(v);
}

static void GenRoutineBody(const TreeNode *u) {

}

static void GenRoutine(const TreeNode *u) {
    GenRoutineHead(u);
    GenRoutineBody(u);
}

void GenCode(const TreeNode *u) {
    sem::sym_t = sem::SymbolTable();

    llvm::FunctionType *func_type =
        llvm::FunctionType::get(TypeContext::Int(), false);
    llvm::Function *fn_main = llvm::Function::Create(func_type,
        llvm::Function::ExternalLinkage, "main", &llvm_module);
    llvm::BasicBlock *block =
        llvm::BasicBlock::Create(llvm_context, "main", fn_main);
    ir_builder.SetInsertPoint(block);

    GenRoutine(u);

    FunctionContext::Return(ValueContext::Const(0));

    freopen("ir.llvm", "w", stdin);
    llvm_module.print(llvm::outs(), nullptr);
}

}