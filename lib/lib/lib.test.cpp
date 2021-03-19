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

using xzr::lib::auto_stop_watch;
using xzr::lib::auto_stop_watch_with_system_clock;
using xzr::lib::benchmark;
using xzr::lib::stop_watch;

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
    using steady = auto_stop_watch;
    using system = auto_stop_watch_with_system_clock;

    {
        BOOST_TEST(benchmark(iter, []() { v1::foo(1); }) >= 500ms);
        BOOST_TEST(benchmark<us>(iter, []() { v2::foo(1); }) >= 200ms);
        BOOST_TEST((benchmark<us, steady>(iter, []() { v2::foo(1); })) >= 200ms);
    }
    {
        BOOST_TEST(benchmark(iter, []() { v1::foo(); }) >= 500ms);
        BOOST_TEST(benchmark<us>(iter, []() { v2::foo(); }) >= 200ms);
        BOOST_TEST((benchmark<us, system>(iter, []() { v2::foo(); })) >= 200ms);
    }
    {
        stop_watch w{};
        using dur = std::chrono::milliseconds;

        BOOST_TEST(w.peek() == dur::zero());
        BOOST_TEST(w.peek() == dur::zero());
        w.start();
        std::this_thread::sleep_for(1ms);
        const auto a{w.peek()};
        BOOST_TEST(a > dur::zero());
        std::this_thread::sleep_for(1ms);
        const auto b{w.peek()};
        BOOST_TEST(b > a);
        w.stop();
        const auto c{w.peek()};
        std::this_thread::sleep_for(1ms);
        const auto d{w.peek()};
        BOOST_TEST(c == d);
        w.start();
        std::this_thread::sleep_for(1ms);
        const auto e{w.peek()};
        BOOST_TEST(e > d);
        w.stop();
        w.reset();
        BOOST_TEST(w.peek() == dur::zero());
        BOOST_TEST(w.peek() == dur::zero());
        w.start();
        std::this_thread::sleep_for(1ms);
        w.reset();
        const auto f{w.peek()};
        BOOST_TEST(f < e);
        BOOST_TEST(f > dur::zero());
        std::this_thread::sleep_for(1ms);
        const auto g{w.peek()};
        BOOST_TEST(g > f);
    }
}

BOOST_AUTO_TEST_SUITE_END()
} // namespace
