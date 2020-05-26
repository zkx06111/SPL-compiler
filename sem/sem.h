#pragma once

#include "Type.h"
#include "tree.h"

namespace sem {

Type CheckTypeDecl(const TreeNode *u);
Type CheckSimpleTypeDecl(const TreeNode *u);

bool CheckSem(const TreeNode *u);

}