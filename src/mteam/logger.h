//
// Created by chadwang on 2025/1/10.
//

#ifndef LOGGER_H
#define LOGGER_H

#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#define LOGGER_TRACE(logger,...)     SPDLOG_LOGGER_TRACE(logger, __VA_ARGS__)
#define LOGGER_DEBUG(logger,...)     SPDLOG_LOGGER_DEBUG(logger, __VA_ARGS__)
#define LOGGER_INFO(logger,...)      SPDLOG_LOGGER_INFO(logger, __VA_ARGS__)
#define LOGGER_WARN(logger,...)      SPDLOG_LOGGER_WARN(logger, __VA_ARGS__)
#define LOGGER_ERROR(logger,...)     SPDLOG_LOGGER_ERROR(logger, __VA_ARGS__)
#define LOGGER_CRITICAL(logger,...)  SPDLOG_LOGGER_CRITICAL(logger, __VA_ARGS__)

#define LOG_TRACE(...)     SPDLOG_TRACE(__VA_ARGS__)
#define LOG_DEBUG(...)     SPDLOG_DEBUG(__VA_ARGS__)
#define LOG_INFO(...)      SPDLOG_INFO(__VA_ARGS__)
#define LOG_WARN(...)      SPDLOG_WARN(__VA_ARGS__)
#define LOG_ERROR(...)     SPDLOG_ERROR(__VA_ARGS__)
#define LOG_CRITICAL(...)  SPDLOG_CRITICAL(__VA_ARGS__)

#define DEFAULT_LOG_PATTERN "[%Y-%m-%d %H:%M:%S.%e] %^[%l]%$ [%P:%t] [%s %!:%#] %v"

namespace Logger {
    inline std::shared_ptr<spdlog::logger> newLog(const std::string &name, const std::string &filename) {
        auto logger = spdlog::get(name);
        if (logger == nullptr) {
            logger = spdlog::rotating_logger_mt(name, filename, 1024 * 1024 * 5, 10);
            logger->set_level(spdlog::level::trace);
            logger->flush_on(spdlog::level::err);
            logger->set_pattern(DEFAULT_LOG_PATTERN);
        }
        return logger;
    }

    inline void init(const std::string &name, const std::string &filename) {
        spdlog::init_thread_pool(8192, 1);
        spdlog::flush_every(std::chrono::milliseconds(100));
        const auto rotating_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(filename, 1048576 * 5, 3);
        const auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        std::vector<spdlog::sink_ptr> sinks {console_sink, rotating_sink};
        const auto logger = std::make_shared<spdlog::logger>("multi_logger", sinks.begin(), sinks.end());
        logger->set_level(spdlog::level::trace);
        logger->flush_on(spdlog::level::err);
        logger->set_pattern(DEFAULT_LOG_PATTERN);
        set_default_logger(logger);
    }
}

#endif //LOGGER_H
