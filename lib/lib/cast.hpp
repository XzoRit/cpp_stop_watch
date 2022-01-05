#pragma once

#include <boost/chrono/duration.hpp>

#include <chrono>

namespace xzr
{
namespace chrono
{
namespace impl
{
template <class ToDur, class Rep, class Period>
ToDur duration_cast(const std::chrono::duration<Rep, Period>& d)
{
    return std::chrono::duration_cast<ToDur>(d);
}
template <class ToDur, class Rep, class Period>
ToDur duration_cast(const boost::chrono::duration<Rep, Period>& d)
{
    return boost::chrono::duration_cast<ToDur>(d);
}
}
}
}
