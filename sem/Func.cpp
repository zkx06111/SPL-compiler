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

}