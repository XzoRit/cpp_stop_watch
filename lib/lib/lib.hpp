#pragma once

#include <boost/hof/apply.hpp>

#include <chrono>
#include <functional>
#include <stdexcept>
#include <utility>

#if defined(BOOST_CHRONO_HAS_PROCESS_CLOCK)
#include <boost/chrono/process_cpu_clocks.hpp>
#endif
#if defined(BOOST_CHRONO_HAS_THREAD_CLOCK)
#include <boost/chrono/thread_clock.hpp>
#endif

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
struct auto_start_t
{
};
template <class StopWatch, class Func>
class benchmark_t
{
  private:
    using stop_watch = StopWatch;

  public:
    using duration = typename stop_watch::duration;

    explicit benchmark_t(Func&& func) noexcept
        : _func{std::forward<Func>(func)}
    {
    }
    duration measure(int iterations) noexcept
    {
        stop_watch watch{auto_start_t{}};
        for (int i{}; i < iterations; ++i)
        {
            invoke(typename std::is_void<decltype(boost::hof::apply(_func))>::type{});
        }
        return watch.peek();
    }

  private:
    void invoke(std::true_type) noexcept
    {
        boost::hof::apply(_func);
    }
    void invoke(std::false_type) noexcept
    {
        volatile auto res = boost::hof::apply(_func);
        static_cast<void>(res);
    }
    Func _func;
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
/// \brief Tag type for starting a stopwatch on creation.
using auto_start_t = impl::auto_start_t;
/// \brief Stopwatch created with this value will start automatically.
constexpr const auto_start_t auto_start{};
/// \brief Models a physical stopwatch to measure how much time elapsed between starting and stopping it.
///
/// The precision of this stopwatch depends on the one from the specified clock but a coarser precision can be specified
/// on each call to \ref peek. To start the stopwatch you can either call start on a default constructed stopwatch or
/// use \ref auto_start_t on construction for starting it immediately. After calling \ref stop the elapsed
/// time can be retrieved witth \ref peek. It is ok to call \ref peek on a running stopwatch to get a snapshot of the
/// elapsed time. \ref reset can be used to set the elapsed time to zero. This works on a running aswell as on a stopped
/// stopwatch.
///
/// \note Calling start on a running stopwatch and stop on a stopped stopwatch is an error and throws.
///
/// @tparam Clock Specifies the source for measuring how much time elapsed. It has to model the [named requirement
/// Clock](https://en.cppreference.com/w/cpp/named_req/Clock)
template <class Clock>
class basic_stop_watch : private impl::stop_watch_state
{
  private:
    using clock = Clock;
    using time_point = typename clock::time_point;

  public:
    /// \brief Precision of stopwatch.
    using duration = typename clock::duration;

  public:
    basic_stop_watch() = default;
    /// \brief Stopwatch is started immediately.
    ///
    /// \post running returns true.
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
            throw std::runtime_error{"start was called when the stopwatch was already running"};
        _start = clock::now();
        on_start();
    }
    /// \brief Stops the stopwatch.
    ///
    /// \pre \ref is_running returns true.
    /// \post \ref is_running returns false. Multiple calls to \ref peek return the same value.
    ///
    /// \throws std::runtime_error If precondition is violated.
    void stop()
    {
        if (!running())
            throw std::runtime_error{"stop was called when the stopwatch was not running"};
        _elapsed = clock::now() - _start;
        on_stop();
    }
    /// \brief Reset elapsed time to zero.
    ///
    /// Can be called on a stoppped aswell as on a running stopwatch.
    void reset() noexcept
    {
        if (running())
            _start = clock::now();
        _elapsed = duration::zero();
    }
    /// \brief Returns the elapsed time since stopwatch was started.
    ///
    /// Can be called even if stopwatch is running to get a snapshot of the elapsed time since \ref start has been
    /// called.
    ///
    /// \tparam Duration Precision to be returned.
    template <class Duration = duration>
    Duration peek() const noexcept
    {
        if (running())
            return std::chrono::duration_cast<Duration>(clock::now() - _start + _elapsed);
        return std::chrono::duration_cast<Duration>(_elapsed);
    }
    /// \brief True if stopwatch is running otherwise false.
    bool is_running() const noexcept
    {
        return running();
    }

  private:
    time_point _start{};
    duration _elapsed{duration::zero()};
};
//// \brief Stopwatch using std::chrono::high_resolution_clock as a source.
using stop_watch = basic_stop_watch<std::chrono::high_resolution_clock>;
//// \brief Stopwatch using std::chrono::steady_clock as a source.
using stop_watch_with_steady_clock = basic_stop_watch<std::chrono::steady_clock>;
//// \brief Stopwatch using std::chrono::system_clock as a source.
using stop_watch_with_system_clock = basic_stop_watch<std::chrono::system_clock>;
#if defined(BOOST_CHRONO_HAS_THREAD_CLOCK)
//// \brief Stopwatch using boost::chrono::thread_clock as a source.
using stop_watch_with_boost_thread_clock = basic_stop_watch<boost::chrono::thread_clock>;
#endif
#if defined(BOOST_CHRONO_HAS_PROCESS_CLOCKS)
//// \brief Stopwatch using boost::chrono::process_real_cpu_clock as a source.
using stop_watch_with_boost_process_real_cpu_clock = basic_stop_watch<boost::chrono::process_real_cpu_clock>;
//// \brief Stopwatch using boost::chrono::process_user_cpu_clock as a source.
using stop_watch_with_boost_process_user_cpu_clock = basic_stop_watch<boost::chrono::process_user_cpu_clock>;
//// \brief Stopwatch using boost::chrono::process_system_cpu_clock as a source.
using stop_watch_with_boost_process_system_cpu_clock = basic_stop_watch<boost::chrono::process_system_cpu_clock>;
//// \brief Stopwatch using boost::chrono::process_cpu_clock as a source.
using stop_watch_with_boost_process_cpu_clock = basic_stop_watch<boost::chrono::process_cpu_clock>;
#endif
/// \brief Named parameter for the sepcifying the amount of iterations.
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
/// @tparam Duration Precision of execution time to be returned.
/// @tparam Stopwatch Defines the stopwatch to measure execution time with.
template <class Duration = typename stop_watch::duration, class StopWatch = stop_watch, class Func>
Duration benchmark(iterations n, Func&& func)
{
    return std::chrono::duration_cast<Duration>(
        impl::benchmark_t<StopWatch, Func>{std::forward<Func>(func)}.measure(n.get()));
}
#if defined(__GNUC__) or defined(__clang__)
template <class T>
void do_not_optimize(T&& t)
{
    asm volatile("" ::"m"(t) : "memory");
}
#else
#pragma optimize("", off)
template <class T>
void do_not_optimize(T&& t)
{
    reinterpret_cast<char volatile&>(t) = reinterpret_cast<char const volatile&>(t);
}
#pragma optimize("", on)
#endif
} // namespace v1
} // namespace chrono
} // namespace xzr
