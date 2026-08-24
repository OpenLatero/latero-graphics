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
    
    long GetTimestamp() const { return m_timer.GetElapsed().total_milliseconds(); }
    
private:
    void Loop();
	
private:
    volatile bool m_stopRequested;
    std::shared_ptr<std::thread> m_thread;
	std::mutex m_mutex;
    std::condition_variable m_cond;
    std::queue<Message> m_fifo;
    std::filesystem::path m_filename;
    Timer m_timer;
};

} // namespace

