#include "Func.h"

#include "TypeUtil.h"

namespace sem {

bool Func::CanCall(const std::vector<Type> &passed_args) const {
    if (args.size() != passed_args.size()) {
        return false;
    }
    for (int i = 0; i < args.size(); i++) {
        if (!CanAssign(args[i], passed_args[i])) {
            return false;
        }
    }
    return true;
}

void Func::ApplyArgs(const std::vector<Type> &passed_args) const {
    if (args.size() != passed_args.size()) {
        throw SemError("need " + std::to_string(args.size()) + " parameters, " +
            "found " + std::to_string(passed_args.size()) + " parameters");
    }
    for (int i = 0; i < args.size(); i++) {
        if (!CanAssign(args[i], passed_args[i])) {
            throw SemError("can't assign the " + OrderStr(i + 1) + " parameter");
        }
    }
}

}