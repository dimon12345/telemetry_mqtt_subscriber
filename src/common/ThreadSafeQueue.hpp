#pragma once

namespace Telemetry::Common {
    template <typename T>
    class ThreadSafeQueue {
    private:
        std::queue<T> queue_;
        mutable std::mutex mutex_;
        std::condition_variable cv_;
        bool aborted_ = false;

    public:
        ThreadSafeQueue() = default;

        void push(T value) {
            {
                std::lock_guard<std::mutex> lock(mutex_);
                queue_.push(std::move(value));
            }
            cv_.notify_one();
        }

        std::optional<T> pop() {
            std::unique_lock<std::mutex> lock(mutex_);
            cv_.wait(lock, [this]() { return !queue_.empty() || aborted_; });

            if (queue_.empty() && aborted_) {
                return std::nullopt;
            }

            T value = std::move(queue_.front());
            queue_.pop();
            return value;
        }

        void abort() {
            {
                std::lock_guard<std::mutex> lock(mutex_);
                aborted_ = true;
            }
            cv_.notify_all();
        }
    };
} // namespace Telemetry::Common
