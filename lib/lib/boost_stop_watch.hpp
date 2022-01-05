#pragma once

#include <lib/stop_watch.hpp>

#include <boost/chrono/config.hpp>
#if defined(BOOST_CHRONO_HAS_THREAD_CLOCK)
#include <boost/chrono/thread_clock.hpp>
#endif
#if defined(BOOST_CHRONO_HAS_PROCESS_CLOCKS)
#include <boost/chrono/process_cpu_clocks.hpp>
#endif

#include <boost/chrono/duration.hpp>
#include <boost/chrono/system_clocks.hpp>

namespace xzr
{
namespace chrono
{
namespace boost_chrono
{
inline namespace v1
{
//// \brief Stopwatch using boost::chrono::steady_clock as a source.
template <class Precision>
using system_stop_watch =
    basic_stop_watch<boost::chrono::system_clock, Precision>;
#if defined(BOOST_CHRONO_HAS_CLOCK_STEADY)
//// \brief Stopwatch using boost::chrono::steady_clock as a source.
template <class Precision>
using stop_watch = basic_stop_watch<boost::chrono::steady_clock, Precision>;
#endif
#if defined(BOOST_CHRONO_HAS_THREAD_CLOCK)
//// \brief Stopwatch using boost::chrono::thread_clock as a source.
template <class Precision>
using thread_stop_watch =
    basic_stop_watch<boost::chrono::thread_clock, Precision>;
#endif
#if defined(BOOST_CHRONO_HAS_PROCESS_CLOCK)
//// \brief Stopwatch using boost::chrono::process_real_cpu_clock as a source.
template <class Precision>
using process_real_stop_watch =
    basic_stop_watch<boost::chrono::process_real_cpu_clock, Precision>;
//// \brief Stopwatch using boost::chrono::process_user_cpu_clock as a source.
template <class Precision>
using process_user_stop_watch =
    basic_stop_watch<boost::chrono::process_user_cpu_clock, Precision>;
//// \brief Stopwatch using boost::chrono::process_system_cpu_clock as a source.
template <class Precision>
using process_system_stop_watch =
    basic_stop_watch<boost::chrono::process_system_cpu_clock, Precision>;
//// \brief Stopwatch using boost::chrono::process_cpu_clock as a source.
template <class Precision>
using process_cpu_stop_watch =
    basic_stop_watch<boost::chrono::process_cpu_clock, Precision>;
#endif
}
}
}
}
