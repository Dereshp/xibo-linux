#pragma once

#include <vector>
#include <string>
#include <boost/date_time/posix_time/posix_time.hpp>

const int EMPTY_LAYOUT_ID = 0;

struct DefaultScheduledLayout
{
    int id;
    std::vector<std::string> dependants;
};

struct ScheduledLayout
{
    int scheduleId;
    int id;
    int priority;
    boost::posix_time::ptime startDT;
    boost::posix_time::ptime endDT;
    std::vector<std::string> dependants;
};
