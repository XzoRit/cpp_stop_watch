#pragma once

#include <lib/cast.hpp>

#include <boost/chrono/duration.hpp>
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
    bool running() const noexcept
    {
        return _running;
    }
};
}
}
}
namespace xzr
{
namespace chrono
{
inline namespace v1
{
/// \brief Parameter shall not be optimized by compiler.
#if defined(__GNUC__) or defined(__clang__)
template <class T>
inline void do_not_optimize(T&& t)
{
    asm volatile("" ::"m"(t) : "memory");
}
#else
#pragma optimize("", off)
template <class T>
inline void do_not_optimize(T&& t)
{
    reinterpret_cast<char volatile&>(t) =
        reinterpret_cast<char const volatile&>(t);
}
#pragma optimize("", on)
#endif
/// \brief Tag type for starting a stopwatch on creation.
struct auto_start_t
{
};
/// \brief Stopwatch created with this value will start automatically.
constexpr const auto_start_t auto_start{};
/// \brief Models a physical stopwatch to measure how much time elapsed between
/// starting and stopping it.
///
/// To start the stopwatch you can either call start
/// on a default constructed stopwatch or use \ref auto_start on construction
/// for starting it immediately.
///
/// After calling \ref stop the elapsed time can be retrieved with \ref peek.
///
/// It is ok to call \ref peek on a running stopwatch to get a snapshot of the
/// elapsed time.
///
/// \ref reset can be used to set the elapsed time to zero.
///
/// \note Calling \ref start or \ref reset on a running stopwatch and \ref stop
/// on a stopped stopwatch is an error and throws.
///
/// @tparam Clock Specifies the source for measuring how much time elapsed.
/// It has to model the [named requirement
/// Clock](https://en.cppreference.com/w/cpp/named_req/Clock).
///
/// @tparam Duration Specifies the precision of this stopwatch.
template <class Clock, class Duration>
class basic_stop_watch : private impl::stop_watch_state
{
  public:
    /// \brief Clock used by this stopwatch.
    using clock = Clock;
    /// \brief Precision of this stopwatch.
    using duration = Duration;
    /// \brief Stopped stopwatch.
    ///
    /// \post is_running returns false.
    basic_stop_watch() = default;
    /// \brief Stopwatch is started immediately.
    ///
    /// \post is_running returns true.
    explicit basic_stop_watch(auto_start_t)
    {
        start();
    }
    /// \brief Starts the stopwatch.
    ///
    /// \pre \ref is_running returns false.
    /// \post \ref is_running returns true.
    ///
    /// \throws std::runtime_error If precondition is violated.
    void start()
    {
        if (running())
            throw std::runtime_error{
                "start was called on an already started stopwatch"};
        if (is_reset())
            _start = clock::now();
        on_start();
    }
    /// \brief Stops the stopwatch.
    ///
    /// \pre \ref is_running returns true.
    /// \post \ref is_running returns false. Multiple calls to \ref peek return
    /// the same value.
    ///
    /// \throws std::runtime_error If precondition is violated.
    void stop()
    {
        if (!running())
            throw std::runtime_error{
                "stop was called on an already stopped stopwatch"};
        _elapsed = impl::duration_cast<duration>(clock::now() - _start);
        on_stop();
    }
    /// \brief Reset elapsed time to zero.
    ///
    /// \pre \ref is_running returns false.
    /// \post \ref peek returns duration::zero.
    ///
    /// \throws std::runtime_error If precondition is violated.
    void reset()
    {
        if (running())
            throw std::runtime_error{"reset was called on a running stopwatch"};
        _elapsed = duration::zero();
    }
    /// \brief Returns the elapsed time since stopwatch was started.
    ///
    /// Can be called even if stopwatch is running to get a snapshot of the
    /// elapsed time since \ref start has been called.
    duration peek() const noexcept
    {
        if (running())
            return impl::duration_cast<duration>(clock::now() - _start +
                                                 _elapsed);
        return impl::duration_cast<duration>(_elapsed);
    }
    /// \brief True if stopwatch is running otherwise false.
    bool is_running() const noexcept
    {
        return running();
    }

  private:
    bool is_reset() const noexcept
    {
        return _elapsed == duration::zero();
    }

  private:
    using time_point = typename clock::time_point;

    time_point _start{};
    duration _elapsed{duration::zero()};
};
//// \brief Stopwatch using std::chrono::steady_clock as a source.
template <class Precision>
using stop_watch = basic_stop_watch<std::chrono::steady_clock, Precision>;
//// \brief Stopwatch using std::chrono::system_clock as a source.
template <class Precision>
using system_stop_watch =
    basic_stop_watch<std::chrono::system_clock, Precision>;
}
}
}
