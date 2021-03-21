#include <lib/lib.hpp>

#include <boost/test/unit_test.hpp>

#include <chrono>
#include <iostream>
#include <thread>

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
BOOST_AUTO_TEST_SUITE(test_chrono)
BOOST_AUTO_TEST_SUITE(test_stop_watch)
BOOST_AUTO_TEST_CASE(not_running_on_construction)
{
    xzr::chrono::stop_watch w{};
    BOOST_TEST(!w.running());
    BOOST_TEST(w.peek() == xzr::chrono::stop_watch::duration::zero());
}
BOOST_AUTO_TEST_CASE(construction_with_auto_start)
{
    xzr::chrono::stop_watch w{xzr::chrono::auto_start};
    BOOST_TEST(w.running());
    BOOST_TEST(w.peek() > xzr::chrono::stop_watch::duration::zero());
}
BOOST_AUTO_TEST_CASE(start)
{
    xzr::chrono::stop_watch w{};
    w.start();
    BOOST_TEST(w.running());
    const auto a{w.peek()};
    BOOST_TEST(a > xzr::chrono::stop_watch::duration::zero());
}
BOOST_AUTO_TEST_CASE(stop)
{
    xzr::chrono::stop_watch w{xzr::chrono::auto_start};
    w.stop();
    BOOST_TEST(!w.running());
    BOOST_TEST(w.peek() > xzr::chrono::stop_watch::duration::zero());
}
BOOST_AUTO_TEST_CASE(start_running_stop_watch)
{
    xzr::chrono::stop_watch w{xzr::chrono::auto_start};
    BOOST_REQUIRE_THROW(w.start(), std::runtime_error);
}
BOOST_AUTO_TEST_CASE(stop_stopped_stop_watch)
{
    xzr::chrono::stop_watch w{};
    BOOST_REQUIRE_THROW(w.stop(), std::runtime_error);
}
BOOST_AUTO_TEST_CASE(reset_stopped_watch)
{
    xzr::chrono::stop_watch w{xzr::chrono::auto_start};
    w.stop();
    BOOST_TEST(w.peek() > xzr::chrono::stop_watch::duration::zero());
    w.reset();
    BOOST_TEST(w.peek() == xzr::chrono::stop_watch::duration::zero());
}
BOOST_AUTO_TEST_CASE(reset_running_watch)
{
    xzr::chrono::stop_watch w{xzr::chrono::auto_start};
    const auto a{w.peek()};
    w.reset();
    const auto b{w.peek()};
    BOOST_TEST(b > xzr::chrono::stop_watch::duration::zero());
    BOOST_TEST(b < a);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
} // namespace
