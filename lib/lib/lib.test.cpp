#include <lib/lib.hpp>

#include <boost/test/unit_test.hpp>

#include <chrono>
#include <iostream>
#include <thread>

namespace std
{
namespace chrono
{
ostream& boost_test_print_type(ostream& o, const seconds& s)
{
    o << s.count() << "sec";
    return o;
}
ostream& boost_test_print_type(ostream& o, const milliseconds& ms)
{
    o << ms.count() << "ms";
    return o;
}
ostream& boost_test_print_type(ostream& o, const microseconds& us)
{
    o << us.count() << "us";
    return o;
}
ostream& boost_test_print_type(ostream& o, const nanoseconds& ns)
{
    o << ns.count() << "ns";
    return o;
}
}
}
namespace
{
using namespace std::chrono_literals;

using xzr::chrono::auto_start;
using xzr::chrono::auto_start_t;
using xzr::chrono::benchmark;
using xzr::chrono::do_not_optimize;
using xzr::chrono::iterations;
using xzr::chrono::stop_watch;

using stop_watch_ns = stop_watch<std::chrono::nanoseconds>;

BOOST_AUTO_TEST_SUITE(test_chrono)
BOOST_AUTO_TEST_SUITE(test_stop_watch)
BOOST_AUTO_TEST_CASE(not_running_on_construction)
{
    stop_watch_ns w{};
    BOOST_TEST(!w.is_running());
    BOOST_TEST(w.peek() == stop_watch_ns::duration::zero());
}
BOOST_AUTO_TEST_CASE(construction_with_auto_start)
{
    stop_watch_ns w{auto_start};
    BOOST_TEST(w.is_running());
    BOOST_TEST(w.peek() > stop_watch_ns::duration::zero());
}
BOOST_AUTO_TEST_CASE(start)
{
    stop_watch_ns w{};
    w.start();
    BOOST_TEST(w.is_running());
    const auto a{w.peek()};
    BOOST_TEST(a > stop_watch_ns::duration::zero());
}
BOOST_AUTO_TEST_CASE(start_running_stop_watch)
{
    stop_watch_ns w{auto_start};
    BOOST_REQUIRE_THROW(w.start(), std::runtime_error);
}
BOOST_AUTO_TEST_CASE(stop)
{
    stop_watch_ns w{auto_start};
    w.stop();
    BOOST_TEST(!w.is_running());
    BOOST_TEST(w.peek() > stop_watch_ns::duration::zero());
}
BOOST_AUTO_TEST_CASE(stop_stopped_stop_watch)
{
    stop_watch_ns w{};
    BOOST_REQUIRE_THROW(w.stop(), std::runtime_error);
}
BOOST_AUTO_TEST_CASE(start_stop_peek_stop_watch)
{
    stop_watch_ns w{};
    w.start();
    w.stop();
    const auto a{w.peek()};
    w.start();
    w.stop();
    const auto b{w.peek()};
    BOOST_TEST(a < b);
    w.start();
    w.stop();
    const auto c{w.peek()};
    BOOST_TEST(b < c);
}
BOOST_AUTO_TEST_CASE(reset_stopped_watch)
{
    stop_watch_ns w{auto_start};
    w.stop();
    w.reset();
    BOOST_TEST(w.peek() == stop_watch_ns::duration::zero());
}
BOOST_AUTO_TEST_CASE(reset_running_watch)
{
    stop_watch_ns w{auto_start};
    BOOST_REQUIRE_THROW(w.reset(), std::runtime_error);
}
BOOST_AUTO_TEST_CASE(peek_stoped_stop_watch)
{
    stop_watch_ns w{auto_start};
    w.stop();
    const auto a{w.peek()};
    const auto b{w.peek()};
    BOOST_TEST(a == b);
}
BOOST_AUTO_TEST_CASE(peek_with_duration)
{
    {
        using stop_watch_ms = stop_watch<std::chrono::milliseconds>;

        stop_watch_ms w{};

        static_assert(std::is_same<stop_watch_ms::duration,
                                   std::chrono::milliseconds>::value,
                      "");
        static_assert(
            std::is_same<decltype(w.peek()), std::chrono::milliseconds>::value,
            "");
    }
    {
        using stop_watch_h = stop_watch<std::chrono::hours>;

        stop_watch_h w{};

        static_assert(
            std::is_same<stop_watch_h::duration, std::chrono::hours>::value,
            "");
        static_assert(
            std::is_same<decltype(w.peek()), std::chrono::hours>::value,
            "");
    }
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(test_benchmark)
BOOST_AUTO_TEST_CASE(iteration)
{
    int a{};
    benchmark<std::chrono::seconds>(iterations{552}, [&a]() mutable { ++a; });
    BOOST_TEST(a == 552);
}
BOOST_AUTO_TEST_CASE(void_function)
{
    benchmark<std::chrono::seconds>(iterations{1}, []() {});
}
BOOST_AUTO_TEST_CASE(non_void_function)
{
    benchmark<std::chrono::seconds>(iterations{1}, []() { return 552; });
}
BOOST_AUTO_TEST_CASE(function_with_side_effect)
{
    int i{0};
    benchmark<std::chrono::seconds>(iterations{1}, [&]() mutable {
        ++i;
        do_not_optimize(i);
    });
    BOOST_TEST(i == 1);
}
struct spy_stop_watch
{
    using duration = std::chrono::microseconds;
    explicit spy_stop_watch(auto_start_t)
    {
        _call_seq += "_auto_start_";
    }
    void start()
    {
        _call_seq += "_start_";
    }
    void stop()
    {
        _call_seq += "_stop_";
    }
    duration peek() const noexcept
    {
        _call_seq += "_peek_";
        return duration{};
    }
    static std::string _call_seq;
};
std::string spy_stop_watch::_call_seq{};

BOOST_AUTO_TEST_CASE(uses_given_stop_watch)
{
    benchmark<spy_stop_watch::duration, spy_stop_watch>(iterations{11},
                                                        []() {});
    BOOST_TEST(spy_stop_watch::_call_seq == "_auto_start__stop__peek_");
}
BOOST_AUTO_TEST_CASE(measure_sleep_time)
{
    const auto& sleep_dur{250ms};
    {
        const auto& a{
            benchmark<std::chrono::milliseconds>(iterations{1}, [&sleep_dur]() {
                std::this_thread::sleep_for(sleep_dur);
            })};
        BOOST_TEST(a >= sleep_dur);
    }
    {
        const auto& a{
            benchmark<std::chrono::seconds>(iterations{1}, [&sleep_dur]() {
                std::this_thread::sleep_for(sleep_dur);
            })};
        BOOST_TEST(a >= 0s);
    }
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
}
