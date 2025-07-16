//
// Created by chadwang on 2025/7/16.
//


#include <logger.h>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include "av_async.h"
#include <vector>
#include <sstream>
#include "av_string.h"
#include "av_http.h"

static size_t writeFunctionCallback(void* contents, const size_t size, size_t nmemb, void* userp);
static size_t headerFunctionCallback(char* buffer, const size_t size, size_t nitems, void* userdata);
static std::pair<std::string, std::string> splitHeader(const std::string& str, const std::string& delimiter);

av::http::Client::Client() {
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

av::http::Client::~Client() {
    curl_global_cleanup();
}

std::unique_ptr<av::http::Response> av::http::Client::send(const Request *request) {
    CURL* curl = curl_easy_init();
    if (!curl) throw std::runtime_error("Failed to initialize curl");
    async::Exit exit([&curl]() {
        curl_easy_cleanup(curl);
    });

    // set headers
    curl_slist* headersInternal = nullptr;
    if (!request->headers.empty()) {
        for (const auto&[fst, snd] : request->headers) {
            std::string header_kv(fst);
            header_kv.append(": ");
            header_kv.append(snd);
            headersInternal = curl_slist_append(headersInternal, header_kv.c_str());
        }
    }

    std::unordered_map<std::string, std::string> responseHeaders = {};
    auto response = std::make_unique<Response>();
    std::vector<std::string> headers;

    // set opt
    curl_easy_setopt(curl, CURLOPT_URL, request->url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headersInternal);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunctionCallback);
    if (request->timeout != nullptr) {
        curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, request->timeout->read_timeout_ms);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, request->timeout->connect_timeout_ms);
    }
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response->body);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, headerFunctionCallback);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &headers);
    if (request->method == Post) {
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request->body.c_str());
    }


    // proxy settings
    if (request->proxy != nullptr) {
        std::string proxy_user(request->proxy->username);
        proxy_user.append(":");
        proxy_user.append(request->proxy->password);
        curl_easy_setopt(curl, CURLOPT_PROXY, (request->proxy->proxy).c_str());
        curl_easy_setopt(curl, CURLOPT_PROXYUSERPWD, proxy_user.c_str());
    }

    // send data
    if (const CURLcode code = curl_easy_perform(curl); code != CURLE_OK) {
        throw std::runtime_error("curl_easy_perform() failed: " + std::to_string(code));
    }

    // get response status code
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response->statusCode);

    // parse response headers
    int count = 0;
    auto httpVersion = headers[0];
    {
        httpVersion.pop_back();
        httpVersion.pop_back();
        std::istringstream iss(httpVersion);
        std::string word;
        std::vector<std::string> result;
        while (iss >> word) {  // 自动跳过多个空格和换行
            result.push_back(word);
        }
        if (!result.empty()) response->version = result[0];
    }
    for (auto& line : headers) {
        if (count == 0) {
            count++;
            continue;
        }
        count++;
        // remove \n
        line.pop_back();
        // remove \r
        line.pop_back();
        if (line.empty()) continue;
        const auto [fst, snd] = splitHeader(line,": ");
        if (fst.empty()) {
            continue;
        }
        response->headers[fst] = snd;
    }
    return response;
}

std::pair<std::string, std::string> splitHeader(const std::string& str, const std::string& delimiter) {
    std::pair<std::string, std::string> tokens;
    size_t start = 0;
    size_t end;
    auto newStr = str;

    if (delimiter.empty() || newStr.find(delimiter,0) == std::string::npos) return tokens;

    // find delimiter
    while ((end = newStr.find(delimiter, start)) != std::string::npos) {
        tokens.first = newStr.substr(start,end);
        tokens.second = av::string::trim( newStr.substr(end+1,str.size()));
        start = start+newStr.length();
    }
    return tokens;
}

size_t writeFunctionCallback(void* contents, const size_t size, size_t nmemb, void* userp) {
    static_cast<std::string *>(userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

size_t headerFunctionCallback(char* buffer, const size_t size, size_t nitems, void* userdata) {
    size_t totalSize = size * nitems;
    auto* headers = static_cast<std::vector<std::string>*>(userdata);
    headers->emplace_back(buffer, totalSize);
    return totalSize;
}