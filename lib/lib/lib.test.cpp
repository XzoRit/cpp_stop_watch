#include <lib/lib.hpp>

#include <boost/test/unit_test.hpp>

#include <chrono>
#include <iostream>
#include <thread>

namespace std::chrono
{
ostream& operator<<(ostream& o, milliseconds ms)
{
    o << ms.count() << "ms";
    return o;
}

ostream& operator<<(ostream& o, microseconds us)
{
    o << us.count() << "us";
    return o;
}
} // namespace std::chrono
namespace
{
using namespace std::chrono_literals;

using xzr::lib::auto_stop_watch_with_steady_clock;
using xzr::lib::auto_stop_watch_with_system_clock;
using xzr::lib::stop_watch;
using xzr::lib::time_func;

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
        BOOST_TEST(time_func(iter, (void (*)(int))(v1::foo), 1) >= 500ms);
        BOOST_TEST((time_func<us>(iter, (void (*)(int))(v2::foo), 1)) >= 200ms);
        BOOST_TEST((time_func<us, steady>(iter, (void (*)(int))(v2::foo), 1)) >= 200ms);
    }
    {
        BOOST_TEST(time_func(iter, (int (*)())(v1::foo)) >= 500ms);
        BOOST_TEST((time_func<us>(iter, (int (*)())(v2::foo))) >= 200ms);
        BOOST_TEST((time_func<us, system>(iter, (int (*)())(v2::foo))) >= 200ms);
    }
    {
        stop_watch w{};
        using dur = std::chrono::milliseconds;

        BOOST_TEST(w.elapsed() == dur{0});
        BOOST_TEST(w.elapsed() == dur{0});
        w.start();
        std::this_thread::sleep_for(1ms);
        const auto a{w.elapsed()};
        BOOST_TEST(a > dur{0});
        std::this_thread::sleep_for(1ms);
        const auto b{w.elapsed()};
        BOOST_TEST(b > a);
        w.stop();
        const auto c{w.elapsed()};
        std::this_thread::sleep_for(1ms);
        const auto d{w.elapsed()};
        BOOST_TEST(c == d);
        w.start();
        std::this_thread::sleep_for(1ms);
        const auto e{w.elapsed()};
        BOOST_TEST(e > d);
        w.stop();
        w.reset();
        BOOST_TEST(w.elapsed() == dur{0});
        BOOST_TEST(w.elapsed() == dur{0});
        w.start();
        std::this_thread::sleep_for(1ms);
        w.reset();
        BOOST_TEST(w.elapsed() == dur{0});
        BOOST_TEST(w.elapsed() == dur{0});
    }
}

BOOST_AUTO_TEST_SUITE_END()
} // namespace
