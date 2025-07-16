//
// Created by chadwang on 2025/7/16.
//

#ifndef HTTP_H
#define HTTP_H

// HttpClient.h
#pragma once

#include <string>
#include <curl/curl.h>
#include <unordered_map>
#include <memory>

namespace av::http {

    class Proxy {
    public:
        std::string proxy;
        std::string username;
        std::string password;
    };

    class Timeout {
    public:
        int connect_timeout_ms = 10000;
        int read_timeout_ms = 10000;
    };

    enum Method {
        Get,
        Post
    };

    class Base {
    public:
        std::unordered_map<std::string, std::string> headers = {}; // http response headers
        std::string body; // http body
    };
    class Request: public Base {
    public:
        std::string url;
        Method method = Method::Get;
        Proxy* proxy = nullptr;
        Timeout* timeout = nullptr;
    };
    class Response: public Base{
    public:
        int code = 0; // curl error code
        int statusCode = 0; // http status code
    };

    class Client {
    public:
        Client();
        ~Client();
        std::unique_ptr<Response> send(const Request* request);
    private:
        static size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userp);
    };
}



#endif //HTTP_H
