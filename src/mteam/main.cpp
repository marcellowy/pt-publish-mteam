#include <iostream>
#include <filesystem>
#include "CLI11/CLI11.hpp"
#include "logger.h"
#include "http.h"


#define TOOLS_NAME "PT PUBLISH TOOLS"


struct CommandParams {
    std::string file; // 种子文件
    std::string nameEn; // 影片英文名,标题的一部分
    std::string subTitle; // 副标题
    int categoryId{0}; // 分类id
    std::string douBanId; //  豆瓣id
    std::string season; // 季集, 类似 S01E09
    int sourceId{0}; // 来源id
    std::string sourceName; // 来源名称
    int teamId{0}; // 组id
    std::string teamName; // 组名称
    std::string titlePrefix; // 名称前缀; 一般是TV放电视台名称,如 CCTV-16

    // video
    std::string resolutionDesc; // 1080i/1080p/2160p
    int resolutionId{0};
    std::string videoCodec; // H.264/H.265/HEVC/AVS
    int videoCodecId{0};
    std::string audioCodec; // AC3/MP3/AC3(DD)
    int audioCodecId{0};
    std::vector<std::string> screenshot; // 截图
    std::vector<std::string> kv; // other param to website
};

static bool parseCommand(int argc, char **argv, CommandParams* params);
bool parseCommand(const int argc, char **argv, CommandParams* params) {
    CLI::App app(TOOLS_NAME);
    app.add_option("-f,--file", params->file, "Torrent file");
    app.add_option("--english-name", params->nameEn, "English name");
    app.add_option("--sub-title", params->subTitle, "Subtitle");
    app.add_option("--category-id", params->categoryId, "Category id");
    app.add_option("--douban-id",params->douBanId,"Douban ID");
    app.add_option("--season",params->season,"Season description");
    app.add_option("--source-id",params->sourceId,"Source ID");
    app.add_option("--source-name", params->sourceName, "Source name");
    app.add_option("--team-id",params->teamId,"Team id");
    app.add_option("--team-name", params->teamName, "Team name");
    app.add_option("--title-prefix",params->titlePrefix,"Title prefix");
    app.add_option("--screenshot", params->screenshot,"Screenshot file");
    app.add_option("--resolution-desc", params->resolutionDesc, "Resolution description");
    app.add_option("--resolution-id", params->resolutionId, "Resolution ID");
    app.add_option("--video-codec", params->videoCodec, "Video codec description");
    app.add_option("--video-codec-id", params->videoCodecId, "Video codec ID");
    app.add_option("--audio-codec", params->audioCodec, "Audio codec description");
    app.add_option("--audio-codec-id", params->audioCodecId, "Audio codec ID");
    app.add_option("--kv", params->kv, "Form field");
    CLI11_PARSE(app, argc, argv);

    // if(params->file.empty()) {
    //     LOG_WARN("-f,--file empty");
    //     return false;
    // }
    // if (!std::filesystem::exists(params->file)) {
    //     LOG_WARN("-f,--file {} not exists", params->file);
    //     return false;
    // }
    // if(params->nameEn.empty()) {
    //     LOG_WARN("--english-name empty");
    //     return false;
    // }
    // if(params->subTitle.empty()) {
    //     LOG_WARN("--sub-title empty");
    //     return false;
    // }
    // if(params->categoryId == 0) {
    //     LOG_WARN("--category-id == 0");
    //     return false;
    // }
    // if (params->douBanId.empty()) {
    //     LOG_WARN("--douban-id empty");
    //     return false;
    // }
    // if (params->season.empty()) {
    //     LOG_WARN("--season empty");
    //     return false;
    // }
    // if (params->sourceId == 0) {
    //     LOG_WARN("--source-id == 0");
    //     return false;
    // }
    // if (params->sourceName.empty()) {
    //     LOG_WARN("--source-name empty");
    //     return false;
    // }
    // if(params->teamId == 0) {
    //     LOG_WARN("--team-id == 0");
    //     return false;
    // }
    // if (params->teamName.empty()) {
    //     LOG_WARN("--teamName empty");
    //     return false;
    // }
    // if (params->screenshot.empty()) {
    //     LOG_WARN("--screenshot empty");
    // }

    // screenshot exists
    std::stringstream ss;
    for (auto & i : params->screenshot) {
        if (!std::filesystem::exists(i)) {
            LOG_WARN("{} not exists", i);
            return false;
        }
        ss << i << ",";
    }
    LOG_INFO("Torrent File: {}\n"
             "English Name: {}\n"
             "Subtitle: {}\n"
             "Category ID: {}\n"
             "DouBan ID: {}\n"
             "Season: {}\n"
             "Season Name: {}\n"
             "Source ID: {}\n"
             "Team ID: {}\n"
             "Team Name: {}\n"
             "Title Prefix: {}\n"
             "Screenshot: {}\n",
             params->file,
             params->nameEn,
             params->subTitle,
             params->categoryId,
             params->douBanId,
             params->season,
             params->sourceId,
             params->sourceName,
             params->teamId,
             params->teamName,
             params->titlePrefix,
             ss.str());

    for (auto & i : params->kv) {
        LOG_INFO("{} ", i);
    }

    return true;
}

int main(const int argc, char **argv){
    Logger::init("publish", "log/publish.log");
    LOG_INFO("{} start", TOOLS_NAME);
    CommandParams params;
    if (!parseCommand(argc, argv, &params)) {
        std::cout << "parse command failed" << std::endl;
        return -1;
    }
    // std::string body = av::http::Client::get("https://www.baidu.com");
    // LOG_INFO("body: {}", body);
    av::http::Client c;
    av::http::Request req = {};
    req.url = "https://www.baidu.com";
    LOG_INFO("{} start", TOOLS_NAME);;
    try {
        const std::unique_ptr<av::http::Response> res = c.send(&req);
        LOG_INFO("code: {}, statusCode: {}", res->code, res->statusCode);
    } catch (std::exception &e) {
        LOG_ERROR("{}", e.what());
        return 0;
    }

    std::cout << "success\n";
}