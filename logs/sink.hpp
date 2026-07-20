/*日志落地模块设计
    1.抽象落地基类
    2.多种落地方案进行派生
    3.运用简单工厂模式对落地方案进行创建和管理
*/

#ifndef __SINK_HPP_
#define __SINK_HPP_

#include <iostream>
#include <fstream>
#include <memory>
#include <cassert>
#include <sstream>
#include "util.hpp"
namespace mylog
{
    class LogSink // 抽象落地基类
    {
    public:
        LogSink() {}
        virtual ~LogSink() {} // 用户自定义落地方案，可能需要释放
        // 子类实现落地的起始位置和大小
        virtual void log(const char *data, size_t len) = 0;
    };
    // 落地方案：标准输出
    class StdoutSink : public LogSink
    {
    public:
        // 将日志写入标准输出
        void log(const char *data, size_t len) override
        {
            std::cout.write(data, len);
        }
    };
    // 落地方案：指定文件
    class FileSink : public LogSink
    {
    public:
        // 构造时传入文件路径名，并打开文件，将操作句柄管理起来
        FileSink(const std::string &pathname) : _pathname(pathname)
        {
            // 1.创建日志文件所在路径
            util::File::CreateDir(util::File::Path(_pathname));
            // 2.打开文件
            _ofs.open(_pathname, std::ios::binary | std::ios::app);
            assert(_ofs.is_open()); // 判断文件是否正常打开
        }
        // 将日志写入指定文件
        void log(const char *data, size_t len) override
        {
            _ofs.write(data, len);
            assert(_ofs.good());
        }

    private:
        std::string _pathname;
        std::ofstream _ofs; // 输出文件操作句柄
    };
    // 落地方案：滚动文件（按大小进行滚动）
    class RollBySizeSink : public LogSink
    {
    public:
        // 构造时传入文件路径名，并打开文件，将操作句柄管理起来
        RollBySizeSink(const std::string &basename, size_t max_size)
            : _basename(basename), _max_size(max_size), _cur_size(0),_namecount(0)
        {
            std::string pathname = CreateFile();
            // 1.创建日志文件所在路径
            util::File::CreateDir(util::File::Path(pathname));
            // 2.打开文件
            _ofs.open(pathname, std::ios::binary | std::ios::app);
            assert(_ofs.is_open()); // 判断文件是否正常打开
        }
        // 将日志写入指定文件,记录当前文件大小，如果超过最大大小，切换文件
        void log(const char *data, size_t len) override
        {
            if (_cur_size >= _max_size)
            {
                _ofs.close(); // 关闭之前打开的文件
                std::string pathname = CreateFile();
                _ofs.open(pathname, std::ios::binary | std::ios::app);
                assert(_ofs.is_open());
                _cur_size=0;
            }
            _ofs.write(data, len);
            assert(_ofs.good());
            _cur_size += len;
        }

    private:
        // 进行大小判断,超过指定大小创建新文件
        std::string CreateFile()
        {
            // 获取系统时间
            time_t t = util::Date::GetTime();
            struct tm lt;
            localtime_r(&t, &lt);
            std::stringstream filename;
            filename << _basename;
            filename << lt.tm_year + 1900;
            filename << lt.tm_mon + 1;
            filename << lt.tm_mday;
            filename << lt.tm_hour;
            filename << lt.tm_min;
            filename << lt.tm_sec;
            filename << "-";
            filename << _namecount++;
            filename << ".log";

            return filename.str();
        }

    private:
        std::string _basename; // 通过基础文件名+扩展名创建新的文件
        std::ofstream _ofs;
        size_t _max_size; // 文件的最大大小
        size_t _cur_size; // 文件的当前大小
        size_t _namecount;
    };

    class SinkFactory
    {
    public:
        template <typename SinkType, typename... Args>
        static std::shared_ptr<LogSink> create(Args &&...args)
        {
            return std::make_shared<SinkType>(std::forward<Args>(args)...);
        }
    };
}

#endif