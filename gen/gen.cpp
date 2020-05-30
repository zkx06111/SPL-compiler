#include "gen.h"

#include <system_error>

#include "llvm/Support/Error.h"

#include "sem.h"
#include "SymbolTable.h"
#include "TypeUtil.h"
#include "GeneratorContext.h"
#include "Operations.h"
#include "SysFunc.h"

#include <iostream>
#define LOG(expr) std::cout << expr << std::endl

namespace gen {

llvm::LLVMContext llvm_context;
llvm::Module llvm_module("SPL_module", llvm_context);
llvm::IRBuilder<> ir_builder(llvm_context);

static std::string src_file_name;

static sem::Type GetSimpleType(const sem::Type &type) {
    sem::Type ret = sem::RemoveSubrange(type);
    if (ret.type == sem::Type::ENUM) {
        ret = sem::Type::Int();
    }
    return ret;
}

static ExValue GetIDValue(const std::string &name, bool prop) {
    ExValue ret;
    if (gen_c.HasVariable(name)) {
        ret = gen_c.GetVariable(name);
        if (!prop) {
            ret.is_const = false;
        }
    } else if (gen_c.HasConst(name)) {
        ret = gen_c.GetConst(name);
    } else {
        ret = ConstContext::ConstEVal(sem::sym_t.GetConstValI(name));
    }
    return ret;
}

static void GenLabelPart(const TreeNode *u) {}

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
        type = GetSimpleType(type);
        for (const auto &name : names) {
            gen_c.NewVariable(name, type);
        }
    }
}

static void GenFunctionHead(const TreeNode *u, bool is_func) {
    const TreeNode *v = u->child;
    std::string name(v->vals);
    v = v->sibling;
    const auto &[params, mut_params] = sem::CheckParametersDecl(v->child);
    if (is_func) {
        v = v->sibling;
        sem::Type ret = sem::CheckSimpleTypeDecl(v->child);
        gen_c.NewFunc(name, ret, params, mut_params);
    } else {
        gen_c.NewFunc(name, sem::Type::Void(), params, mut_params);
    }
    sem::sym_t.NewScope();
}

static void GenRoutinePart(const TreeNode *u);
static void GenRoutineBody(const TreeNode *u);
static void GenSubroutine(const TreeNode *u) {
    auto v = u->child->rchild;
    GenRoutinePart(v);

    gen_c.BeginFunc();
    v = u->child->child;
    GenLabelPart(v);
    v = v->sibling;
    GenConstPart(v);
    v = v->sibling;
    GenTypePart(v);
    v = v->sibling;
    GenVarPart(v);

    v = u->child->sibling;
    GenRoutineBody(v);

    gen_c.EndFunc();
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

static ExValue GenExpression(const TreeNode *u, bool prop);

static ExValue GetArrayElement(const std::string &name, const ExValue &inde) {
    ExValue arre = gen_c.GetVariable(name);
    sem::Array arr = sem::sym_t.GetArray(arre.type);

    int base = arr.GetBase();
    ExValue base_eval = ConstContext::ConstEVal(base);
    ExValue indei = Cast(sem::Type::Int(), inde);
    ExValue bias = DoSub(indei, base_eval);

    ExValue ret;
    ret.addr = ir_builder.CreateGEP(arre.addr,
        { ConstContext::Const(0), bias.Value() });
    ret.type = arr.GetElementType();
    ret.type = GetSimpleType(ret.type);
    return ret;
}

static ExValue
GetRecordMember(const std::string &rec_name, const std::string &mem_name) {
    ExValue rece = gen_c.GetVariable(rec_name);
    sem::Record rec = sem::sym_t.GetRecord(rece.type);
    int bias = rec.name2ind[mem_name];
    ExValue ret;
    ret.addr = ir_builder.CreateGEP(rece.addr,
        { ConstContext::Const(0), ConstContext::Const(bias) });
    ret.type = rec.GetDataType(mem_name);
    ret.type = GetSimpleType(ret.type);
    return ret;
}

static std::vector<ExValue> GenExpressionList(const TreeNode *u, bool prop) {
    std::vector<ExValue> ret;
    for (const TreeNode *v = u->child; v; v = v->sibling) {
        ret.emplace_back(GenExpression(v->child, prop));
    }
    return ret;
}

static ExValue GenFactor(const TreeNode *u, bool prop) {
    if (strcmp(u->type, "INTEGER") == 0) {
        ExValue ret = ConstContext::ConstEVal(u->vali);
        return ret;
    } else if (strcmp(u->type, "REAL") == 0) {
        ExValue ret = ConstContext::ConstEVal(u->valf);
        return ret;
    } else if (strcmp(u->type, "BOOLEAN") == 0) {
        ExValue ret = ConstContext::ConstEVal(bool(u->vali));
        return ret;
    } else if (strcmp(u->type, "CHAR") == 0) {
        ExValue ret = ConstContext::ConstEVal(u->valc);
        return ret;
    } else if (strcmp(u->type, "ID") == 0) {
        std::string name(u->vals);
        if (gen_c.HasFunction(name)) {
            FuncSign sign = gen_c.GetFunction(name);
            ExValue ret = sign.Call({});
            return ret;
        } else {
            ExValue ret = GetIDValue(name, prop);
            return ret;
        }
    } else if (strcmp(u->type, "NOT") == 0) {
        ExValue val = GenFactor(u->child, prop);
        ExValue ret = DoNot(val);
        return ret;
    } else if (strcmp(u->type, "MINUS") == 0) {
        ExValue val = GenFactor(u->child, prop);
        ExValue ret = DoNeg(val);
        return ret;
    } else if (strcmp(u->type, "factor") == 0) {
        const TreeNode *v = u->child;
        if (strcmp(v->type, "ID") == 0) {
            std::string name(v->vals);
            v = v->sibling;
            if (strcmp(v->type, "LP") == 0) {
                auto params = GenExpressionList(v->sibling, prop);
                FuncSign sign = gen_c.GetFunction(name);
                ExValue ret = sign.Call(params);
                return ret;
            } else if (strcmp(v->type, "LB") == 0) {
                ExValue inde = GenExpression(v->sibling, prop);
                ExValue val = GetArrayElement(name, inde);
                return val;
            } else {
                std::string mem_name(v->sibling->vals);
                ExValue val = GetRecordMember(name, mem_name);
                return val;
            }
        } else {
            auto params = GenExpressionList(v->sibling->sibling, prop);
            ExValue val = params[0], ret;
            v = v->child;
            if (strcmp(v->type, "ABS") == 0) {
                ret = DoAbs(val);
            } else if (strcmp(v->type, "CHR") == 0) {
                ret = DoChr(val);
            } else if (strcmp(v->type, "ODD") == 0) {
                ret = DoOdd(val);
            } else if (strcmp(v->type, "ORD") == 0) {
                ret = DoOrd(val);
            } else if (strcmp(v->type, "PRED") == 0) {
                ret = DoPred(val);
            } else if (strcmp(v->type, "SQR") == 0) {
                ret = DoSqr(val);
            } else if (strcmp(v->type, "SQRT") == 0) {
                ret = DoSqrt(val);
            } else if (strcmp(v->type, "SUCC") == 0) {
                ret = DoSucc(val);
            }
            return ret;
        }
    } else {
        ExValue ret = GenExpression(u, prop);
        return ret;
    }
}

static ExValue GenTerm(const TreeNode *u, bool prop) {
    if (strcmp(u->type, "MUL") == 0) {
        ExValue lhs = GenTerm(u->child, prop);
        ExValue rhs = GenFactor(u->child->sibling, prop);
        ExValue ret = DoMul(lhs, rhs);
        return ret;
    } else if (strcmp(u->type, "DIV") == 0) {
        ExValue lhs = GenTerm(u->child, prop);
        ExValue rhs = GenFactor(u->child->sibling, prop);
        ExValue ret = DoDiv(lhs, rhs);
        return ret;
    } else if (strcmp(u->type, "MOD") == 0) {
        ExValue lhs = GenTerm(u->child, prop);
        ExValue rhs = GenFactor(u->child->sibling, prop);
        ExValue ret = DoMod(lhs, rhs);
        return ret;
    } else if (strcmp(u->type, "AND") == 0) {
        ExValue lhs = GenTerm(u->child, prop);
        ExValue rhs = GenFactor(u->child->sibling, prop);
        ExValue ret = DoAnd(lhs, rhs);
        return ret;
    } else {
        ExValue ret = GenFactor(u, prop);
        return ret;
    }
}

static ExValue GenExpr(const TreeNode *u, bool prop) {
    if (strcmp(u->type, "PLUS") == 0) {
        ExValue lhs = GenExpr(u->child, prop);
        ExValue rhs = GenTerm(u->child->sibling, prop);
        ExValue ret = DoAdd(lhs, rhs);
        return ret;
    } else if (strcmp(u->type, "MINUS") == 0 && u->child->sibling != nullptr) {
        ExValue lhs = GenExpr(u->child, prop);
        ExValue rhs = GenTerm(u->child->sibling, prop);
        ExValue ret = DoSub(lhs, rhs);
        return ret;
    } else if (strcmp(u->type, "OR") == 0) {
        ExValue lhs = GenExpr(u->child, prop);
        ExValue rhs = GenTerm(u->child->sibling, prop);
        ExValue ret = DoOr(lhs, rhs);
        return ret;
    } else {
        ExValue ret = GenTerm(u, prop);
        return ret;
    }
}

static ExValue GenExpression(const TreeNode *u, bool prop) {
    if (strcmp(u->type, "EQUAL") == 0) {
        ExValue lhs = GenExpression(u->child, prop);
        ExValue rhs = GenExpr(u->child->sibling, prop);
        ExValue ret = CmpEqual(lhs, rhs);
        return ret;
    } else if (strcmp(u->type, "UNEQUAL") == 0) {
        ExValue lhs = GenExpression(u->child, prop);
        ExValue rhs = GenExpr(u->child->sibling, prop);
        ExValue ret = CmpNEqual(lhs, rhs);
        return ret;
    } else if (strcmp(u->type, "LT") == 0) {
        ExValue lhs = GenExpression(u->child, prop);
        ExValue rhs = GenExpr(u->child->sibling, prop);
        ExValue ret = CmpLess(lhs, rhs);
        return ret;
    } else if (strcmp(u->type, "LE") == 0) {
        ExValue lhs = GenExpression(u->child, prop);
        ExValue rhs = GenExpr(u->child->sibling, prop);
        ExValue ret = CmpLessEq(lhs, rhs);
        return ret;
    } else if (strcmp(u->type, "GT") == 0) {
        ExValue lhs = GenExpression(u->child, prop);
        ExValue rhs = GenExpr(u->child->sibling, prop);
        ExValue ret = CmpGreat(lhs, rhs);
        return ret;
    } else if (strcmp(u->type, "GE") == 0) {
        ExValue lhs = GenExpression(u->child, prop);
        ExValue rhs = GenExpr(u->child->sibling, prop);
        ExValue ret = CmpGreatEq(lhs, rhs);
        return ret;
    } else {
        ExValue ret = GenExpr(u, prop);
        return ret;
    }
}

static void GenCompoundStmt(const TreeNode *u, bool prop);
static void GenStmt(const TreeNode *u, bool prop);

static void GenAssignStmt(const TreeNode *u, bool prop) {
    const TreeNode *v = u->child;
    std::string name(v->vals);
    v = v->sibling;
    if (strcmp(v->type, "ASSIGN") == 0) {
        ExValue dste = gen_c.GetVariable(name);
        ExValue srce = GenExpression(v->sibling->child, prop);
        Assign(dste, srce);
        gen_c.ModifyVariable(name, dste);
    } else if (strcmp(v->type, "LB") == 0) {
        v = v->sibling;
        ExValue inde = GenExpression(v->child, prop);
        ExValue srce = GenExpression(v->sibling->sibling->sibling->child, prop);
        ExValue dste = GetArrayElement(name, inde);
        Assign(dste.addr, srce.Value());
    } else if (strcmp(v->type, "DOT") == 0) {
        v = v->sibling;
        std::string member_name(v->vals);
        ExValue srce = GenExpression(v->sibling->sibling->child, prop);
        ExValue dste = GetRecordMember(name, member_name);
        Assign(dste.addr, srce.Value());
    }
}

static void GenProcStmt(const TreeNode *u, bool prop) {
    const TreeNode *v = u->child;
    if (strcmp(v->type, "ID") == 0) {
        std::string name(v->vals);
        FuncSign sign = gen_c.GetFunction(name);
        std::vector<ExValue> params;
        if (v->sibling != nullptr) {
            params = GenExpressionList(v->sibling->sibling, prop);
        }
        sign.Call(params);
    } else if (strcmp(v->type, "sys_proc") == 0) {
        auto params = GenExpressionList(v->sibling->sibling, prop);
        bool newline = (strcmp(v->child->type, "WRITELN") == 0);
        Write(params, newline);
    } else if (strcmp(v->type, "READ") == 0) {
        // auto params = GenExpressionList(v->sibling->sibling, prop);
        auto params = { GenFactor(v->sibling->sibling, prop) };
        Read(params);
    }
}

static void GenIfStmt(const TreeNode *u, bool prop) {
    const TreeNode *v = u->child;
    ExValue conde = GenExpression(v->child, prop);

    if (conde.is_const) {
        if (conde.val_b) {
            v = v->sibling;
            GenStmt(v, prop);
        } else {
            v = v->sibling->sibling->child;
            if (v != nullptr) {
                GenStmt(v, prop);
            }
        }
        return;
    }

    llvm::BasicBlock *if_true = LabelContext::NewBlock("if_true");
    llvm::BasicBlock *if_false = LabelContext::NewBlock("if_false");
    llvm::BasicBlock *after_if = LabelContext::NewBlock("after_if");
    ir_builder.CreateCondBr(conde.Value(), if_true, if_false);

    v = v->sibling;
    ir_builder.SetInsertPoint(if_true);
    GenStmt(v, prop);
    ir_builder.CreateBr(after_if);

    v = v->sibling->child;
    if (v != nullptr) {
        ir_builder.SetInsertPoint(if_false);
        GenStmt(v, prop);
        ir_builder.CreateBr(after_if);
    }

    ir_builder.SetInsertPoint(after_if);
}

static void GenRepeatStmt(const TreeNode *u, bool prop) {
    const TreeNode *v = u->child;

    llvm::BasicBlock *loop = LabelContext::NewBlock("loop_repeat");
    llvm::BasicBlock *after_repeat = LabelContext::NewBlock("after_repeat");

    ir_builder.CreateBr(loop);
    ir_builder.SetInsertPoint(loop);
    for (const TreeNode *w = v->child; w; w = w->sibling) {
        GenStmt(w, false);
    }
    ExValue conde = GenExpression(v->sibling->child, false);
    ir_builder.CreateCondBr(conde.Value(), after_repeat, loop);

    ir_builder.SetInsertPoint(after_repeat);
}

static void GenWhileStmt(const TreeNode *u, bool prop) {
    const TreeNode *v = u->child;

    llvm::BasicBlock *loop = LabelContext::NewBlock("loop_while");
    llvm::BasicBlock *cond = LabelContext::NewBlock("while_cond");
    llvm::BasicBlock *after_while = LabelContext::NewBlock("after_while");

    ir_builder.CreateBr(cond);

    ir_builder.SetInsertPoint(loop);
    GenStmt(v->sibling, false);
    ir_builder.CreateBr(cond);
    ir_builder.SetInsertPoint(cond);
    ExValue conde = GenExpression(v->child, false);
    ir_builder.CreateCondBr(conde.Value(), loop, after_while);

    ir_builder.SetInsertPoint(after_while);
}

static void GenForStmt(const TreeNode *u, bool prop) {
    const TreeNode *v = u->child;
    std::string name(v->vals);
    v = v->sibling;
    ExValue inite = GenExpression(v->child, prop);
    ExValue loop_var = gen_c.GetVariable(name);
    Assign(loop_var, inite);
    loop_var.is_const = false;
    gen_c.ModifyVariable(name, loop_var);

    v = v->sibling;
    bool asc = (strcmp(v->child->type, "TO") == 0);

    v = v->sibling;
    llvm::BasicBlock *loop = LabelContext::NewBlock("loop_for");
    llvm::BasicBlock *cond = LabelContext::NewBlock("for_cond");
    llvm::BasicBlock *after_for = LabelContext::NewBlock("after_for");

    ir_builder.CreateBr(cond);

    ir_builder.SetInsertPoint(loop);
    GenStmt(v->sibling, false);
    ExValue loop_var_next;
    if (asc) {
        loop_var_next = DoSucc(loop_var);
    } else {
        loop_var_next = DoPred(loop_var);
    }
    Assign(loop_var, loop_var_next);
    loop_var.is_const = false;
    gen_c.ModifyVariable(name, loop_var);
    ir_builder.CreateBr(cond);

    ir_builder.SetInsertPoint(cond);
    ExValue finale = GenExpression(v->child, false);
    ExValue conde;
    if (asc) {
        conde = CmpLessEq(loop_var, finale);
    } else {
        conde = CmpGreatEq(loop_var, finale);
    }
    ir_builder.CreateCondBr(conde.Value(), loop, after_for);

    ir_builder.SetInsertPoint(after_for);
}

static void GenCaseStmt(const TreeNode *u, bool prop) {
    const TreeNode *v = u->child;
    ExValue case_eval = GenExpression(v->child, prop);

    v = v->sibling;
    int n_case = 0;
    llvm::BasicBlock *after_case = LabelContext::NewBlock("after_case");
    for (const TreeNode *w = v->child; w; w = w->sibling) {
        const TreeNode *cond_node = w->child;
        ExValue case_item_eval;
        if (strcmp(cond_node->type, "ID") == 0) {
            case_item_eval = GetIDValue(cond_node->vals, prop);
        } else if (strcmp(cond_node->type, "INTEGER") == 0) {
            int val = cond_node->vali;
            case_item_eval = ConstContext::ConstEVal(val);
        } else if (strcmp(cond_node->type, "REAL") == 0) {
            double val = cond_node->valf;
            case_item_eval = ConstContext::ConstEVal(val);
        } else if (strcmp(cond_node->type, "CHAR") == 0) {
            char val = cond_node->valc;
            case_item_eval = ConstContext::ConstEVal(val);
        } else if (strcmp(cond_node->type, "BOOLEAN") == 0) {
            bool val = cond_node->vali;
            case_item_eval = ConstContext::ConstEVal(val);
        }

        ExValue conde = CmpEqual(case_eval, case_item_eval);
        std::string ns = std::to_string(n_case);
        llvm::BasicBlock *bt = LabelContext::NewBlock("case_" + ns + "_true");
        llvm::BasicBlock *bf;
        if (w->sibling == nullptr) {
            bf = after_case;
        } else {
            bf = LabelContext::NewBlock("case_" + ns + "_false");
        }
        ir_builder.CreateCondBr(conde.Value(), bt, bf);

        ir_builder.SetInsertPoint(bt);
        GenStmt(cond_node->sibling, prop);
        ir_builder.CreateBr(after_case);
        
        if (w->sibling != nullptr) {
            ir_builder.SetInsertPoint(bf);
            ++n_case;
        }
    }

    ir_builder.SetInsertPoint(after_case);
}

static void GenGotoStmt(const TreeNode *u, bool prop) {
    int label = u->child->vali;
    llvm::BasicBlock *block = gen_c.GetBlock(label);
    if (block == nullptr) {
        gen_c.NewLabel(label);
        block = gen_c.GetBlock(label);
    }
    ir_builder.CreateBr(block);
    llvm::BasicBlock *after = LabelContext::NewBlock("after_goto");
    ir_builder.SetInsertPoint(after);
}

static void GenStmt(const TreeNode *u, bool prop) {
    u = u->child;
    if (strcmp(u->type, "INTEGER") == 0) {
        int label = u->vali;
        llvm::BasicBlock *block = gen_c.GetBlock(label);
        if (block == nullptr) {
            gen_c.NewLabel(label);
            block = gen_c.GetBlock(label);
        }
        ir_builder.CreateBr(block);
        ir_builder.SetInsertPoint(block);
        gen_c.DeclLabel(label);
        prop = false;
        u = u->sibling;
    }
    if (strcmp(u->type, "assign_stmt") == 0) {
        GenAssignStmt(u, prop);
    } else if (strcmp(u->type, "proc_stmt") == 0) {
        GenProcStmt(u, prop);
    } else if (strcmp(u->type, "compound_stmt") == 0) {
        GenCompoundStmt(u, prop);
    } else if (strcmp(u->type, "if_stmt") == 0) {
        GenIfStmt(u, prop);
    } else if (strcmp(u->type, "repeat_stmt") == 0) {
        GenRepeatStmt(u, prop);
    } else if (strcmp(u->type, "while_stmt") == 0) {
        GenWhileStmt(u, prop);
    } else if (strcmp(u->type, "for_stmt") == 0) {
        GenForStmt(u, prop);
    } else if (strcmp(u->type, "case_stmt") == 0) {
        GenCaseStmt(u, prop);
    } else if (strcmp(u->type, "goto_stmt") == 0) {
        GenGotoStmt(u, prop);
    }
}

static void GenCompoundStmt(const TreeNode *u, bool prop) {
    for (const TreeNode *v = u->child->child; v; v = v->sibling) {
        prop = prop && !gen_c.HasLabel();
        GenStmt(v, prop);
    }
}

static void GenRoutineBody(const TreeNode *u) {
    GenCompoundStmt(u->child, true);
}

static void GenRoutine(const TreeNode *u) {
    auto v = u->child->rchild;
    GenRoutinePart(v);
    FunctionContext::MainFunction();
    v = u->child->child;
    GenLabelPart(v);
    v = v->sibling;
    GenConstPart(v);
    v = v->sibling;
    GenTypePart(v);
    v = v->sibling;
    GenVarPart(v);
    
    GenRoutineBody(u->child->sibling);
}

void GenCode(const TreeNode *u, const std::string &file_name) {
    src_file_name = file_name;
    sem::sym_t = sem::SymbolTable();

    GenRoutine(u->child);
    ir_builder.CreateRet(ConstContext::Const(0));

    std::error_code EC;
    llvm::raw_fd_ostream ll_out(file_name + ".ll", EC);
    llvm_module.print(ll_out, nullptr);
}

void GenExe(std::string output_name) {
    if (output_name == "") {
        int pos = src_file_name.rfind(".");
        output_name = src_file_name.substr(0, pos);
    }
    std::string ll_file_name = src_file_name + ".ll";
    std::string s_file_name = src_file_name + ".s";

    system(("llc " + ll_file_name).c_str());
    system(("gcc -o " + output_name + " -lm " + s_file_name).c_str());
}

}