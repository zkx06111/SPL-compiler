#pragma once

#include <unordered_set>

namespace sem {

class LabelTable {
  public:
    bool Check(int i) const;
    bool CheckDecl(int i) const;
    void Insert(int i);
    void Declare(int i);
    void Goto(int i);
    bool CheckAll() const;

  private:
    std::unordered_set<int> labels;
    std::unordered_set<int> decls;
    std::unordered_set<int> gotos;
};

}