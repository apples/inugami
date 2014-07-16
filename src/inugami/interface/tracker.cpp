#include "tracker.hpp"

#include <utility>

namespace Inugami {
namespace Interface_detail {

TrackerHandle::TrackerHandle(Tracker* tp, Window* window)
    : tracker(tp)
{
    tp->window = window;
}

TrackerHandle::TrackerHandle(TrackerHandle&& other)
    : tracker(std::exchange(other.tracker, nullptr))
{}

TrackerHandle& TrackerHandle::operator=(TrackerHandle&& other)
{
    this->~TrackerHandle(); // TODO: gcc bug 12333
    tracker = std::exchange(other.tracker, nullptr);
    return *this;
}

TrackerHandle::~TrackerHandle()
{
    if (tracker)
    {
        tracker->alive = false;
        tracker->window = nullptr;
        tracker = nullptr;
    }
}

} // namespace Interface_detail
} // namespace Inugami
