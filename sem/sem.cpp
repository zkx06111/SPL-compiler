#include "sem.h"

#include <iostream>
#include <limits>

#include "SemError.h"
#include "SymbolTable.h"

#define LOG_ERROR(u, e) do { \
    std::cout << "line " << u->lineNumber << ": " << e.what() << std::endl; \
} while (0)

namespace sem {

static std::vector<std::string> CheckNameList(const TreeNode *u) {
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

static Type CheckSimpleTypeDecl(const TreeNode *u) {
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

static Type CheckTypeDecl(const TreeNode *u);

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

static Type CheckTypeDecl(const TreeNode *u) {
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

static bool CheckRoutineBody(const TreeNode *u) {
    // TODO
    return true;
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
