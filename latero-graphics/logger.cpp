// -----------------------------------------------------------
//
// Copyright (c) 2015 by Vincent Levesque. All Rights Reserved.
//
// This file is part of latero-graphics.
//
//    latero-graphics is free software: you can redistribute it and/or modify
//    it under the terms of the GNU Lesser General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    latero-graphics is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public License
//    along with latero-graphics.  If not, see <http://www.gnu.org/licenses/>.
//
// -----------------------------------------------------------

#include "logger.h"
#include <iostream>
#include <fstream>
#include <math.h>
//#include "config.h"

namespace latero {
namespace graphics {

Logger::Logger(std::string filename) : 
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
	m_thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&Logger::Loop, this)));
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
        boost::mutex::scoped_lock lock(m_mutex);
        Message msg;
        msg.str = str;
        msg.code = code_print;
        m_fifo.push(msg);
    }
    if (!wait) m_cond.notify_all();
}
	
void Logger::SetNewFilename(std::string filename)
{
    {
        boost::mutex::scoped_lock lock(m_mutex);
        Message msg;
        msg.str = filename;
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
            boost::mutex::scoped_lock lock(m_mutex);
            while ((m_fifo.size()==0) && !m_stopRequested)
                m_cond.wait(lock);

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

} // namespace graphics
} // namespace latero

