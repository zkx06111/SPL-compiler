#include "sem.h"

#include <iostream>

#include "SemError.h"
#include "SymbolTable.h"

#define LOG_ERROR(u, e) do { \
    std::cout << "line " << u->lineNumber << ": " << e.what() << std::endl; \
} while (0)

namespace sem {

static bool CheckLabelList(const TreeNode *u) {
    auto v = u->child;
    if (strcmp(v->type, "label_list")) {
        int i = v->sibling->vali;
        bool ret = true;
        try { sym_t.NewLabel(i); } catch (const SemError &e) {
            LOG_ERROR(v->sibling, e);
            ret = false;
        }
        bool ret_t = CheckLabelList(v);
        return ret && ret_t;
    } else {
        int i = v->vali;
        try { sym_t.NewLabel(i); } catch (const SemError &e) {
            LOG_ERROR(v, e);
            return false;
        }
        return true;
    }
}

static bool CheckLabelPart(const TreeNode *u) {
    if (u->child == nullptr) {
        return true;
    } else {
        return CheckLabelList(u->child);
    }
}

static bool CheckConstList(const TreeNode *u) {
    auto v = u->child;
    if (strcmp(v->type, "const_expr_list") == 0) {

    } else {
        
    }
}

static bool CheckConstPart(const TreeNode *u) {
    if (u->child == nullptr) {
        return true;
    } else {
        return CheckConstList(u->child->sibling);
    }
}

static bool CheckTypePart(const TreeNode *u) {

}

static bool CheckVarPart(const TreeNode *u) {

}

static bool CheckRoutinePart(const TreeNode *u) {

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

}

static bool CheckRoutine(const TreeNode *u) {
    bool ret_head = CheckRoutineHead(u->child);
    bool ret_body = CheckRoutineBody(u->child->sibling);
    return ret_head && ret_body;
}

bool CheckSem(const TreeNode *u) {
    if (strcmp(u->type, "program") == 0) {
        return CheckRoutine(u->child->sibling);
    }
}

}
