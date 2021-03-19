#pragma once

#include <chrono>
#include <functional>
#include <utility>

namespace xzr
{
namespace lib
{
inline namespace v1
{
/// \brief returns the sum of a and b.
///
/// Just a simple sumation function with a nice documentation.
int add(int a, int b);
/// \brief Automatic stop watch.
///
/// Calcultes time pased after construction.
/// Can never be stopped or reset.
///
/// \tparam Clock Specifies which clock shall be used.
template <class Clock>
class basic_auto_stop_watch
{
  public:
    /// \brief Period of clock
    using duration = typename Clock::duration;
    /// \brief Returns time passed since construction.
    duration elapsed() const noexcept
    {
        return Clock::now() - _start;
    }

  private:
    using time_point = typename Clock::time_point;
    const time_point _start{Clock::now()};
};
/// \brief Auto stop 3watch with std::chrono::high_resolution_clock as time source.
using auto_stop_watch = basic_auto_stop_watch<std::chrono::high_resolution_clock>;
/// \brief Auto stop watch with std::chrono::system_clock as time source.
using auto_stop_watch_with_system_clock = basic_auto_stop_watch<std::chrono::system_clock>;

template <class AutoStopWatch, class Func>
class benchmark_t
{
  public:
    using duration = typename AutoStopWatch::duration;

    explicit benchmark_t(Func func) noexcept
        : _func{std::move(func)}
    {
    }

    duration measure(int iterations) const
    {
        AutoStopWatch watch{};
        for (int i{}; i < iterations; ++i)
        {
            if constexpr (std::is_void_v<decltype(std::invoke(_func))>)
            {
                std::invoke(_func);
            }
            else
            {
                volatile auto res = std::invoke(_func);
                static_cast<void>(res);
            }
        }
        return watch.elapsed();
    }

  private:
    Func _func;
};

template <class Duration = std::chrono::milliseconds, class AutoStopWatch = auto_stop_watch, class Func>
Duration benchmark(int iterations, Func&& func)
{
    return std::chrono::duration_cast<Duration>(
        benchmark_t<AutoStopWatch, Func>{std::forward<Func>(func)}.measure(iterations));
}

class stop_watch_state
{
  private:
    bool _running{false};

  public:
    void on_start() noexcept
    {
        _running = true;
    }

    void on_stop() noexcept
    {
        _running = false;
    }

    bool running() const noexcept
    {
        return _running;
    }
};

template <class Clock>
class basic_stop_watch : private stop_watch_state
{
  private:
    using clock = Clock;
    using time_point = typename clock::time_point;
    using duration = typename clock::duration;

    time_point _start{};
    duration _elapsed{duration::zero()};

  public:
    void start() noexcept
    {
        _start = clock::now();
        this->on_start();
    }

    void stop() noexcept
    {
        _elapsed = clock::now() - _start;
        this->on_stop();
    }

    void reset() noexcept
    {
        _elapsed = duration::zero();
    }

    template <class Duration = std::chrono::milliseconds>
    Duration peek() const noexcept
    {
        if (this->running())
            return std::chrono::duration_cast<Duration>(clock::now() - _start + _elapsed);
        return std::chrono::duration_cast<Duration>(_elapsed);
    }
};

using stop_watch = basic_stop_watch<std::chrono::high_resolution_clock>;
using stop_watch_with_system_clock = basic_stop_watch<std::chrono::system_clock>;
} // namespace v1
} // namespace lib
} // namespace xzr
