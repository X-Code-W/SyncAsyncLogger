/*异步工作器设计
 */

#include "buffer.hpp"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>

namespace mylog
{
    using Function = std::function<void(Buffer &)>;
    enum class AsyncType
    {
        ASYNC_SAFE,
        ASYNC_NOSAFE,
    };
    class AsyncLooper
    {
    public:
        AsyncLooper(const Function &cb,
                    AsyncType type = AsyncType::ASYNC_SAFE) : _looper_type(type),
                                                              _stop(false),
                                                              _thread(std::thread(&AsyncLooper::threadStart, this)),
                                                              _callBack(cb) {}
        // 停止工作器
        void stop()
        {
            _stop = true;
            _con_cond.notify_all(); // 唤醒所有的工作线程
            _thread.join();         // 所有工作线程退出
        }
        void push(const char *data, size_t len)
        {
            // 1.无限扩容 2.固定空间，生产缓冲区中数据满了就阻塞
            std::unique_lock<std::mutex> lock(_mutex);
            // 条件变量为空值，若缓冲区剩余空间大小大于数据长度，可以写入数据
            if (_looper_type == AsyncType::ASYNC_SAFE)
                _pro_cond.wait(lock, [&]()
                               { return _pro_buf.writerAbleSize() >= len; });
            // 满足条件，写入数据
            _pro_buf.push(data, len);
            // 唤醒消费者对缓冲区数据处理
            _con_cond.notify_one();
        }

    private:
        void threadStart() // 线程入口函数
        {
            while (!_stop)
            {
                // 1.生产缓冲区是否有数据，有则交换，没有则堵塞
                {
                    std::unique_lock<std::mutex> lock(_mutex);
                    // 退出前或满足条件，往下走
                    _con_cond.wait(lock, [&]()
                                   { return _stop || !_pro_buf.empty(); });
                    _con_buf.swap(_pro_buf);
                    // 2.唤醒生产者
                    if (_looper_type == AsyncType::ASYNC_SAFE)
                        _pro_cond.notify_all();
                }
                // 3.进行数据处理
                _callBack(_con_buf);
                // 4.初始化消费缓冲区
                _con_buf.reset();
            }
        }

    private:
        Function _callBack; // 具体对缓冲区数据处理的回调函数，由异步工作者传入
    private:
        AsyncType _looper_type;
        std::atomic<bool> _stop; // 异步工作器停止标志
        Buffer _pro_buf;         // 生产缓冲区
        Buffer _con_buf;         // 消费缓冲区
        std::condition_variable _pro_cond;
        std::condition_variable _con_cond;
        std::thread _thread; // 异步工作器对应的工作线程
        std::mutex _mutex;
    };
}