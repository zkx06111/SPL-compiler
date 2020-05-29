#include "sem.h"

#include <iostream>
#include <limits>

#include "SemError.h"
#include "SymbolTable.h"
#include "TypeUtil.h"

#define LOG_ERROR(u, e) do { \
    std::cout << "line " << u->lineNumber << ": " << e.what() << std::endl; \
} while (0)

namespace sem {

std::vector<std::string> CheckNameList(const TreeNode *u) {
    std::vector<std::string> vec;
    for (const TreeNode *v = u->child; v; v = v->sibling) {
        vec.emplace_back(v->vals);
    }
    return vec;
}

static bool CheckLabelPart(const TreeNode *u) {
    bool ret = true;
    for (const TreeNode *v = u->child; v; v = v->sibling) {
        try {
            int i = v->vali;
            sym_t.NewLabel(i);
        } catch (const SemError &e) {
            LOG_ERROR(v, e);
            ret = false;
        }
    }
    return ret;
}

static bool CheckConstPart(const TreeNode *u) {
    bool ret = true;
    for (const TreeNode *v = u->child; v; v = v->sibling) {
        try {
            std::string name(v->vals);
            v = v->sibling;
            if (strcmp(v->type, "INTEGER") == 0) {
                int val = v->vali;
                sym_t.NewConst(name, val);
            } else if (strcmp(v->type, "REAL") == 0) {
                double val = v->valf;
                sym_t.NewConst(name, val);
            } else if (strcmp(v->type, "CHAR") == 0) {
                char val = v->valc;
                sym_t.NewConst(name, val);
            } else if (strcmp(v->type, "BOOLEAN") == 0) {
                bool val = v->vali;
                sym_t.NewConst(name, val);
            }
        } catch (const SemError &e) {
            LOG_ERROR(v, e);
            ret = false;
        }
    }
    return ret;
}

Type CheckSimpleTypeDecl(const TreeNode *u) {
    if (strcmp(u->type, "sys_type") == 0) {
        const TreeNode *v = u->child;
        if (strcmp(v->type, "INTEGER_TYPE") == 0) {
            return Type::Int();
        } else if (strcmp(v->type, "CHAR_TYPE") == 0) {
            return Type::Char();
        } else if (strcmp(v->type, "BOOLEAN_TYPE") == 0) {
            return Type::Bool();
        } else if (strcmp(v->type, "REAL_TYPE") == 0) {
            return Type::Real();
        }
    } else if (strcmp(u->type, "ID") == 0) {
        if (u->sibling == nullptr) {
            std::string name(u->vals);
            return sym_t.GetType(name);
        } else {
            std::string le(u->vals);
            std::string re(u->sibling->sibling->vals);
            return sym_t.NewEnumSubrange(le, re);
        }
    } else if (strcmp(u->type, "name_list") == 0) {
        auto names = CheckNameList(u);
        return sym_t.NewEnum(names);
    } else if (strcmp(u->type, "INTEGER") == 0) {
        const TreeNode *v = u->sibling->sibling;
        if (strcmp(v->type, "INTEGER") == 0) {
            int li = u->vali;
            int ri = v->vali;
            return sym_t.NewIntSubrange(li, ri);
        } else {
            throw SemError("LHS type of '..' doen't match to RHS type");
        }
    } else if (strcmp(u->type, "CHAR") == 0) {
        const TreeNode *v = u->sibling->sibling;
        if (strcmp(v->type, "CHAR") == 0) {
            char lc = u->valc;
            char rc = v->valc;
            return sym_t.NewCharSubrange(lc, rc);
        } else {
            throw SemError("LHS type of '..' doen't match to RHS type");
        }
    } else if (strcmp(u->type, "BOOLEAN") == 0) {
        const TreeNode *v = u->sibling->sibling;
        if (strcmp(v->type, "BOOLEAN") == 0) {
            bool lb = u->vali;
            bool rb = u->vali;
            return sym_t.NewBoolSubrange(lb, rb);
        } else {
            throw SemError("LHS type of '..' doen't match to RHS type");
        }
    } else if (strcmp(u->type, "MINUS") == 0) {
        u = u->sibling;
        if (strcmp(u->type, "INTEGER") != 0) {
            throw SemError("unexpected '-'");
        }
        int li = -u->vali, ri;
        u = u->sibling->sibling;
        if (strcmp(u->type, "MINUS") == 0) {
            u = u->sibling;
            if (strcmp(u->type, "INTEGER") != 0) {
                throw SemError("unexpected '-'");
            }
            ri = -u->vali;
        } else if (strcmp(u->type, "INTEGER") == 0) {
            ri = u->vali;
        }
        return sym_t.NewIntSubrange(li, ri);
    }
    throw SemError("unknown type decleration");
}

static Type CheckArrayTypeDecl(const TreeNode *u) {
    try {
        Type ind_type = CheckSimpleTypeDecl(u->child->child);
        Type ele_type = CheckTypeDecl(u->child->sibling);
        return sym_t.NewArray(ind_type, ele_type);
    } catch (const SemError &e) {
        LOG_ERROR(u, e);
        return Type::Void();
    }
}

static Type CheckRecordTypeDecl(const TreeNode *u) {
    try {
        std::vector<std::pair<std::string, Type>> vec;
        for (const TreeNode *v = u->child; v; v = v->sibling) {
            auto names = CheckNameList(v->child);
            Type type = CheckTypeDecl(v->child->sibling);
            for (const auto &name : names) {
                vec.emplace_back(name, type);
            }
        }
        return sym_t.NewRecord(vec);
    } catch (const SemError &e) {
        LOG_ERROR(u, e);
        return Type::Void();
    }
}

Type CheckTypeDecl(const TreeNode *u) {
    if (strcmp(u->type, "simple_type_decl") == 0) {
        const TreeNode *v = u->child;
        return CheckSimpleTypeDecl(v);
    } else if (strcmp(u->type, "array_type_decl") == 0) {
        return CheckArrayTypeDecl(u);
    } else if (strcmp(u->type, "record_type_decl") == 0) {
        return CheckRecordTypeDecl(u);
    }
    throw SemError("unknown type decleration");
}

static bool CheckTypePart(const TreeNode *u) {
    bool ret = true;
    for (const TreeNode *v = u->child; v; v = v->sibling) {
        try {
            std::string name(v->child->vals);
            Type type = CheckTypeDecl(v->child->sibling);
            sym_t.NameType(name, type);
        } catch (const SemError &e) {
            LOG_ERROR(v, e);
            ret = false;
        }
    }
    return ret;
}

static bool CheckVarPart(const TreeNode *u) {
    bool ret = true;
    for (const TreeNode *v = u->child; v; v = v->sibling) {
        try {
            auto names = CheckNameList(v->child);
            Type type = CheckTypeDecl(v->child->sibling);
            for (const auto &name : names) {
                sym_t.NewVariable(name, type);
            }
        } catch (const SemError &e) {
            LOG_ERROR(v, e);
            ret = false;
        }
    }
    return ret;
}

std::pair<std::vector<std::pair<std::string, Type>>, std::vector<int>>
CheckParametersDecl(const TreeNode *u) {
    std::vector<std::pair<std::string, Type>> params;
    std::vector<int> mut_params;
    int n_param = 0;
    for (const TreeNode *v = u->child; v; v = v->sibling) {
        auto names = CheckNameList(v->child->child);
        Type type;
        try {
            type = CheckSimpleTypeDecl(v->child->sibling->child);
        } catch (const SemError &e) {
            LOG_ERROR(v, e);
            type = Type::Void();
        }
        bool mut = (strcmp(v->child->type, "var_para_list") == 0);
        for (const auto &name : names) {
            params.emplace_back(name, type);
            if (mut) {
                mut_params.emplace_back(n_param);
            }
            ++n_param;
        }
    }
    return { params, mut_params };
}

bool CheckFunctionHead(const TreeNode *u, bool is_func) {
    std::string name(u->child->vals);
    const auto &[params, mut_params] =
        CheckParametersDecl(u->child->sibling->child);
    bool ret = true, flag = false;
    try {
        if (is_func) {
            Type ret = CheckSimpleTypeDecl(u->child->sibling->sibling->child);
            flag = true;
            sym_t.NewFunc(name, ret, params, mut_params);
        } else {
            flag = true;
            sym_t.NewProc(name, params, mut_params);
        }
    } catch (const SemError &e) {
        LOG_ERROR(u, e);
        if (!flag) {
            sym_t.NewScope();
        }
        ret = false;
    }
    return ret;
}

static bool CheckRoutine(const TreeNode *u);
static bool CheckSubroutine(const TreeNode *u) {
    bool ret = CheckRoutine(u);
    sym_t.EndScope();
    return ret;
}

static bool CheckFunctionDecl(const TreeNode *u, bool is_func) {
    bool ret_head = CheckFunctionHead(u->child, is_func);
    bool ret_routine = CheckSubroutine(u->child->sibling);
    return ret_head && ret_routine;
}

static bool CheckRoutinePart(const TreeNode *u) {
    bool ret = true;
    for (const TreeNode *v = u->child; v; v = v->sibling) {
        if (strcmp(v->type, "function_decl") == 0) {
            bool ret_t = CheckFunctionDecl(v, true);
            ret = ret && ret_t;
        } else if (strcmp(v->type, "procedure_decl") == 0) {
            bool ret_t = CheckFunctionDecl(v, false);
            ret = ret && ret_t;
        }
    }
    return ret;
}

static bool CheckRoutineHead(const TreeNode *u) {
    auto v = u->child;
    bool ret = CheckLabelPart(v);
    v = v->sibling;
    bool ret_t = CheckConstPart(v);
    ret = ret && ret_t;
    v = v->sibling;
    ret_t = CheckTypePart(v);
    ret = ret && ret_t;
    v = v->sibling;
    ret_t = CheckVarPart(v);
    ret = ret && ret_t;
    v = v->sibling;
    ret_t = CheckRoutinePart(v);
    return ret && ret_t;
}

void codeError() { // temporary func for debugging
    std::cerr << "Unexpected branch" << std::endl;
    exit(-1);
}

static std::pair<bool, Type> CheckExpression(const TreeNode *u);

static bool CheckStmt(const TreeNode *u);

static std::pair<bool, Type> CheckArrRec(const TreeNode *u) {
    std::cerr << "CheckArrRec" << std::endl;
    Type lvalType = sym_t.GetVarType(u->vals);
    Type lelem;
    bool ret = true;
    if (strcmp(u->sibling->type, "LB") == 0)
    {
        try
        {
            if (lvalType.type != Type::ARRAY)
            {
                throw SemError("Using index to access a non-array");
            }
        }
        catch (const SemError &e)
        {
            LOG_ERROR(u, e);
            ret = false;
        }
        Array lArray = sym_t.GetArray(lvalType);
        auto idx = CheckExpression(u->sibling->sibling);
        if (idx.first == false)
        {
            ret = false;
        } else {
            try {
                lArray.TakeSub(idx.second);
            } catch (const SemError &e) {
                LOG_ERROR(u, e);
                ret = false;
            }
        }
        lelem = lArray.GetElementType();
    }
    else if (strcmp(u->sibling->type, "DOT") == 0)
    {
        try
        {
            if (lvalType.type != Type::RECORD)
            {
                throw SemError("Accessing member value of a non-record");
            }
        }
        catch (const SemError &e)
        {
            LOG_ERROR(u, e);
            ret = false;
        }
        if (lvalType.type == Type::RECORD) {
            Record lRec = sym_t.GetRecord(lvalType);
            try {
                lelem = lRec.GetDataType(u->sibling->sibling->vals);
            } catch (const SemError &e) {
                LOG_ERROR(u, e);
                ret = false;
            }
        }
    }
    return std::make_pair(ret, lelem);
}

static std::pair<bool, std::vector<Type> >
CheckExpressionList(const TreeNode *u) {
    std::cerr << "CheckExpressionList" << std::endl;
    bool resb = true;
    std::vector<Type> resv;
    for (TreeNode *p = u->child; p; p = p->sibling) {
        std::pair<bool, Type> t = CheckExpression(p);
        resb = resb && t.first;
        resv.push_back(t.second);
    }
    return make_pair(resb, resv);
}

static std::pair<bool, Type> CheckFactor(const TreeNode *u) {
    std::cerr << "CheckFactor" << std::endl;
    fprintf(stderr, "%s\n", u->type);
    if (strcmp(getKthChild(u, 2)->type, "LP") != 0) {
        return CheckArrRec(u->child);
    }
    // function call
    bool res = true;
    std::pair<bool, std::vector<Type> > args = CheckExpressionList(getKthChild(u, 3));
    res = args.first;
    Type rest;
    if (strcmp(u->child->type, "sys_funct") == 0) {
        if (args.second.size() != 1) {
            LOG_ERROR(u, SemError("1 parameter expected, "
            + std::to_string(args.second.size()) + " parameter(s) found"));
        }
        const char* funcname = u->child->child->type;
        Type (*func)(const Type &);
        if (strcmp(funcname, "ABS") == 0) {
            func = DoAbs;
        } else if (strcmp(funcname, "CHR") == 0) {
            func = DoChr;
        } else if (strcmp(funcname, "ODD") == 0) {
            func = DoOdd;
        } else if (strcmp(funcname, "ORD") == 0) {
            func = DoOrd;
        } else if (strcmp(funcname, "PRED") == 0) {
            func = DoPred;
        } else if (strcmp(funcname, "SQR") == 0) {
            func = DoSqr;
        } else if (strcmp(funcname, "SQRT") == 0) {
            func = DoSqrt;
        } else if (strcmp(funcname, "SUCC") == 0) {
            func = DoSucc;
        }
        std::cerr << args.second[0].type << std::endl;
        try {
            rest = func(args.second[0]);
        } catch (const SemError &e) {
            LOG_ERROR(u, e);
            res = false;
        }
    } else {
        try{
            if (not sym_t.CheckFunc(u->child->vals)) {
                std::string msg = "function " + std::string(u->child->vals) + " undefined";
                throw SemError(msg);
            }
        } catch (const SemError &e) {
            res = false;
            LOG_ERROR(u, e);
        }
        if (sym_t.CheckFunc(u->child->vals)) {
            Func f = sym_t.GetFunc(u->child->vals);
            try {
                f.ApplyArgs(args.second);
                rest = f.ret;
            } catch (const SemError &e) {
                res = false;
                LOG_ERROR(u, e);
            }
        }
    }
    return std::make_pair(res, rest);
}

static std::pair<bool, Type> CheckExpression(const TreeNode *u) {
    std::cerr << "CheckExpression" << std::endl;
    fprintf(stderr, "%s\n", u->type);
    if (strcmp(u->type, "expression") == 0) {
        return CheckExpression(u->child);
    }
    const char* op = u->type;
    if (strcmp(op, "factor") == 0) {
        return CheckFactor(u);
    }
    if (strcmp(u->type, "ID") == 0) {
        Type rest;
        bool resb = true;
        try {
            if (sym_t.CheckFunc(u->vals)) {
                rest = sym_t.GetFunc(u->vals).ret;
            } else if (sym_t.CheckVar(u->vals)) {
                rest = sym_t.GetVarType(u->vals);
            } else if (sym_t.CheckConst(u->vals)) {
                rest = sym_t.GetConstType(u->vals);
            } else if (sym_t.CheckEnum(u->vals)) {
                rest = sym_t.GetEnumType(u->vals);
            } else {
                throw SemError("ID " + std::string(u->vals) + " not defined");
            }
        } catch (const SemError &e) {
            LOG_ERROR(u, e);
            resb = false;
        }
        return std::make_pair(resb, rest);
    }
    if (strcmp(u->type, "INTEGER") == 0) {
        return std::make_pair(true, Type::Int());
    }
    if (strcmp(u->type, "REAL") == 0) {
        return std::make_pair(true, Type::Real());
    }
    if (strcmp(u->type, "CHAR") == 0) {
        return std::make_pair(true, Type::Char());
    }
    if (strcmp(u->type, "BOOLEAN") == 0) {
        return std::make_pair(true, Type::Bool());
    }
    
    if (strcmp(op, "NOT") == 0) {
        std::pair<bool, Type> p = CheckExpression(u->child);
        bool resb;
        Type rest;
        try {
            rest = DoNot(p.second);
        } catch (const SemError &e) {
            LOG_ERROR(u, e);
            resb = false;
        }
        return std::make_pair(resb, rest);
    } else if (strcmp(op, "MINUS") == 0 && u->child->sibling == nullptr) {
        // unary minus
        std::pair<bool, Type> p = CheckExpression(u->child);
        bool resb = true;
        Type rest;
        try {
            rest = DoNeg(p.second);
        }
        catch (const SemError &e) {
            LOG_ERROR(u, e);
            resb = false;
        }
        return std::make_pair(resb, rest);
    } else { // binary operator
        std::pair<bool, Type>
            l = CheckExpression(u->child),
            r = CheckExpression(u->child->sibling);
        if (l.first == false || r.first == false) {
            return std::make_pair(true, Type::Void());
        }
        Type (*func)(const Type &, const Type &);
        if (strcmp(op, "GE") == 0
         || strcmp(op, "GT") == 0
         || strcmp(op, "LE") == 0
         || strcmp(op, "LT") == 0
         || strcmp(op, "EQUAL") == 0
         || strcmp(op, "UNEQUAL") == 0) {
            func = DoCmp;
        } else if (strcmp(op, "PLUS") == 0) {
            func = DoAdd;
        } else if (strcmp(op, "MINUS") == 0) {
            func = DoSub;
        } else if (strcmp(op, "OR") == 0) {
            func = DoOr;
        } else if (strcmp(op, "MUL") == 0) {
            func = DoMul;
        } else if (strcmp(op, "DIV") == 0) {
            func = DoDiv;
        } else if (strcmp(op, "MOD") == 0) {
            func = DoMod;
        } else if (strcmp(op, "AND") == 0) {
            func = DoAnd;
        }
        bool resb = true;
        Type rest;
        try {
            rest = func(l.second, r.second);
        } catch (const SemError &e) {
            LOG_ERROR(u, e);
            resb = false;
        }
        return std::make_pair(resb, rest);
    }
}

static bool CheckAssignStmt(const TreeNode *u) {
    std::cerr << "CheckAssignStmt" << std::endl;
    char *secondSonType = getKthChild(u, 2)->type;
    Type lvalType;
    Type lelem;
    TreeNode *rexp_ptr;
    std::pair<bool, Type> rexp;
    bool ret = true;
    try {
        lvalType = sym_t.GetVarType(u->child->vals);
    } catch (const SemError &e) {
        LOG_ERROR(u, e);
        ret = false;
    }
    if (strcmp(secondSonType, "ASSIGN") == 0) {
        lelem = lvalType;
        rexp_ptr = getKthChild(u, 3);
    } else {
        std::pair<bool, Type> t = CheckArrRec(u->child);
        if (t.first == false) {
            ret = false;
        }
        lelem = t.second;
        if (strcmp(u->child->sibling->type, "DOT") == 0) {
            rexp_ptr = getKthChild(u, 5);
        } else {
            rexp_ptr = getKthChild(u, 6);
        }
    }
    rexp = CheckExpression(rexp_ptr);
    if (rexp.first == false) {
        ret = false;
    } else if (ret) {
        try {
            DoAssign(lelem, rexp.second);
        } catch(const SemError &e) {
            LOG_ERROR(u, e);
            ret = false;
        }
    }
    return ret;
}

static bool CheckProcStmt(const TreeNode *u) {
    // TODO
    return true;
}

static bool CheckCompoundStmt(const TreeNode *u) {
    std::cerr <<  "CheckCompoundStmt" << std::endl;
    bool ret = true;
    for (TreeNode *p = u->child->child; p; p = p->sibling) {
        ret = CheckStmt(p) && ret;
    }
    return ret;
}

static bool CheckIfStmt(const TreeNode *u) {
    // TODO
    return true;
}

static bool CheckRepeatStmt(const TreeNode *u) {
    // TODO
    return true;
}

static bool CheckWhileStmt(const TreeNode *u) {
    // TODO
    return true;
}

static bool CheckForStmt(const TreeNode *u) {
    // TODO
    return true;
}

static bool CheckCaseStmt(const TreeNode *u) {
    // TODO
    return true;
}

static bool CheckGotoStmt(const TreeNode *u) {
    int id = u->child->vali;
    bool ret = sym_t.CheckLabel(id);
    if (ret) {
        LOG_ERROR(u, SemError("label not found"));
    }
    return ret;
}

static bool CheckNonLabelStmt(const TreeNode *u) {
    std::cerr << "CheckNonLabelStmt" << std::endl;
    if (strcmp(u->type, "assign_stmt") == 0) {
        return CheckAssignStmt(u);
    } else if (strcmp(u->type, "proc_stmt") == 0) {
        return CheckProcStmt(u);
    } else if (strcmp(u->type, "compound_stmt") == 0) {
        return CheckCompoundStmt(u);
    } else if (strcmp(u->type, "if_stmt") == 0) {
        return CheckIfStmt(u);
    } else if (strcmp(u->type, "repeat_stmt") == 0) {
        return CheckRepeatStmt(u);
    } else if (strcmp(u->type, "while_stmt") == 0) {
        return CheckWhileStmt(u);
    } else if (strcmp(u->type, "for_stmt") == 0) {
        return CheckForStmt(u);
    } else if (strcmp(u->type, "case_stmt") == 0) {
        return CheckCaseStmt(u);
    } else if (strcmp(u->type, "goto_stmt") == 0) {
        return CheckGotoStmt(u);
    }
    codeError();
}

static bool CheckStmt(const TreeNode *u)
{
    std::cerr << "CheckStmt" << std::endl;
    if (strcmp(u->child->type, "INTEGER") == 0)
    {
        try {
            sym_t.NeedLabel(u->child->vali);
        } catch (const SemError &e) {
            LOG_ERROR(u, e);
            return false;
        }
        return CheckNonLabelStmt(u->child->sibling);
    }
    bool ret;
    try {
        ret = CheckNonLabelStmt(u->child);
    } catch (const SemError &e) {
        LOG_ERROR(u, e);
        ret = false;
    }
    return ret;
}

static bool CheckRoutineBody(const TreeNode *u) {
    std::cerr << "CheckRoutineBody" << std::endl;
    return CheckNonLabelStmt(u->child);
}

static bool CheckRoutine(const TreeNode *u) {
    bool ret_head = CheckRoutineHead(u->child);
    bool ret_body = CheckRoutineBody(u->child->sibling);
    return ret_head && ret_body;
}

bool CheckSem(const TreeNode *u) {
    return CheckRoutine(u->child);
}

}
