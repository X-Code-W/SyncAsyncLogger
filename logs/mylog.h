
#ifndef __MTLOG_H__
#define __MTLOG_H__

#include "logger.hpp"

namespace mylog
{
    // 1.通过全局接口，获取指定日志器（避免用户进行单例模式）
    std::shared_ptr<Logger> getLogger(const std::string &name)
    {
        return LoggerManager::getInstance().getLogger(name);
    }
    std::shared_ptr<Logger> rootLogger()
    {
        return LoggerManager::getInstance().rootLogger();
    }
    //2.定义宏函数，对日志器接口进行代理
    #define debug(fmt,...) debug(__FILE__,__LINE__,fmt,##__VA_ARGS__)
    #define info(fmt,...) info(__FILE__,__LINE__,fmt,##__VA_ARGS__)
    #define warn(fmt,...) warn(__FILE__,__LINE__,fmt,##__VA_ARGS__)
    #define error(fmt,...) error(__FILE__,__LINE__,fmt,##__VA_ARGS__)
    #define fatal(fmt,...) fatal(__FILE__,__LINE__,fmt,##__VA_ARGS__)
    //3.提供宏函数，直接通过默认的日志器对日志进行标准输出
    #define DEBUG(fmt,...) mylog::rootLogger()->debug(fmt,##__VA_ARGS__)
    #define INFO(fmt,...) mylog::rootLogger()->info(fmt,##__VA_ARGS__)
    #define WARN(fmt,...) mylog::rootLogger()->warn(fmt,##__VA_ARGS__)
    #define ERROR(fmt,...) mylog::rootLogger()->error(fmt,##__VA_ARGS__)
    #define FATAL(fmt,...) mylog::rootLogger()->fatal(fmt,##__VA_ARGS__)

}

#endif