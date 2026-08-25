#pragma once

#include <condition_variable>
#include <filesystem>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include "timer.h"

namespace latero::graphics {
    
class Logger
{
protected:
    typedef struct {
        std::string str;
        int code;
    } Message;
    enum { code_print, code_new_filename };
    
public:
    Logger(const std::filesystem::path& filename = "logger.txt");
    virtual ~Logger();
	
    void Start();
    void Stop();

    void Write(std::string str, bool wait=false);
    void SetNewFilename(const std::filesystem::path& filename);
    
    long GetTimestamp() const { return timer_.GetElapsed().count() / 1000; }
    
private:
    void Loop();
	
private:
    volatile bool stopRequested_;
    std::shared_ptr<std::thread> thread_;
	std::mutex mutex_;
    std::condition_variable cond_;
    std::queue<Message> fifo_;
    std::filesystem::path filename_;
    Timer timer_;
};

} // namespace

