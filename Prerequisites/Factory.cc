// 简单工厂模式
#include <iostream>
#include <memory>
#include <string>
class Fruit
{
public:
    virtual void name() = 0;
};

class Apple : public Fruit
{
public:
    void name() override
    {
        std::cout << "我是苹果" << std::endl;
    }
};

class Banana : public Fruit
{
public:
    void name() override
    {
        std::cout << "我是香蕉" << std::endl;
    }
};

class FactoryFruit
{
public:
    static std::unique_ptr<Fruit> create(const std::string &name)
    {
        if (name == "苹果")
            return std::make_unique<Apple>();
        else
            return std::make_unique<Banana>();
    }
};

int main()
{
    std::unique_ptr<Fruit> fruit = FactoryFruit::create("苹果");
    fruit->name();
}

// 工厂方法模式
class Fruit
{
public:
    virtual void name() = 0;
};

class Apple : public Fruit
{
public:
    void name() override
    {
        std::cout << "我是苹果" << std::endl;
    }
};

class Banana : public Fruit
{
public:
    void name() override
    {
        std::cout << "我是香蕉" << std::endl;
    }
};

class Factory
{
public:
    virtual std::unique_ptr<Fruit> create() = 0;
};

class AppleFactory : public Factory
{
public:
    std::unique_ptr<Fruit> create() override
    {
        return std::make_unique<Apple>();
    }
};
class BananaFactory : public Factory
{
public:
    std::unique_ptr<Fruit> create() override
    {
        return std::make_unique<Banana>();
    }
};

int main()
{
    std::unique_ptr<Factory> f = std::make_unique<AppleFactory>();
    std::unique_ptr<Fruit> ff = f->create();
    ff->name();
    return 0;
}

// 抽象工厂模式
 //思想：将⼯⼚抽象成两层，抽象⼯⼚ & 具体⼯⼚⼦类， 在⼯⼚⼦类种⽣产不同类型的⼦产品