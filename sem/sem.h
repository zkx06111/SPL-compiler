#pragma once

#include "Type.h"
#include "tree.h"

namespace sem {

std::vector<std::string> CheckNameList(const TreeNode *u);
Type CheckTypeDecl(const TreeNode *u);
Type CheckSimpleTypeDecl(const TreeNode *u);

std::pair<std::vector<std::pair<std::string, Type>>, std::vector<int>>
CheckParametersDecl(const TreeNode *u);

bool CheckSem(const TreeNode *u, const std::string &file_name);

}