#include "LabelTable.h"

namespace sem {

bool LabelTable::Check(int i) const {
    return labels.count(i) != 0;
}

void LabelTable::Insert(int i) {
    labels.insert(i);
}

}