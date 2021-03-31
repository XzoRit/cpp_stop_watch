#include <lib/lib.hpp>

#include <boost/test/unit_test.hpp>

#include <chrono>
#include <iostream>

namespace std
{
namespace chrono
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
ostream& operator<<(ostream& o, nanoseconds ns)
{
    o << ns.count() << "ns";
    return o;
}
} // namespace chrono
} // namespace std
namespace
{
using namespace std::chrono_literals;
using xzr::chrono::auto_start;
using xzr::chrono::auto_start_t;
using xzr::chrono::benchmark;
using xzr::chrono::iterations;
using xzr::chrono::stop_watch;
BOOST_AUTO_TEST_SUITE(test_chrono)
BOOST_AUTO_TEST_SUITE(test_stop_watch)
BOOST_AUTO_TEST_CASE(not_running_on_construction)
{
    stop_watch w{};
    BOOST_TEST(!w.is_running());
    BOOST_TEST(w.peek() == stop_watch::duration::zero());
}
BOOST_AUTO_TEST_CASE(construction_with_auto_start)
{
    stop_watch w{auto_start};
    BOOST_TEST(w.is_running());
    BOOST_TEST(w.peek() > stop_watch::duration::zero());
}
BOOST_AUTO_TEST_CASE(start)
{
    stop_watch w{};
    w.start();
    BOOST_TEST(w.is_running());
    const auto a{w.peek()};
    BOOST_TEST(a > stop_watch::duration::zero());
}
BOOST_AUTO_TEST_CASE(stop)
{
    stop_watch w{auto_start};
    w.stop();
    BOOST_TEST(!w.is_running());
    BOOST_TEST(w.peek() > stop_watch::duration::zero());
}
BOOST_AUTO_TEST_CASE(start_running_stop_watch)
{
    stop_watch w{auto_start};
    BOOST_REQUIRE_THROW(w.start(), std::runtime_error);
}
BOOST_AUTO_TEST_CASE(stop_stopped_stop_watch)
{
    stop_watch w{};
    BOOST_REQUIRE_THROW(w.stop(), std::runtime_error);
}
BOOST_AUTO_TEST_CASE(reset_stopped_watch)
{
    stop_watch w{auto_start};
    w.stop();
    BOOST_TEST(w.peek() > stop_watch::duration::zero());
    w.reset();
    BOOST_TEST(w.peek() == stop_watch::duration::zero());
}
BOOST_AUTO_TEST_CASE(reset_running_watch)
{
    stop_watch w{auto_start};
    const auto a{w.peek()};
    w.reset();
    const auto b{w.peek()};
    BOOST_TEST(b > stop_watch::duration::zero());
    BOOST_TEST(b < a);
}
BOOST_AUTO_TEST_CASE(peek)
{
    stop_watch w{};
    const auto a{w.peek()};
    BOOST_TEST(a == stop_watch::duration::zero());
    w.start();
    const auto b{w.peek()};
    BOOST_TEST(b > a);
    const auto c{w.peek()};
    BOOST_TEST(c > b);
    w.stop();
    const auto d{w.peek()};
    const auto e{w.peek()};
    BOOST_TEST(d == e);
    w.reset();
    BOOST_TEST(w.peek() == stop_watch::duration::zero());
}
BOOST_AUTO_TEST_CASE(peek_with_duration)
{
    stop_watch w{};
    static_assert(std::is_same<decltype(w.peek()), stop_watch::duration>::value, "");
    static_assert(std::is_same<decltype(w.peek<std::chrono::milliseconds>()), std::chrono::milliseconds>::value, "");
    static_assert(std::is_same<decltype(w.peek<std::chrono::hours>()), std::chrono::hours>::value, "");
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(test_benchmark)
BOOST_AUTO_TEST_CASE(iteration)
{
    int a{};
    benchmark(iterations{552}, [&a]() mutable { ++a; });
    BOOST_TEST(a == 552);
}
BOOST_AUTO_TEST_CASE(void_function)
{
    benchmark(iterations{1}, []() {});
}
BOOST_AUTO_TEST_CASE(non_void_function)
{
    benchmark(iterations{1}, []() { return 552; });
}
BOOST_AUTO_TEST_CASE(uses_given_stop_watch)
{
    struct spy_stop_watch
    {
        using duration = std::chrono::microseconds;
        explicit spy_stop_watch(auto_start_t)
        {
        }
        duration peek() const noexcept
        {
            return duration{552};
        }
    };
    BOOST_TEST((benchmark<spy_stop_watch::duration, spy_stop_watch>(iterations{1}, []() {})) ==
               spy_stop_watch::duration{552});
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
} // namespace
