#pragma once

#include <boost/hof/apply.hpp>

#include <chrono>
#include <functional>
#include <stdexcept>
#include <utility>

namespace xzr
{
namespace chrono
{
namespace impl
{
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
    bool is_running() const noexcept
    {
        return _running;
    }
};
} // namespace impl
} // namespace chrono
} // namespace xzr
namespace xzr
{
namespace chrono
{
inline namespace v1
{
struct auto_start_t
{
};
constexpr const auto_start_t auto_start{};
template <class Clock>
class basic_stop_watch : private impl::stop_watch_state
{
  private:
    using clock = Clock;
    using time_point = typename clock::time_point;

  public:
    using duration = typename clock::duration;

  public:
    basic_stop_watch() = default;
    explicit basic_stop_watch(auto_start_t)
    {
        start();
    }
    void start()
    {
        if (is_running())
            throw std::runtime_error{"start was called when the stopwatch was already running"};
        _start = clock::now();
        on_start();
    }
    void stop()
    {
        if (!is_running())
            throw std::runtime_error{"stop was called when the stopwatch was not running"};
        _elapsed = clock::now() - _start;
        on_stop();
    }
    void reset() noexcept
    {
        if (is_running())
            _start = clock::now();
        _elapsed = duration::zero();
    }
    template <class Duration = duration>
    Duration peek() const noexcept
    {
        if (is_running())
            return std::chrono::duration_cast<Duration>(clock::now() - _start + _elapsed);
        return std::chrono::duration_cast<Duration>(_elapsed);
    }
    bool running() const noexcept
    {
        return is_running();
    }

  private:
    time_point _start{};
    duration _elapsed{duration::zero()};
};
using stop_watch = basic_stop_watch<std::chrono::high_resolution_clock>;
using stop_watch_with_steady_clock = basic_stop_watch<std::chrono::steady_clock>;
using stop_watch_with_system_clock = basic_stop_watch<std::chrono::system_clock>;
template <class StopWatch, class Func>
class benchmark_t
{
  private:
    using stop_watch = StopWatch;

  public:
    using duration = typename stop_watch::duration;

    explicit benchmark_t(Func func) noexcept
        : _func{std::move(func)}
    {
    }
    duration measure(int iterations) const
    {
        stop_watch watch{auto_start};
        for (int i{}; i < iterations; ++i)
        {
            invoke(typename std::is_void<decltype(boost::hof::apply(_func))>::type{});
        }
        return watch.peek();
    }

  private:
    void invoke(std::true_type) const
    {
        boost::hof::apply(_func);
    }
    void invoke(std::false_type) const
    {
        volatile auto res = boost::hof::apply(_func);
        static_cast<void>(res);
    }
    Func _func;
};
/// \brief Named parameter for the \ref benchmark function.
class iterations
{
  public:
    /// \brief Sets the amount of iterations.
    constexpr explicit iterations(int i)
        : _value{i}
    {
    }
    /// \brief Returns the amount of iterations.
    constexpr int get() const noexcept
    {
        return _value;
    }

  private:
    int _value{0};
};
/// \brief Executes given function n-times and returns the execution time.
///
/// @tparam Duration Precision of execution time.
/// @tparam AutoStopwatch Defines the stopwatch to measure execution time with.
template <class Duration = std::chrono::milliseconds, class StopWatch = stop_watch, class Func>
Duration benchmark(iterations n, Func&& func)
{
    return std::chrono::duration_cast<Duration>(
        benchmark_t<StopWatch, Func>{std::forward<Func>(func)}.measure(n.get()));
}
} // namespace v1
} // namespace chrono
} // namespace xzr
