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

#ifndef LATERO_GRAPHICS_LOGGER_H
#define LATERO_GRAPHICS_LOGGER_H

#include <boost/thread.hpp>
#include <boost/thread/condition.hpp>
#include <queue>
#include "timer.h"

namespace latero {
namespace graphics {
    
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
    boost::shared_ptr<boost::thread> m_thread;
	boost::mutex m_mutex;
    boost::condition m_cond;
    std::queue<Message> m_fifo;
    std::string m_filename;
    Timer m_timer;
};

} // namespace graphics
} // namespace latero

#endif