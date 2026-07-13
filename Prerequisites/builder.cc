#include <iostream>
#include <string>
#include <memory>
// 建造者模式

class Computer
{
public:
    Computer() {}
    void SetBaord(const std::string &baord)
    {
        _baord = baord;
    }
    void SetDisplay(const std::string &display)
    {
        _display = display;
    }
    virtual void SetOs() = 0;
    std::string show()
    {
        std::string s = "computer";
        s += "\tbaord" + _baord + "\n";
        s += "\tdisplay" + _display + "\n";
        s += "\tos" + _os + "\n";
        std::cout << s << std::endl;
        return s;
    }

protected:
    std::string _baord;
    std::string _display;
    std::string _os;
};

class MacBook : public Computer
{
    void SetOs() override
    {
        _os = "mac OS x12";
    }
};

class Builder
{
public:
    virtual void BuildBaord(const std::string &baord) = 0;
    virtual void BuildDisolay(const std::string &display) = 0;
    virtual void BuildOs() = 0;
    virtual std::shared_ptr<Computer> build() = 0;
};

class MacBuilder : public Builder
{
public:
    MacBuilder() : _computer(std::make_shared<MacBook>()) {}
    void BuildBaord(const std::string &baord) override
    {
        _computer->SetBaord(baord);
    }
    void BuildDisolay(const std::string &display) override
    {
        _computer->SetDisplay(display);
    }
    void BuildOs() override
    {
        _computer->SetOs();
    }
    std::shared_ptr<Computer> build() override
    {
        return _computer;
    }

private:
    std::shared_ptr<Computer> _computer;
};

class Director
{
public:
    Director(Builder *builder) : _builder(builder) {}
    void construct(const std::string &board, const std::string &display)
    {
        _builder->BuildBaord(board);
        _builder->BuildDisolay(display);
        _builder->BuildOs();
    }

private:
    std::shared_ptr<Builder> _builder;
};

int main()
{
    Builder *builder = new MacBuilder();
    std::unique_ptr<Director> director = std::make_unique<Director>(builder);
    director->construct("华硕", "三星");
    std::shared_ptr<Computer> computer = builder->build();
    computer->show();
    return 0;
}