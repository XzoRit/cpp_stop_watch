#include <lib/lib.hpp>

#include <boost/test/unit_test.hpp>

#include <chrono>
#include <functional>
#include <iostream>
#include <thread>
#include <utility>

namespace
{
using namespace std::chrono_literals;

using xzr::lib::auto_stop_watch_with_steady_clock;
using xzr::lib::auto_stop_watch_with_system_clock;
using xzr::lib::time_func;

class stop_watch
{
  private:
    using clock = std::chrono::steady_clock;

  public:
    using duration = clock::duration;

  private:
    using time_point = clock::time_point;

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
        *this = stop_watch{};
    }

    template <class Duration = duration>
    duration elapsed() const noexcept
    {
        if (_state == 0)
            return std::chrono::duration_cast<Duration>(duration{0});
        if (_state == 2)
            return std::chrono::duration_cast<Duration>(_elapsed);
        return std::chrono::duration_cast<Duration>(clock::now() - _start + _elapsed);
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
    using system = auto_stop_watch_with_system_clock;

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
