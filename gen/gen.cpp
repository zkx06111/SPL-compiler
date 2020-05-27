#include "gen.h"

#include "llvm/Support/Error.h"

#include "sem.h"
#include "SymbolTable.h"

#include "GeneratorContext.h"
#include "Operations.h"
#include "SysFunc.h"

namespace gen {

llvm::LLVMContext llvm_context;
llvm::Module llvm_module("SPL_module", llvm_context);
llvm::DataLayout llvm_data(&llvm_module);
llvm::IRBuilder<> ir_builder(llvm_context);

static llvm::Value *GetIDValue(const std::string &name) {
    llvm::Value *val = gen_c.GetVariable(name);
    if (val == nullptr) {
        return gen_c.GetConst(name);
    } else {
        return Deref(val);
    }
}

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
        sem::sym_t.NameType(name, type);
    }
}

static void GenVarPart(const TreeNode *u) {
    for (const TreeNode *v = u->child; v; v = v->sibling) {
        auto names = sem::CheckNameList(v->child);
        sem::Type type = sem::CheckTypeDecl(v->child->sibling);
        for (const auto &name : names) {
            gen_c.NewVariable(name, type);
            sem::sym_t.NewVariable(name, type);
        }
    }
}

void GenFunctionHead(const TreeNode *u, bool is_func) {
    std::string name(u->child->vals);
    const auto &[params, mut_params] =
        sem::CheckParametersDecl(u->child->sibling->child);
    std::vector<sem::Type> params_type;
    for (const auto &[_, ty] : params) {
        params_type.emplace_back(ty);
    }
    if (is_func) {
        sem::Type ret = sem::CheckSimpleTypeDecl(u->child->sibling->sibling->child);
        gen_c.NewFunction(name, sem::Func { ret, params_type, mut_params });
    } else {
        sem::Type ret = sem::Type::Void();
        gen_c.NewFunction(name, sem::Func { ret, params_type, mut_params });
    }
    sem::sym_t.NewScope();
}

static void GenRoutine(const TreeNode *u);
static void GenSubroutine(const TreeNode *u) {
    GenRoutine(u);
    gen_c.EndScope();
    sem::sym_t.EndScope();
}

static void GenFunctionDecl(const TreeNode *u, bool is_func) {
    GenFunctionHead(u->child, is_func);
    GenSubroutine(u->child->sibling);
}

static void GenRoutinePart(const TreeNode *u) {
    for (const TreeNode *v = u->child; v; v = v->sibling) {
        GenFunctionDecl(v, strcmp(v->type, "function_decl") == 0);
    }
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

static llvm::Value *GenFactor(const TreeNode *u) {
    // TODO
}

static llvm::Value *GenTerm(const TreeNode *u) {
    // TODO
}

static llvm::Value *GenExpr(const TreeNode *u) {
    // TODO
}

static llvm::Value *GenExpression(const TreeNode *u) {
    // TODO
}

static void GenCompoundStmt(const TreeNode *u);
static void GenStmt(const TreeNode *u);

static void GenAssignStmt(const TreeNode *u) {
    const TreeNode *v = u->child;
    std::string name(v->vals);
    v = v->sibling;
    if (strcmp(v->type, "ASSIGN") == 0) {
        llvm::Value *dst = gen_c.GetVariable(name);
        llvm::Value *src = GenExpression(v->sibling);
        Assign(dst, src);
    } else if (strcmp(v->type, "LB") == 0) {
        v = v->sibling;
        llvm::Value *ind = GenExpression(v);
        llvm::Value *src = GenExpression(v->sibling->sibling->sibling);

        sem::Type arr_type = sem::sym_t.GetVarType(name);
        sem::Array arr = sem::sym_t.GetArray(arr_type);
        int base = arr.GetBase();
        llvm::Value *bias =
            DoSub(sem::Type::Int(), ind, ConstContext::Const(base));
        llvm::Value *arr_addr = gen_c.GetVariable(name);
        llvm::Value *dst = ir_builder.CreateGEP(arr_addr,
            { ConstContext::Const(0), bias });

        Assign(dst, src);
    } else if (strcmp(v->type, "DOT") == 0) {
        v = v->sibling->sibling;
        std::string member_name(v->vals);
        llvm::Value *src = GenExpression(v->sibling->sibling);

        sem::Type rec_type = sem::sym_t.GetVarType(name);
        sem::Record rec = sem::sym_t.GetRecord(rec_type);
        int bias = rec.name2ind[member_name];
        llvm::Value *rec_addr = gen_c.GetVariable(name);
        llvm::Value *dst = ir_builder.CreateGEP(rec_addr,
            { ConstContext::Const(0), ConstContext::Const(bias) });

        Assign(dst, src);
    }
}

static void GenProcStmt(const TreeNode *u) {
    const TreeNode *v = u->child;
    if (strcmp(v->type, "ID") == 0) {
        // TODO
    } else if (strcmp(v->type, "sys_porc") == 0) {
        // TODO
    } else if (strcmp(v->type, "READ") == 0) {
        // TODO
    }
}

static void GenIfStmt(const TreeNode *u) {
    const TreeNode *v = u->child;
    llvm::Value *condition = GenExpression(v);

    llvm::BasicBlock *if_true = LabelContext::NewBlock("if_true");
    llvm::BasicBlock *if_false = LabelContext::NewBlock("if_false");
    ir_builder.CreateCondBr(condition, if_true, if_false);

    v = v->sibling;
    ir_builder.SetInsertPoint(if_true);
    GenStmt(v);

    v = v->sibling->child;
    if (v->child != nullptr) {
        ir_builder.SetInsertPoint(if_false);
        GenStmt(v->child);
    }

    llvm::BasicBlock *after_if = LabelContext::NewBlock("after_if");
    ir_builder.SetInsertPoint(after_if);
}

static void GenRepeatStmt(const TreeNode *u) {
    const TreeNode *v = u->child;

    llvm::BasicBlock *loop = LabelContext::NewBlock("loop_repeat");
    llvm::BasicBlock *after_repeat = LabelContext::NewBlock("after_repeat");

    ir_builder.SetInsertPoint(loop);
    for (const TreeNode *w = v->child; w; w = w->sibling) {
        GenStmt(w);
    }
    llvm::Value *condition = GenExpression(v->sibling);
    ir_builder.CreateCondBr(condition, after_repeat, loop);

    ir_builder.SetInsertPoint(after_repeat);
}

static void GenWhileStmt(const TreeNode *u) {
    const TreeNode *v = u->child;

    llvm::BasicBlock *loop = LabelContext::NewBlock("loop_while");
    llvm::BasicBlock *cond = LabelContext::NewBlock("while_cond");
    llvm::BasicBlock *after_while = LabelContext::NewBlock("after_while");

    ir_builder.CreateBr(cond);

    ir_builder.SetInsertPoint(loop);
    GenStmt(v->sibling);
    ir_builder.SetInsertPoint(cond);
    llvm::Value *condition = GenExpression(v);
    ir_builder.CreateCondBr(condition, loop, after_while);

    ir_builder.SetInsertPoint(after_while);
}

// TODO - enum for (need 'get expression type')
static void GenForStmt(const TreeNode *u) {
    gen_c.NewScope();
    sem::sym_t.NewScope();

    const TreeNode *v = u->child;
    std::string name(v->vals);
    gen_c.NewVariable(name, sem::Type::Int()); // TODO - type
    sem::sym_t.NewVariable(name, sem::Type::Int());
    llvm::Value *loop_var = gen_c.GetVariable(name);

    v = v->sibling;
    llvm::Value *init_val = GenExpression(v);
    Assign(loop_var, init_val);

    v = v->sibling;
    bool asc = (strcmp(v->child->type, "TO") == 0);

    v = v->sibling;
    llvm::BasicBlock *loop = LabelContext::NewBlock("loop_for");
    llvm::BasicBlock *cond = LabelContext::NewBlock("for_cond");
    llvm::BasicBlock *after_for = LabelContext::NewBlock("after_for");

    ir_builder.CreateBr(cond);

    ir_builder.SetInsertPoint(loop);
    llvm::Value *loop_var_val;
    if (asc) {
        loop_var_val = DoSucc(sem::Type::Int(), Deref(loop_var));
    } else {
        loop_var_val = DoPred(sem::Type::Int(), Deref(loop_var));
    }
    Assign(loop_var, loop_var_val);
    GenStmt(v->sibling);

    ir_builder.SetInsertPoint(cond);
    llvm::Value *final_value = GenExpression(v);
    llvm::Value *condition;
    if (asc) {
        condition = CmpLessEq(sem::Type::Int(), Deref(loop_var), final_value);
    } else {
        condition = CmpGreatEq(sem::Type::Int(), Deref(loop_var), final_value);
    }
    ir_builder.CreateCondBr(condition, loop, after_for);

    gen_c.EndScope();
    sem::sym_t.EndScope();
    ir_builder.SetInsertPoint(after_for);
}

// TODO - type convert (need 'get expression type')
static void GenCaseStmt(const TreeNode *u) {
    const TreeNode *v = u->child;
    llvm::Value *case_val = GenExpression(v);

    v = v->sibling;
    int n_case = 0;
    llvm::BasicBlock *after_case = LabelContext::NewBlock("after_case");
    for (const TreeNode *w = v->child; w; w = w->sibling) {
        const TreeNode *cond_node = w->child;
        llvm::Value *cond_val;
        if (strcmp(cond_node->type, "ID") == 0) {
            cond_val = GetIDValue(cond_node->vals);
        } else if (strcmp(v->type, "INTEGER") == 0) {
            int val = v->vali;
            cond_val = ConstContext::Const(val);
        } else if (strcmp(v->type, "REAL") == 0) {
            double val = v->valf;
            cond_val = ConstContext::Const(val);
        } else if (strcmp(v->type, "CHAR") == 0) {
            char val = v->valc;
            cond_val = ConstContext::Const(val);
        } else if (strcmp(v->type, "BOOLEAN") == 0) {
            bool val = v->vali;
            cond_val = ConstContext::Const(val);
        }

        // TODO - type
        llvm::Value *condition = CmpEqual(sem::Type::Int(), case_val, cond_val);
        std::string ns = std::to_string(n_case);
        llvm::BasicBlock *bt = LabelContext::NewBlock("case_" + ns + "_true");
        llvm::BasicBlock *bf = LabelContext::NewBlock("case_" + ns + "_false");
        ir_builder.CreateCondBr(condition, bt, bf);

        ir_builder.SetInsertPoint(bt);
        GenStmt(cond_node->sibling);
        ir_builder.CreateBr(after_case);
        
        ir_builder.SetInsertPoint(bf);
        ++n_case;
    }

    ir_builder.SetInsertPoint(after_case);
}

static void GenGotoStmt(const TreeNode *u) {
    int label = u->child->vali;
    llvm::BasicBlock *block = gen_c.GetBlock(label);
    if (block == nullptr) {
        gen_c.NewLabel(label);
        block = gen_c.GetBlock(label);
    }
    ir_builder.CreateBr(block);
    llvm::BasicBlock *after_goto = LabelContext::NewBlock("after_goto");
    ir_builder.SetInsertPoint(after_goto);
}

static void GenStmt(const TreeNode *u) {
    if (strcmp(u->child->type, "INTEGER") == 0) {
        int label = u->child->vali;
        llvm::BasicBlock *block = gen_c.GetBlock(label);
        if (block == nullptr) {
            gen_c.NewLabel(label);
            block = gen_c.GetBlock(label);
        }
        ir_builder.CreateBr(block);
        ir_builder.SetInsertPoint(block);
        u = u->sibling;
    }
    if (strcmp(u->type, "assign_stmt") == 0) {
        GenAssignStmt(u);
    } else if (strcmp(u->type, "proc_stmt") == 0) {
        GenProcStmt(u);
    } else if (strcmp(u->type, "compound_stmt") == 0) {
        GenCompoundStmt(u);
    } else if (strcmp(u->type, "if_stmt") == 0) {
        GenIfStmt(u);
    } else if (strcmp(u->type, "repeat_sttm") == 0) {
        GenRepeatStmt(u);
    } else if (strcmp(u->type, "while_stmt") == 0) {
        GenWhileStmt(u);
    } else if (strcmp(u->type, "for_stmt") == 0) {
        GenForStmt(u);
    } else if (strcmp(u->type, "case_stmt") == 0) {
        GenCaseStmt(u);
    } else if (strcmp(u->type, "goto_stmt") == 0) {
        GenGotoStmt(u);
    }
}

static void GenCompoundStmt(const TreeNode *u) {
    for (const TreeNode *v = u->child->child; v; v = v->sibling) {
        GenStmt(v);
    }
}

static void GenRoutineBody(const TreeNode *u) {
    GenCompoundStmt(u->child);
}

static void GenRoutine(const TreeNode *u) {
    GenRoutineHead(u->child);
    GenRoutineBody(u->child->sibling);
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

    FunctionContext::Return(ConstContext::Const(0));

    freopen("ir.llvm", "w", stdin);
    llvm_module.print(llvm::outs(), nullptr);
}

}