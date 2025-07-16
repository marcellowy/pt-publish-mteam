//
// Created by chadwang on 2025/7/16.
//

#ifndef ASYNC_H
#define ASYNC_H

#include <future>
#include <utility>


namespace av::async {

class Exit {
public:
    Exit() = delete;
    Exit(std::function<void()> func) : func_(std::move(func)) {
    }
    ~Exit() {
        if (func_ != nullptr) {
            func_();
            func_ = nullptr;
        }
    }

    void cancel() {
        func_ = nullptr;
    }

private:
    std::function<void()> func_ = nullptr;
};
}
#endif //ASYNC_H
