#include "LayoutSchedule.hpp"

const size_t FIRST_ITEM_INDEX = 0;

LayoutSchedule::LayoutSchedule() :
    m_nextLayoutIndex{FIRST_ITEM_INDEX}
{
}

void LayoutSchedule::updateDependants(std::vector<std::string>&& globalDependants)
{
    m_globalDependants = std::move(globalDependants);
}

const std::vector<std::string>& LayoutSchedule::globalDependants() const
{
    return m_globalDependants;
}

void LayoutSchedule::updateDefaultLayout(DefaultScheduledLayout&& defaultLayout)
{
    m_defaultLayout = std::move(defaultLayout);
}

const DefaultScheduledLayout& LayoutSchedule::defaultLayout() const
{
    return m_defaultLayout;
}

void LayoutSchedule::updateScheduledLayouts(std::vector<ScheduledLayout>&& layouts)
{
    m_scheduledLayouts = std::move(layouts);

    std::stable_sort(m_scheduledLayouts.begin(), m_scheduledLayouts.end(), [](const auto& first, const auto& second){
        return first.priority > second.priority;
    });
}

const std::vector<ScheduledLayout>& LayoutSchedule::layouts() const
{
    return m_scheduledLayouts;
}

const ScheduledLayout& LayoutSchedule::nextScheduledLayout()
{
    size_t layoutIndex = m_nextLayoutIndex;

    m_nextLayoutIndex = increaseLayoutIndex(layoutIndex);

    return m_scheduledLayouts[layoutIndex];
}

void LayoutSchedule::updateGeneratedTime(const std::string& dt)
{
    m_generatedTime = dt;
}

std::string LayoutSchedule::generatedTime() const
{
    return m_generatedTime;
}

size_t LayoutSchedule::scheduledLayoutsCount() const
{
    return m_scheduledLayouts.size();
}

size_t LayoutSchedule::increaseLayoutIndex(std::size_t index) const
{
    size_t nextLayoutIndex = index + 1;

    if(nextLayoutIndex >= m_scheduledLayouts.size())
        return FIRST_ITEM_INDEX;

    return nextLayoutIndex;
}
