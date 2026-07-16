
/*
    日志消息类
    1.日志时间
    2.日志等级
    3.线程id
    4.日志产生文件
    5.日志行号
    6.日志器
    7。日志主题消息
*/
#ifndef __MESSAGE_HPP_
#define __MESSAGE_HPP_

#include<iostream>
#include<string>
#include<thread>
#include"level.hpp"
#include"util.hpp"

namespace mylog
{
    class LogMsg
    {
        public:
        time_t _ctime;
        LogLevel::value _level;
        std::thread::id _cid;
        std::string _file;
        size_t _line;
        std::string _logger;
        std::string _payLoad;

        LogMsg(LogLevel::value level,const std::string file,size_t line,
            const std::string logger,const std::string msg):
                _ctime(util::Date::GetTime()),
                _level(level),
                _cid(std::this_thread::get_id()),
                _file(file),
                _line(line),
                _logger(logger),
                _payLoad(msg)
                {}

    };   
}

#endif