#include "SemError.h"

namespace sem {

SemError::SemError(const std::string &info) {
    this->info = info;
}

const char *SemError::what() const noexcept {
    return info.c_str();
}

std::string OrderStr(int i) {
    int mod = i % 10;
    if (mod == 1 && i != 11) {
        return std::to_string(i) + "st";
    } else if (mod == 2 && i != 12) {
        return std::to_string(i) + "nd";
    } else if (mod == 3 && i != 13) {
        return std::to_string(i) + "rd";
    } else {
        return std::to_string(i) + "th";
    }
}

}
