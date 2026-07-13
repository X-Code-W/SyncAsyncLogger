#include <iostream>

// 饿汉模式
class Singleton
{
private:
    // 类加载时直接创建静态变量
    static Singleton _instance;
    Singleton()
    {
        std::cout << "饿汉单例对象" << std::endl;
    }
    Singleton(const Singleton &) = delete;
    ~Singleton() {}

public:
    static Singleton &getInstance()
    {
        return _instance;
    }
};
//Singleton Singleton::_instance;

// 懒汉模式
class Singleton
{
private:
    Singleton() = default;
    Singleton(const Singleton &) = delete;
    Singleton &operator=(const Singleton &) = delete;
    //c++11,线程安全版本，相比于老版本双if更加直观
public:
    static Singleton &getInstance()
    {
        // 局部静态变量：首次进入函数才创建，线程安全
        static Singleton ins;
        return ins;
    };
} ;
int main()
{
    // Singleton
    return 0;
}
