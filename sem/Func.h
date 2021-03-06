#pragma once

#include "Type.h"

namespace sem {

struct Func {
    bool CanCall(const std::vector<Type> &passed_args) const;
    void ApplyArgs(const std::vector<Type> &passed_args) const;

    Type ret;
    std::vector<Type> args;
    std::vector<int> mut_args;
};

}