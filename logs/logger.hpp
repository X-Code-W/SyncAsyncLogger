/* 日志器模块实现
    1.抽象日志器基类
    2.派生出同步日志器和异步日志器
*/

#ifndef __LOGGER_HPP_
#define __LOGGER_HPP_
// #define _GNU_SOURCE
#include "format.hpp"
#include "level.hpp"
#include "sink.hpp"
#include <mutex>
#include <atomic>
#include <cstdarg>
namespace mylog
{
    class Logger
    {
    public:
        Logger(std::string &logger_name, LogLevel::value level,
               std::shared_ptr<Formatter> &formatter,
               std::vector<std::shared_ptr<LogSink>> sinks)
            : _logger_name(logger_name), _limit_level(level),
              _formatter(formatter),
              _sinks(sinks.begin(), sinks.end()) {}
        // 完成构造日志消息对象过程并进行格式化，得到格式化的日志消息字符串--然后进行落地输出
        void debug(const std::string &file, size_t line, const std::string &fmt, ...)
        {
            // 1.判断当前日志等级是否小于输出等级
            if (LogLevel::value::DEBUG < _limit_level)
            {
                return;
            }
            // 2.对fmt格式化字符串和不定参进行字符串组织，得到日志消息的字符串
            va_list ap;
            va_start(ap, fmt);
            char *res;
            int ret = vasprintf(&res, fmt.c_str(), ap);
            if (ret = -1)
            {
                std::cout << "vasprintf failed!!\n";
                return;
            }
            va_end(ap);
            serialize(LogLevel::value::DEBUG, file, line, res);
            free(res);
        }
        void info(const std::string &file, size_t line, const std::string &fmt, ...)
        {
            // 1.判断当前日志等级是否小于输出等级
            if (LogLevel::value::INFO < _limit_level)
            {
                return;
            }
            // 2.对fmt格式化字符串和不定参进行字符串组织，得到日志消息的字符串
            va_list ap;
            va_start(ap, fmt);
            char *res;
            int ret = vasprintf(&res, fmt.c_str(), ap);
            if (ret == -1)
            {
                std::cout << "vasprintf failed!!\n";
                return;
            }
            va_end(ap);
            serialize(LogLevel::value::INFO, file, line, res);
            free(res);
        }
        void warn(const std::string &file, size_t line, const std::string &fmt, ...)
        {
            // 1.判断当前日志等级是否小于输出等级
            if (LogLevel::value::WARN < _limit_level)
            {
                return;
            }
            // 2.对fmt格式化字符串和不定参进行字符串组织，得到日志消息的字符串
            va_list ap;
            va_start(ap, fmt);
            char *res;
            int ret = vasprintf(&res, fmt.c_str(), ap);
            if (ret == -1)
            {
                std::cout << "vasprintf failed!!\n";
                return;
            }
            va_end(ap);
            serialize(LogLevel::value::WARN, file, line, res);
            free(res);
        }
        void error(const std::string &file, size_t line, const std::string &fmt, ...)
        {
            // 1.判断当前日志等级是否小于输出等级
            if (LogLevel::value::ERROR < _limit_level)
            {
                return;
            }
            // 2.对fmt格式化字符串和不定参进行字符串组织，得到日志消息的字符串
            va_list ap;
            va_start(ap, fmt);
            char *res;
            int ret = vasprintf(&res, fmt.c_str(), ap);
            if (ret == -1)
            {
                std::cout << "vasprintf failed!!\n";
                return;
            }
            va_end(ap);
            serialize(LogLevel::value::ERROR, file, line, res);
            free(res);
        }
        void fatal(const std::string &file, size_t line, const std::string &fmt, ...)
        {
            // 1.判断当前日志等级是否小于输出等级
            if (LogLevel::value::FATAL < _limit_level)
            {
                return;
            }
            // 2.对fmt格式化字符串和不定参进行字符串组织，得到日志消息的字符串
            va_list ap;
            va_start(ap, fmt);
            char *res;
            int ret = vasprintf(&res, fmt.c_str(), ap);
            if (ret == -1)
            {
                std::cout << "vasprintf failed!!\n";
                return;
            }
            va_end(ap);
            serialize(LogLevel::value::FATAL, file, line, res);
            free(res);
        }

    protected:
        void serialize(LogLevel::value level, const std::string &file, size_t line, char *str)
        {
            // 3.构造LogMsg对象
            LogMsg msg(level, file, line, _logger_name, str);
            // 4.对logmsg进行格式化
            std::stringstream ss;
            _formatter->format(ss, msg);
            // 5.日志落地
            log(ss.str().c_str(), ss.str().size());
        }
        // 抽象接口完成实际的落地输出--不同的日志器会有不同的实际落地方式
        virtual void log(const char *data, size_t len) = 0;

    protected:
        std::mutex _mutex;
        std::string _logger_name;
        std::atomic<LogLevel::value> _limit_level;
        std::shared_ptr<Formatter> _formatter;
        std::vector<std::shared_ptr<LogSink>> _sinks;
    };

    class SyncLogger : public Logger
    {
    public:
        SyncLogger(std::string &logger_name, LogLevel::value level,
                   std::shared_ptr<Formatter> &formatter,
                   std::vector<std::shared_ptr<LogSink>> sinks)
            : Logger(logger_name, level, formatter, sinks) {}

        void log(const char *data, size_t len) override
        {
            std::unique_lock<std::mutex> lock(_mutex);
            if (_sinks.empty())
                return;
            for (auto &sink : _sinks)
            {
                sink->log(data, len);
            }
        }
    };

    /*使用建造者模式构建日志器，参数过多，用户使用困难，为了简化用户操作难度
        1.抽象一个建造者类
            1.设计日志类型
            2.将不同的日志器的创建放在一个建造者类中
        2.派生出具体的建造者类（局部日志的建造者类&全局日志的建造者）
    */
    enum class LoggerType
    {
        LOGGER_SYNC,
        LOGGER_ASYNC,
    };
    class LoggerBuilder
    {
    public:
        LoggerBuilder()
            : _logger_type(LoggerType::LOGGER_SYNC),
              _limit_level(LogLevel::value::DEBUG) {}
        void BuildType(LoggerType type) { _logger_type = type; }
        void BuidLoggerName(const std::string &name) { _logger_name = name; }
        void BuildLoggerLevel(LogLevel::value level) { _limit_level = level; }
        void BuildFormatter(const std::string &pattern)
        {
            _formatter = std::make_shared<Formatter>(pattern);
        }
        template <typename Sinktype, typename... Args>
        void BuildSink(Args &&...args)
        {
            std::shared_ptr<LogSink> psink = SinkFactory::create<Sinktype>(std::forward<Args>(args)...);
            _sinks.push_back(psink);
        }
        virtual std::shared_ptr<Logger> Build() = 0;

    protected:
        LoggerType _logger_type;
        std::string _logger_name;
        LogLevel::value _limit_level;
        std::shared_ptr<Formatter> _formatter;
        std::vector<std::shared_ptr<LogSink>> _sinks;
    };

    class LocalLoggerBuilder : public LoggerBuilder
    {
    public:
        std::shared_ptr<Logger> Build() override
        {
            assert(!_logger_name.empty()); // 日志器名字不能为空
            if (_formatter.get() == nullptr)
            {
                _formatter = std::make_shared<Formatter>();
            }
            if (_sinks.empty())
            {
                BuildSink<StdoutSink>();
            }
            if (_logger_type == LoggerType::LOGGER_ASYNC)
            {
            }
            return std::make_shared<SyncLogger>(_logger_name, _limit_level, _formatter, _sinks);
        }
    };

}

#endif