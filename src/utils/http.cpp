//
// Created by chadwang on 2025/7/16.
//

#include "http.h"

#include <logger.h>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include "async.h"
#include <vector>


av::http::Client::Client() {
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

av::http::Client::~Client() {
    curl_global_cleanup();
}

size_t av::http::Client::writeCallback(void* contents, const size_t size, size_t nmemb, void* userp) {
    static_cast<std::string *>(userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

size_t header_callback(char* buffer, const size_t size, size_t nitems, void* userdata) {
    size_t totalSize = size * nitems;
    auto* headers = static_cast<std::vector<std::string>*>(userdata);
    headers->emplace_back(buffer, totalSize);
    return totalSize;
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
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, request->timeout->read_timeout_ms);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, request->timeout->connect_timeout_ms);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response->body);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_callback);
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
    LOG_INFO("req");
    if (const CURLcode code = curl_easy_perform(curl); code != CURLE_OK) {
        response->code = code;
        return response;
    }
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response->statusCode);
    for (const auto& line : headers) {
        LOG_INFO("header: {}", line);
    }
    return response;
}