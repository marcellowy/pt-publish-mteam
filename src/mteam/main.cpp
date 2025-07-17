#include <av_string.h>
#include <iostream>
#include <sstream>
#include <filesystem>
#include "CLI11/CLI11.hpp"
#include "logger.h"
#include "av_http.h"

#define TOOLS_NAME "PT PUBLISH TOOLS"

struct CommandParams {
    std::string headers;                    // send header
    std::vector<std::string> file;          // files
    std::vector<std::string> screenshot;    // screenshot
    std::string data;                       // other param to website
};

static bool parseCommand(int argc, char **argv, CommandParams* params);
bool parseCommand(const int argc, char **argv, CommandParams* params) {
    CLI::App app(TOOLS_NAME);
    app.add_option("-f,--file",params->file, "");
    app.add_option("-s,--screenshot",params->screenshot, "");
    app.add_option("--data", params->data, "Form field, format query string");
    app.add_option("--headers", params->headers,"Http Header, format query string");
    CLI11_PARSE(app, argc, argv);

    // screenshot exists
    std::stringstream screenshotSS;
    for (auto & i : params->screenshot) {
        if (!std::filesystem::exists(i)) {
            LOG_WARN("{} not exists", i);
            return false;
        }
        screenshotSS << i << ",";
    }
    if (params->screenshot.empty()) {
        LOG_WARN("screenshot is empty");
        return false;
    }

    // file
    std::stringstream fileSS;
    for (auto & i : params->file) {
        if (!std::filesystem::exists(i)) {
            LOG_WARN("{} not exists", i);
            return false;
        }
        fileSS << i << ",";
    }
    if (params->file.empty()) {
        LOG_WARN("file is empty");
        return false;
    }

    // kv
    std::stringstream kvSS;
    for (auto & i : params->data) {
        kvSS << i << ",";
    }
    if (params->data.empty()) {
        LOG_WARN("kv is empty");
        return false;
    }

    // headers
    std::stringstream headerSS;
    for (auto & i : params->headers) {
        headerSS << i << ",";
    }

    LOG_INFO("file: {}\n"
             "screenshot: {}\n"
             "kv: {}\n"
             "headers: {}\n",
             fileSS.str(),
             screenshotSS.str(),
             kvSS.str(),
             headerSS.str());
    return true;
}

int main(const int argc, char **argv){
    Logger::init("publish", "log/publish.log");
    LOG_INFO("{} start", TOOLS_NAME);
    CommandParams params;
    if (!parseCommand(argc, argv, &params)) {
        std::cout << "parse command failed" << std::endl;
        return 255;
    }

    auto header = av::string::parseQueryString(params.headers);
    for (const auto& i : header) {
        LOG_INFO("{}={}",i.first,i.second);
    }

    // LOG_INFO("{}", av::string::urlDecode("http%3A%2F%2F89.58.40.232%3A38800%2F%23%2FconfigurationManagement%3FdataId%3D%26group%3D%26appName%3D%26namespace%3D%26pageSize%3D%26pageNo%3D"));

    // std::string body = av::http::Client::get("https://www.baidu.com");
    // LOG_INFO("body: {}", body);
    // av::http::Client c;
    // av::http::Request req = {};
    // req.url = "https://www.baidu.com";
    // LOG_INFO("{} start", TOOLS_NAME);;
    // try {
    //     const std::unique_ptr<av::http::Response> res = c.send(&req);
    //     LOG_INFO("http version: {}", res->body);
    // } catch (std::exception &e) {
    //     LOG_ERROR("{}", e.what());
    //     return 0;
    // }

    std::cout << "success\n";
}