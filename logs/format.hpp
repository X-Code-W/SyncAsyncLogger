#ifndef __FORMAT_HPP_
#define __FORMAT_HPP_

#include "level.hpp"
#include "message.hpp"
#include <vector>
#include <memory>
#include <sstream>
#include <cassert>
#include <cstdlib>
namespace mylog
{
    // 抽象格式化子项基类
    class FormatItem
    {
    public:
        virtual void format(std::ostream &out, const LogMsg &msg) = 0;
    };
    // 派生格式化子类对象-消息，等级，时间，文件名，线程id，行号，日志器，制表，换行，其他
    class MsgFormatItem : public FormatItem
    {
    public:
        void format(std::ostream &out, const LogMsg &msg) override
        {
            out << msg._payLoad;
        }
    };
    class LevelFormatItem : public FormatItem
    {
    public:
        void format(std::ostream &out, const LogMsg &msg) override
        {
            out << LogLevel::ToString(msg._level);
        }
    };
    class TimeFormatItem : public FormatItem
    {
    public:
        TimeFormatItem(const std::string &timefmt = "%H%M%S") : _timefmt(timefmt) {}
        void format(std::ostream &out, const LogMsg &msg) override
        {
            struct tm t;
            localtime_r(&msg._ctime, &t);
            char tmp[32] = {0};
            strftime(tmp, 31, _timefmt.c_str(), &t);
            out << tmp;
        }

    private:
        std::string _timefmt;
    };
    class FileFormatItem : public FormatItem
    {
    public:
        void format(std::ostream &out, const LogMsg &msg) override
        {
            out << msg._file;
        }
    };
    class LineFormatItem : public FormatItem
    {
    public:
        void format(std::ostream &out, const LogMsg &msg) override
        {
            out << msg._line;
        }
    };
    class ThreadFormatItem : public FormatItem
    {
    public:
        void format(std::ostream &out, const LogMsg &msg) override
        {
            out << msg._cid;
        }
    };
    class TabFormatItem : public FormatItem
    {
    public:
        void format(std::ostream &out, const LogMsg &msg) override
        {
            out << "\t";
        }
    };
    class LoggerFormatItem : public FormatItem
    {
    public:
        void format(std::ostream &out, const LogMsg &msg) override
        {
            out << msg._logger;
        }
    };
    class NlineFormatItem : public FormatItem
    {
    public:
        void format(std::ostream &out, const LogMsg &msg) override
        {
            out << "\n";
        }
    };
    class OtherFormatItem : public FormatItem
    {
    public:
        OtherFormatItem(const std::string &str) : _str(str) {}
        void format(std::ostream &out, const LogMsg &msg) override
        {
            out << _str;
        }

    private:
        std::string _str;
    };

    /*
         %d 日期
         %T 缩进
         %t 线程id
         %p 日志级别
         %c 日志器名称
         %f 文件名
         %l 行号
         %m 日志消息
         %n 换行
     */

    class Formatter
    {
    public:
        Formatter(const std::string &pattern = "[%d{%H:%M:%S}][%t][%c][%f:%l][%p]%T%m%n")
            : _pattern(pattern)
        {
            assert(ParsePattern());
        }
        // 对msg消息格式化
        void format(std::ostream &out, const LogMsg msg)
        {
            for (auto &item : _item)
                item->format(out, msg);
        }
        std::string format(const LogMsg &msg)
        {
            std::stringstream ss;
            format(ss, msg);
            return ss.str();
        }

    private:
        // 对格式化规则字符串进行解析
        bool ParsePattern()
        {
            // 对格式化规则字符串进行分析
            // %cabc%%[%d{%H:%M:%S}][%t][%p]%m%n
            std::vector<std::pair<std::string, std::string>> fmt_order;
            size_t pos = 0;
            std::string key, val;
            while (pos < _pattern.size())
            {
                // 原始字符
                if (_pattern[pos] != '%')
                {
                    val.push_back(_pattern[pos++]);
                    continue;
                }
                // 可能是%%，转义字符
                //%可能是最后一个字符，pos+1越界
                if (pos + 1 < _pattern.size() && _pattern[pos + 1] == '%')
                {
                    val.push_back('%');
                    pos += 2;
                    continue;
                }
                // 原始字符处理完毕
                if (!val.empty())
                {
                    fmt_order.push_back(std::make_pair("", val));
                    val.clear();
                }
                // 处理格式字符
                pos += 1; //%
                if (pos == _pattern.size())
                {
                    std::cout << "%之后，没有对应的格式化字符\n";
                    return false;
                }
                key = _pattern[pos]; // 格式字符，格式字符为最后一个
                pos += 1;            // 格式字符下一个位置
                if (pos < _pattern.size() && _pattern[pos] == '{')
                {
                    pos += 1; // 子项第一个位置
                    while (pos < _pattern.size() && _pattern[pos] != '}')
                    {
                        val.push_back(_pattern[pos++]);
                    }
                    if (pos == _pattern.size())
                    {
                        std::cout << "子规则{}匹配出错\n";
                        return false;
                    }
                    pos += 1;
                }
                fmt_order.push_back(std::make_pair(key, val));
                key.clear();
                val.clear();
            }
            // 根据解析得到的数据初始化格式子项数据成员
            for (auto &it : fmt_order)
            {
                _item.push_back(CreateItem(it.first, it.second));
            }
            return true;
        }
        // 根据不同的格式化字符串，创建不同的格式化子项对象
        std::shared_ptr<FormatItem> CreateItem(const std::string &key, const std::string &val)
        {
            if (key == "d")
                return std::make_shared<TimeFormatItem>(val);
            if (key == "T")
                return std::make_shared<TabFormatItem>();
            if (key == "t")
                return std::make_shared<ThreadFormatItem>();
            if (key == "c")
                return std::make_shared<LoggerFormatItem>();
            if (key == "p")
                return std::make_shared<LevelFormatItem>();
            if (key == "f")
                return std::make_shared<FileFormatItem>();
            if (key == "l")
                return std::make_shared<LineFormatItem>();
            if (key == "n")
                return std::make_shared<NlineFormatItem>();
            if (key == "m")
                return std::make_shared<MsgFormatItem>();
            if (key == "")
                return std::make_shared<OtherFormatItem>(val);
            std::cout<<"没有对应的格式化字符: %"<<key<<std::endl;
            abort();
        }
        std::string _pattern; // 格式化规则字符串
        std::vector<std::shared_ptr<FormatItem>> _item;
    };
}

#endif