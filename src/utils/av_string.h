//
// Created by Marcello on 2025/7/16.
//

#ifndef AV_STRING_H
#define AV_STRING_H
#include <string>
#include <string_view>
#include <unordered_map>

namespace av::string {
    std::string trim(std::string_view sv);
    std::string urlDecode(const std::string& value);
    std::unordered_map<std::string, std::string> parseQueryString(const std::string& query);
}

#endif //AV_STRING_H
