#include "Func.h"

#include "TypeUtil.h"

namespace sem {

bool Func::CanCall(const std::vector<Type> &passed_args) const {
    if (args.size() != passed_args.size()) {
        return false;
    }
    for (int i = 0, j = 0; i < args.size(); i++) {
        if (j < mut_args.size() && i == mut_args[j]) {
            if (!passed_args[i].is_lhs) {
                return false;
            }
            if (!IsAlmostSame(passed_args[i], args[i])) {
                return false;
            }
            ++j;
        }
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
    for (int i = 0, j = 0; i < args.size(); i++) {
        if (j < mut_args.size() && i == mut_args[j]) {
            if (!passed_args[i].is_lhs) {
                throw SemError("the " + OrderStr(i + 1) +
                    " parameter is variable and need an lval");
            }
            if (!IsAlmostSame(passed_args[i], args[i])) {
                throw SemError(
                    "can't assign the " + OrderStr(i + 1) + " parameter");
            }
            ++j;
        }
        if (!CanAssign(args[i], passed_args[i])) {
            throw SemError("can't assign the " + OrderStr(i + 1) + " parameter");
        }
    }
}

}