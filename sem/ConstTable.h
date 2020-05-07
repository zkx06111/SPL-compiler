#pragma once

#include <unordered_map>

#include "Type.h"

namespace sem {

struct Const {
    Const();
    ~Const();
    Const(const Const &rhs);
    Const &operator=(const Const &rhs);

    Type type;
    union {
        int val_i;
        double val_r;
        bool val_b;
        char val_c;
        std::string val_s;
    };
};

class ConstTable {
  public:
    void AddConst(const std::string &name, int val);
    void AddConst(const std::string &name, double val);
    void AddConst(const std::string &name, bool val);
    void AddConst(const std::string &name, char val);
    void AddConst(const std::string &name, const std::string &val);
    void AddConst(const std::string &name, const Type &type, const Enum &enm);

    bool CheckConst(const std::string &name) const;
    bool CheckEnum(const std::string &name) const;
    Type GetConstType(const std::string &name) const;

    int GetConstValI(const std::string &name) const;
    double GetConstValR(const std::string &name) const;
    bool GetConstValB(const std::string &name) const;
    char GetConstValC(const std::string &name) const;
    std::string GetConstValS(const std::string &name) const;

  private:
    std::unordered_map<std::string, Const> consts;
};

}