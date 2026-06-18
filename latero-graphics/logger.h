#pragma once

#include <boost/thread.hpp>
#include <memory>
#include <boost/thread/condition.hpp>
#include <queue>
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
    Logger(std::string filename="logger.txt");
    virtual ~Logger();
	
    void Start();
    void Stop();

    void Write(std::string str, bool wait=false);
    void SetNewFilename(std::string filename);
    
    long GetTimestamp() const { return m_timer.GetElapsed().total_milliseconds(); }
    
private:
    void Loop();
	
private:
    volatile bool m_stopRequested;
    std::shared_ptr<boost::thread> m_thread;
	boost::mutex m_mutex;
    boost::condition m_cond;
    std::queue<Message> m_fifo;
    std::string m_filename;
    Timer m_timer;
};

} // namespace

