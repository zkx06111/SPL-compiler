#pragma once

#include <unordered_set>
#include <string>

namespace sem {

class IdTable {
  public:
    bool Check(const std::string &name) const;
    bool Insert(const std::string &name);

  private:
    std::unordered_set<std::string> names;
};

}