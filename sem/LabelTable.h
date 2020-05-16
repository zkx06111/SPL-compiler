#pragma once

#include <unordered_set>

namespace sem {

class LabelTable {
  public:
    bool Check(int i) const;
    void Insert(int i);

  private:
    std::unordered_set<int> labels;
};

}