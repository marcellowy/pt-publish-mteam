//
// Created by Marcello on 2025/7/16.
//
#include "av_string.h"
std::string av::string::trim(std::string_view sv) {
    while (!sv.empty() && std::isspace(static_cast<unsigned char>(sv.front()))) {
        sv.remove_prefix(1);
    }
    while (!sv.empty() && std::isspace(static_cast<unsigned char>(sv.back()))) {
        sv.remove_suffix(1);
    }
    auto result = std::string(sv);
    return result;
}

