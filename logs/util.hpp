/*实用接口类设计
 获取系统时间
 判断文件是否存在
  获取文件路径
  创建目录
*/
#ifndef __UTIL_HPP_
#define __UTIL_HPP_

#include <iostream>
#include <ctime>
#include <string>
#include <unistd.h>
#include <sys/stat.h>
namespace mylog
{
    namespace util
    {
        class Date
        {
        public:
            static size_t GetTime()
            {
                return (size_t)time(nullptr);
            }
        };
        class File
        {
        public:
            static bool Exists(const std::string &name)
            {
                // return (access(pathname.c_str(),F_OK)==0);
                struct stat st;
                return (stat(name.c_str(), &st) == 0);
            }
            static std::string Path(const std::string &pathname)
            {
                //./a.txt
                //./abc/a.txt
                size_t pos = pathname.find_last_of("/\\");
                if (pos == std::string::npos)
                {
                    return ".";
                }
                return pathname.substr(0, pos+1);
            }
            static void CreateDir(const std::string &path)
            {
                //./abc/bcd/test.txt
                size_t pos = 0, idx = 0;
                while (idx < path.size())
                {
                    pos = path.find_first_of("/\\", idx);
                    if (pos == std::string::npos)
                    {
                        mkdir(path.c_str(), 0777);
                    }
                    std::string dir=path.substr(0,pos+1);
                    if (Exists(dir))
                    {
                        idx = pos + 1;
                        continue;
                    }
                    mkdir(dir.c_str(),0777);
                    idx=pos+1;
                }
            }
        };
    }
}


#endif 