#include "IdTable.h"

namespace sem {

bool IdTable::Check(const std::string &name) const {
    return names.count(name) != 0;
}

bool IdTable::Insert(const std::string &name) {
    if (names.count(name) == 0) {
        names.insert(name);
        return true;
    } else {
        return false;
    }
}

}