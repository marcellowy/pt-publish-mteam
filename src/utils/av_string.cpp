#include <string>
#include <sstream>
#include <iomanip>
#include <cctype>
#include "av_string.h"
#include <unordered_map>
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

std::string av::string::urlDecode(const std::string& value) {
    std::ostringstream decoded;
    size_t i = 0;
    while (i < value.length()) {
        if (const char c = value[i]; c == '%') {
            if (i + 2 < value.length() &&
                std::isxdigit(value[i + 1]) &&
                std::isxdigit(value[i + 2])) {
                std::string hex = value.substr(i + 1, 2);
                const char decoded_char = static_cast<char>(std::stoi(hex, nullptr, 16));
                decoded << decoded_char;
                i += 3;
            } else {
                decoded << '%';
                ++i;
            }
        } else if (c == '+') {
            decoded << ' ';
            ++i;
        } else {
            decoded << c;
            ++i;
        }
    }
    return decoded.str();
}

std::unordered_map<std::string, std::string> av::string::parseQueryString(const std::string& query) {
    std::unordered_map<std::string, std::string> params;
    std::istringstream ss(query);
    std::string pair;

    while (std::getline(ss, pair, '&')) {
        if (size_t pos = pair.find('='); pos != std::string::npos) {
            std::string key = urlDecode(pair.substr(0, pos));
            std::string value = urlDecode(pair.substr(pos + 1));
            params[key] = value;
        } else {
            // 只有 key 没有 value（如 ?flag），可根据需求处理
            std::string key = urlDecode(pair);
            params[key] = "";
        }
    }

    return params;
}