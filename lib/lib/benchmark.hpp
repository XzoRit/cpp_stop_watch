#pragma once

#include <lib/lib.hpp>

#include <boost/hof/apply.hpp>

#include <type_traits>
#include <utility>

namespace xzr
{
namespace chrono
{
namespace impl
{
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
            invoke(typename std::is_void<decltype(boost::hof::apply(
                       _func))>::type{});
        }
        watch.stop();
        return watch.peek();
    }

  private:
    void invoke(std::true_type) noexcept
    {
        boost::hof::apply(_func);
    }
    void invoke(std::false_type) noexcept
    {
        volatile auto res{boost::hof::apply(_func)};
        do_not_optimize(res);
    }
    Func _func;
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
template <class Duration, class StopWatch = stop_watch<Duration>, class Func>
inline Duration benchmark(iterations n, Func&& func)
{
    return impl::duration_cast<Duration>(
        impl::benchmark_t<StopWatch, Func>{std::forward<Func>(func)}.measure(
            n.get()));
}
}
}
}
