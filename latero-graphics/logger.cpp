#include "logger.h"
#include <cassert>
#include <iostream>
#include <fstream>
#include <math.h>
//#include "config.h"

namespace latero::graphics {

Logger::Logger(const std::filesystem::path& filename) :
	stopRequested_(false), filename_(filename)
{
}
	
Logger::~Logger()
{
}

void Logger::Start() 
{
	assert(!thread_);
    timer_.Reset();
	thread_ = std::make_shared<std::thread>(&Logger::Loop, this);
    // todo: see if the priority of this thread can be decreased
}

void Logger::Stop()
{
	assert(thread_);
	stopRequested_ = true;
    cond_.notify_all();    
	thread_->join();
}

void Logger::Write(std::string str, bool wait)
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        Message msg;
        msg.str = str;
        msg.code = code_print;
        fifo_.push(msg);
    }
    if (!wait) cond_.notify_all();
}
	
void Logger::SetNewFilename(const std::filesystem::path& filename)
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        Message msg;
        msg.str = filename.string();
        msg.code = code_new_filename;
        fifo_.push(msg);
    }
    cond_.notify_all();
}

void Logger::Loop()
{
	std::ofstream myfile;
	myfile.open(filename_.c_str());
    // todo... boost::filesystem::exists( "myfile.txt" )
    
	while (!stopRequested_)
	{
        Message msg;
        {
            std::unique_lock<std::mutex> lock(mutex_);
            cond_.wait(lock, [this]{ return fifo_.size() > 0 || stopRequested_; });

            if (fifo_.size())
            {
                msg =  fifo_.front();
                fifo_.pop();
            }
        }
        
        if ((msg.code == code_print) && (msg.str != ""))
        {
            myfile << msg.str;
        }
        else if (msg.code == code_new_filename)
        {
            filename_ = msg.str;
            myfile.close();
            myfile.open(filename_.c_str());
        }
	}

	myfile.close();
}    

} // namespace

