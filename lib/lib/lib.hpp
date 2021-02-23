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
class auto_stop_watch
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
/// \brief Auto stop 3watch with std::chrono::steady_clock as time source.
using auto_stop_watch_with_steady_clock = auto_stop_watch<std::chrono::steady_clock>;
/// \brief Auto stop watch with std::chrono::system_clock as time source.
using auto_stop_watch_with_system_clock = auto_stop_watch<std::chrono::system_clock>;

template <class AutoStopWatch, class Func>
class timed_func
{
  public:
    using duration = typename AutoStopWatch::duration;

    explicit timed_func(Func func) noexcept
        : _func{std::move(func)}
    {
    }

    template <class... Args>
    duration measure(int iterations, Args&&... args) const
    {
        AutoStopWatch watch{};
        for (int i{}; i < iterations; ++i)
        {
            if constexpr (std::is_void_v<decltype(std::invoke(_func, std::forward<Args>(args)...))>)
            {
                std::invoke(_func, std::forward<Args>(args)...);
            }
            else
            {
                volatile auto res = std::invoke(_func, std::forward<Args>(args)...);
                static_cast<void>(res);
            }
        }
        return watch.elapsed();
    }

  private:
    Func _func;
};

template <class Duration = std::chrono::milliseconds,
          class AutoStopWatch = auto_stop_watch_with_steady_clock,
          class Func,
          class... Args>
Duration time_func(int iterations, Func&& func, Args&&... args)
{
    return std::chrono::duration_cast<Duration>(
        timed_func<AutoStopWatch, Func>{std::forward<Func>(func)}.measure(iterations, std::forward<Args>(args)...));
}

template <class Clock>
class basic_stop_watch
{
  private:
    using clock = Clock;
    using time_point = typename clock::time_point;
    using duration = typename clock::duration;

    time_point _start{};
    duration _elapsed{0};
    int _state{0};

  public:
    void start() noexcept
    {
        _start = clock::now();
        _state = 1;
    }

    void stop() noexcept
    {
        _elapsed = clock::now() - _start;
        _state = 2;
    }

    void reset() noexcept
    {
        *this = basic_stop_watch{};
    }

    template <class Duration = std::chrono::milliseconds>
    Duration elapsed() const noexcept
    {
        if (_state == 0)
            return std::chrono::duration_cast<Duration>(duration{0});
        if (_state == 2)
            return std::chrono::duration_cast<Duration>(_elapsed);
        return std::chrono::duration_cast<Duration>(clock::now() - _start + _elapsed);
    }
};
using stop_watch = basic_stop_watch<std::chrono::steady_clock>;
} // namespace v1
} // namespace lib
} // namespace xzr
