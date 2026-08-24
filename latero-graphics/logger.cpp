#include "logger.h"
#include <iostream>
#include <fstream>
#include <math.h>
//#include "config.h"

namespace latero::graphics {

Logger::Logger(const std::filesystem::path& filename) :
	m_stopRequested(false), m_filename(filename)
{
}
	
Logger::~Logger()
{
}

void Logger::Start() 
{
	assert(!m_thread);
    m_timer.Reset();
	m_thread = std::make_shared<std::thread>(&Logger::Loop, this);
    // todo: see if the priority of this thread can be decreased
}

void Logger::Stop()
{
	assert(m_thread);
	m_stopRequested = true;
    m_cond.notify_all();    
	m_thread->join();
}

void Logger::Write(std::string str, bool wait)
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        Message msg;
        msg.str = str;
        msg.code = code_print;
        m_fifo.push(msg);
    }
    if (!wait) m_cond.notify_all();
}
	
void Logger::SetNewFilename(const std::filesystem::path& filename)
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        Message msg;
        msg.str = filename.string();
        msg.code = code_new_filename;
        m_fifo.push(msg);
    }
    m_cond.notify_all();
}

void Logger::Loop()
{
	std::ofstream myfile;
	myfile.open(m_filename.c_str());
    // todo... boost::filesystem::exists( "myfile.txt" )
    
	while (!m_stopRequested)
	{
        Message msg;
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_cond.wait(lock, [this]{ return m_fifo.size() > 0 || m_stopRequested; });

            if (m_fifo.size())
            {
                msg =  m_fifo.front();
                m_fifo.pop();
            }
        }
        
        if ((msg.code == code_print) && (msg.str != ""))
        {
            myfile << msg.str;
        }
        else if (msg.code == code_new_filename)
        {
            m_filename = msg.str;
            myfile.close();
            myfile.open(m_filename.c_str());
        }
	}

	myfile.close();
}    

} // namespace

