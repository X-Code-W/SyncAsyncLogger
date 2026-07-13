// 不定参宏定义
#include <iostream>
#include <cstdarg>

#define LOG(fmt,...) printf("[%s:%d]" fmt,__FILE__,__LINE__,##__VA_ARGS__);

int main()
{
    LOG("%s-%s\n", "hello", "github");
    //不加##，会报错，VA为空 ，#define LOG(fmt,...) printf("[%s:%d]" fmt,__FILE__,__LINE__,);
    //多一个逗号，加上##一起删去
    LOG("github");
    return 0;
}

// C语言不定参数
#include <iostream>
#include <cstdarg>
void printNum(int n, ...)
{
    va_list al;
    va_start(al, n); // 让al指向n参数之后的第⼀个可变参数
    for (int i = 0; i < n; i++)
    {
        int num = va_arg(al, int); // 从可变参数中取出⼀个整形参数
        std::cout << num << std::endl;
    }
    va_end(al); // 清空可变参数列表--其实是将al置空
}
int main()
{
    printNum(3, 11, 22, 33);
    printNum(5, 44, 55, 66, 77, 88);
    return 0;
}

// C++不定参数
#include <iostream>
#include <cstdarg>
#include <memory>
#include <functional>
void xprintf()
{
    std::cout << std::endl;
}
template <typename T, typename... Args>
void xprintf(const T &value, Args &&...args)//右值引用
{
    std::cout << value << " ";
    if ((sizeof...(args)) > 0)//编译时处理
    {
        //参数为空，走特化版本
        xprintf(std::forward<Args>(args)...);//完美转发

    }
    else
    {
        xprintf();
    }
}
int main()
{
    xprintf("git");
    xprintf("git", 666);
    xprintf("git", "hub", 666);
    return 0;
}