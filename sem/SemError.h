#pragma once

#include <exception>
#include <string>

namespace sem {

class SemError : public std::exception {
  public:
    SemError(const std::string &info);

    const char *what() const noexcept override;

  private:
    std::string info;
};

std::string OrderStr(int i);

}