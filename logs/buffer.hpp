/* 异步操作缓冲区设计 */
#include "util.hpp"
#include <vector>
#include <cassert>
#include <algorithm>
#include <cstring>

namespace mylog
{
#define BUFFER_DEFAULT_SIZE (10 * 1024 * 1024)
#define BUFFER_THRESHOLD_SIZE (80 * 1024 * 1024)
#define BUFFER_INCREMENT_SIZE (10 * 1024 * 1024)

    class Buffer
    {
    public:
        Buffer() : _buffer(BUFFER_DEFAULT_SIZE), _reader_idx(0), _writer_idx(0) {}

        // 向缓冲区写入数据
        void push(const char *data, size_t len)
        {
            if (len == 0 || data == nullptr)
                return;
            // 确保空间充足
            ensureEnoughSize(len);
            // 将数据拷贝到缓冲区
            std::copy(data, data + len, &_buffer[_writer_idx]);
            // 移动写指针
            moveWriter(len);
        }

        // 获取可读数据的起始位置
        const char *begin() const
        {
            return &_buffer[_reader_idx];
        }

        // 获取可写剩余长度
        size_t writerAbleSize() const
        {
            return _buffer.size() - _writer_idx;
        }

        // 获取有效可读数据长度
        size_t readerAbleSize() const
        {
            return _writer_idx - _reader_idx;
        }

        // 移动读指针
        void moveReader(size_t len)
        {
            assert(len <= readerAbleSize());
            _reader_idx += len;
        }

        // 重置读写指针，清空缓冲区
        void reset()
        {
            _writer_idx = 0;
            _reader_idx = 0;
        }

        // 两个缓冲区交换资源
        void swap(Buffer &buffer)
        {
            _buffer.swap(buffer._buffer);
            std::swap(_reader_idx, buffer._reader_idx);
            std::swap(_writer_idx, buffer._writer_idx);
        }

        // 缓冲区是否无有效数据
        bool empty() const
        {
            return _writer_idx == _reader_idx;
        }

    private:
        // 移动写指针
        void moveWriter(size_t len)
        {
            assert(_writer_idx + len <= _buffer.size());
            _writer_idx += len;
        }

        // 按需扩容：小于阈值翻倍扩容，超过阈值线性增长
        void ensureEnoughSize(size_t len)
        {
            // 剩余空间足够，无需扩容
            if (writerAbleSize() >= len)
                return;

            size_t new_size = _buffer.size();
            // 循环扩容直到空间满足写入需求
            do
            {
                if (new_size < BUFFER_THRESHOLD_SIZE)
                {
                    new_size *= 2;
                }
                else
                {
                    new_size += BUFFER_INCREMENT_SIZE;
                }
            } while ((new_size - _writer_idx) < len);

            _buffer.resize(new_size);
        }

    private:
        std::vector<char> _buffer;
        size_t _reader_idx; // 读偏移
        size_t _writer_idx; // 写偏移
    };
}