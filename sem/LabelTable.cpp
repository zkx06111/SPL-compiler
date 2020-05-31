#include "LabelTable.h"

namespace sem {

bool LabelTable::Check(int i) const {
    return labels.count(i) != 0;
}

bool LabelTable::CheckDecl(int i) const {
    return decls.count(i) != 0;
}

void LabelTable::Insert(int i) {
    labels.insert(i);
}

void LabelTable::Declare(int i) {
    decls.insert(i);
}

void LabelTable::Goto(int i) {
    gotos.insert(i);
}

bool LabelTable::CheckAll() const {
    for (int i : gotos) {
        if (decls.count(i) == 0) {
            return false;
        }
    }
    return true;
}

}