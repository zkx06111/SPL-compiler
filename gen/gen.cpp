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

static ExValue GetIDValue(const std::string &name) {
    if (gen_c.HasVariable(name)) {
        return gen_c.GetVariable(name);
    } else {
        return gen_c.GetConst(name);
    }
}

static void GenLabelPart(const TreeNode *u) {
    for (const TreeNode *v = u->child; v; v = v->sibling) {
        int i = v->vali;
        gen_c.NewLabel(i);
    }
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

static ExValue GenFactor(const TreeNode *u) {
    // TODO
}

static ExValue GenTerm(const TreeNode *u) {
    // TODO
}

static ExValue GenExpr(const TreeNode *u) {
    // TODO
}

static ExValue GenExpression(const TreeNode *u) {
    // TODO
}

static void GenCompoundStmt(const TreeNode *u);
static void GenStmt(const TreeNode *u);

static void GenAssignStmt(const TreeNode *u) {
    const TreeNode *v = u->child;
    std::string name(v->vals);
    v = v->sibling;
    if (strcmp(v->type, "ASSIGN") == 0) {
        ExValue dste = gen_c.GetVariable(name);
        ExValue srce = GenExpression(v->sibling);
        Assign(dste, srce);
    } else if (strcmp(v->type, "LB") == 0) {
        v = v->sibling;
        ExValue inde = GenExpression(v);
        ExValue srce = GenExpression(v->sibling->sibling->sibling);

        sem::Type arr_type = sem::sym_t.GetVarType(name);
        sem::Array arr = sem::sym_t.GetArray(arr_type);
        int base = arr.GetBase();
        ExValue base_eval;
        base_eval.type = sem::Type::Int();
        base_eval.is_const = true;
        base_eval.val_i = base;
        ExValue indei = Cast(sem::Type::Int(), inde);
        ExValue bias = DoSub(indei, base_eval);
        ExValue arre = gen_c.GetVariable(name);
        llvm::Value *dst = ir_builder.CreateGEP(arre.addr,
            { ConstContext::Const(0), bias.Value() });

        Assign(dst, srce.Value());
    } else if (strcmp(v->type, "DOT") == 0) {
        v = v->sibling->sibling;
        std::string member_name(v->vals);
        ExValue srce = GenExpression(v->sibling->sibling);

        sem::Type rec_type = sem::sym_t.GetVarType(name);
        sem::Record rec = sem::sym_t.GetRecord(rec_type);
        int bias = rec.name2ind[member_name];
        ExValue rece = gen_c.GetVariable(name);
        llvm::Value *dst = ir_builder.CreateGEP(rece.addr,
            { ConstContext::Const(0), ConstContext::Const(bias) });

        Assign(dst, srce.Value());
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
    ExValue conde = GenExpression(v);

    if (conde.is_const) {
        if (conde.val_b) {
            v = v->sibling;
            GenStmt(v);
        } else {
            v = v->sibling->sibling;
            GenStmt(v);
        }
        return;
    }

    llvm::BasicBlock *if_true = LabelContext::NewBlock("if_true");
    llvm::BasicBlock *if_false = LabelContext::NewBlock("if_false");
    ir_builder.CreateCondBr(conde.Value(), if_true, if_false);

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
    ExValue conde = GenExpression(v->sibling);
    ir_builder.CreateCondBr(conde.Value(), after_repeat, loop);

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
    ExValue conde = GenExpression(v);
    ir_builder.CreateCondBr(conde.Value(), loop, after_while);

    ir_builder.SetInsertPoint(after_while);
}

static void GenForStmt(const TreeNode *u) {
    gen_c.NewScope();
    sem::sym_t.NewScope();

    const TreeNode *v = u->child;
    std::string name(v->vals);
    v = v->sibling;
    ExValue inite = GenExpression(v);
    gen_c.NewVariable(name, inite.type);
    sem::sym_t.NewVariable(name, inite.type);
    ExValue loop_var = gen_c.GetVariable(name);
    Assign(loop_var, inite);

    v = v->sibling;
    bool asc = (strcmp(v->child->type, "TO") == 0);

    v = v->sibling;
    llvm::BasicBlock *loop = LabelContext::NewBlock("loop_for");
    llvm::BasicBlock *cond = LabelContext::NewBlock("for_cond");
    llvm::BasicBlock *after_for = LabelContext::NewBlock("after_for");

    ir_builder.CreateBr(cond);

    ir_builder.SetInsertPoint(loop);
    llvm::Value *loop_var_val;
    ExValue loop_var_next;
    if (asc) {
        loop_var_next = DoSucc(loop_var);
    } else {
        loop_var_next = DoPred(loop_var);
    }
    Assign(loop_var, loop_var_next);
    GenStmt(v->sibling);

    ir_builder.SetInsertPoint(cond);
    ExValue finale = GenExpression(v);
    ExValue conde;
    if (asc) {
        conde = CmpLessEq(loop_var, finale);
    } else {
        conde = CmpGreatEq(loop_var, finale);
    }
    ir_builder.CreateCondBr(conde.Value(), loop, after_for);

    gen_c.EndScope();
    sem::sym_t.EndScope();
    ir_builder.SetInsertPoint(after_for);
}

static void GenCaseStmt(const TreeNode *u) {
    const TreeNode *v = u->child;
    ExValue case_eval = GenExpression(v);

    v = v->sibling;
    int n_case = 0;
    llvm::BasicBlock *after_case = LabelContext::NewBlock("after_case");
    for (const TreeNode *w = v->child; w; w = w->sibling) {
        const TreeNode *cond_node = w->child;
        ExValue case_item_eval;
        if (strcmp(cond_node->type, "ID") == 0) {
            case_item_eval = GetIDValue(cond_node->vals);
        } else if (strcmp(v->type, "INTEGER") == 0) {
            int val = v->vali;
            case_item_eval.is_const = true;
            case_item_eval.val_i = val;
            case_item_eval.type = sem::Type::Int();
        } else if (strcmp(v->type, "REAL") == 0) {
            double val = v->valf;
            case_item_eval.is_const = true;
            case_item_eval.val_r = val;
            case_item_eval.type = sem::Type::Real();
        } else if (strcmp(v->type, "CHAR") == 0) {
            char val = v->valc;
            case_item_eval.is_const = true;
            case_item_eval.val_c = val;
            case_item_eval.type = sem::Type::Char();
        } else if (strcmp(v->type, "BOOLEAN") == 0) {
            bool val = v->vali;
            case_item_eval.is_const = true;
            case_item_eval.val_b = val;
            case_item_eval.type = sem::Type::Bool();
        }

        ExValue conde = CmpEqual(case_eval, case_item_eval);
        std::string ns = std::to_string(n_case);
        llvm::BasicBlock *bt = LabelContext::NewBlock("case_" + ns + "_true");
        llvm::BasicBlock *bf = LabelContext::NewBlock("case_" + ns + "_false");
        ir_builder.CreateCondBr(conde.Value(), bt, bf);

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
    ir_builder.CreateBr(block);
}

static void GenStmt(const TreeNode *u) {
    if (strcmp(u->child->type, "INTEGER") == 0) {
        int label = u->child->vali;
        llvm::BasicBlock *block = gen_c.GetBlock(label);
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