#include <boost/test/unit_test.hpp>

#include <chrono>
#include <functional>
#include <iostream>
#include <thread>
#include <utility>

using namespace std::chrono_literals;

namespace
{

template <class Clock>
class auto_stop_watch
{
  public:
    using duration = typename Clock::duration;

    duration elapsed() const noexcept
    {
        return Clock::now() - _start;
    }

  private:
    using time_point = typename Clock::time_point;
    const time_point _start{Clock::now()};
};

using auto_stop_watch_with_steady_clock = auto_stop_watch<std::chrono::steady_clock>;
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

class stop_watch
{
  public:
    using clock = std::chrono::steady_clock;
    using duration = clock::duration;
    using time_point = clock::time_point;

    time_point _start{};
    duration _elapsed{0};
    int _state{0};

    void start()
    {
        _start = clock::now();
        _state = 1;
    }
    void stop()
    {
        _elapsed = clock::now() - _start;
        _state = 2;
    }

    void reset()
    {
        *this = stop_watch{};
    }

    duration elapsed()
    {
        if (_state == 0)
            return duration{0};
        if (_state == 2)
            return _elapsed;
        return clock::now() - _start + _elapsed;
    }
};

namespace v1
{
void foo(int a)
{
    std::this_thread::sleep_for(250ms * a);
}

int foo()
{
    std::this_thread::sleep_for(250ms);
    return (250ms).count();
}
} // namespace v1
namespace v2
{
void foo(int a)
{
    std::this_thread::sleep_for(100ms * a);
}

int foo()
{
    std::this_thread::sleep_for(100ms);
    return (100ms).count();
}
} // namespace v2

BOOST_AUTO_TEST_SUITE(lib_tests)

BOOST_AUTO_TEST_CASE(test_stop_watch)
{
    constexpr const int iter{2};

    using us = std::chrono::microseconds;
    using steady = auto_stop_watch_with_steady_clock;
    using system = auto_stop_watch_with_steady_clock;

    {
        BOOST_TEST(time_func(iter, (void (*)(int))(v1::foo), 1).count() >= 250);
        BOOST_TEST((time_func<us>(iter, (void (*)(int))(v2::foo), 1).count()) >= 100000);
        BOOST_TEST((time_func<us, steady>(iter, (void (*)(int))(v2::foo), 1).count()) >= 100000);
    }
    {
        BOOST_TEST(time_func(iter, (int (*)())(v1::foo)).count() >= (250ms).count());
        BOOST_TEST((time_func<us>(iter, (int (*)())(v2::foo)).count()) >= (100000us).count());
        BOOST_TEST((time_func<us, system>(iter, (int (*)())(v2::foo)).count()) >= (100000us).count());
    }
    {
        stop_watch w{};
        using dur = stop_watch::duration;

        BOOST_TEST(w.elapsed().count() == dur{0}.count());
        BOOST_TEST(w.elapsed().count() == dur{0}.count());
        w.start();
        std::this_thread::sleep_for(1ms);
        const auto a{w.elapsed().count()};
        BOOST_TEST(a > dur{0}.count());
        std::this_thread::sleep_for(1ms);
        const auto b{w.elapsed().count()};
        BOOST_TEST(b > a);
        w.stop();
        const auto c{w.elapsed().count()};
        std::this_thread::sleep_for(1ms);
        const auto d{w.elapsed().count()};
        BOOST_TEST(c == d);
        w.start();
        std::this_thread::sleep_for(1ms);
        const auto e{w.elapsed().count()};
        BOOST_TEST(e > d);
        w.stop();
        w.reset();
        BOOST_TEST(w.elapsed().count() == dur{0}.count());
        BOOST_TEST(w.elapsed().count() == dur{0}.count());
        w.start();
        std::this_thread::sleep_for(1ms);
        w.reset();
        BOOST_TEST(w.elapsed().count() == dur{0}.count());
        BOOST_TEST(w.elapsed().count() == dur{0}.count());
    }
}

BOOST_AUTO_TEST_SUITE_END()
} // namespace
