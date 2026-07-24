#include "mylog.h"
#include <thread>
#include <vector>

// 多线程测试日志打印函数
void testThreadLog(int tid)
{
    for (int i = 0; i < 10; ++i)
    {
        INFO("线程%d 循环打印 %d", tid, i);
        DEBUG("调试信息 tid=%d", tid);
        WARN("警告：测试告警 tid=%d", tid);
        ERROR("错误模拟 tid=%d", tid);
        FATAL("致命日志 tid=%d", tid);
    }
}

int main()
{
    // ====================== 方式1：使用默认root根日志器（开箱即用） ======================
    DEBUG("程序启动，使用默认root日志器，默认输出控制台");
    INFO("普通信息日志");
    WARN("警告日志");
    ERROR("错误日志");
    FATAL("致命错误日志");

    // ====================== 方式2：自定义创建异步日志器，输出控制台+滚动文件 ======================
    // 1. 创建全局日志建造者
    std::unique_ptr<mylog::LoggerBuilder> builder = std::make_unique<mylog::GlobalLoggerBuilder>();
    // 配置日志器名称
    builder->BuildLoggerName("biz_logger");
    // 设置为异步日志
    builder->BuildLoggerType(mylog::LoggerType::LOGGER_ASYNC);
    // 异步模式：ASYNC_SAFE 缓冲区满阻塞业务线程
    builder->BuildLooperType();
    // 设置最低输出等级：只输出INFO及以上，屏蔽DEBUG
    builder->BuildLoggerLevel(mylog::LogLevel::value::INFO);
    // 自定义格式化模板
    // 正确完整模板
    builder->BuildFormatter("[%d{%Y-%m-%d %H:%M:%S}][%t][%c][%f:%l][%p] | %m%n");
    // 添加输出1：控制台输出
    builder->BuildSink<mylog::StdoutSink>();
    // 添加输出2：按大小滚动日志文件，基础名./log/biz，单文件最大10MB
    builder->BuildSink<mylog::RollBySizeSink>("./log/biz", 10 * 1024 * 1024);
    // 构建日志器，自动存入全局管理器
    auto biz_log = builder->Build();

    // 获取自定义日志器打印日志
    auto logger = mylog::getLogger("biz_logger");
    logger->info("自定义异步日志器启动成功");
    logger->warn( "业务警告：参数异常 %s", "demo");

    // ====================== 多线程并发打印测试 ======================
    std::vector<std::thread> threads;
    for (int i = 0; i < 4; ++i)
    {
        threads.emplace_back(testThreadLog, i);
    }
    for (auto &t : threads)
    {
        t.join();
    }

    INFO("程序执行完毕");
    return 0;
}