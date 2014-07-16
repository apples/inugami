#ifndef INUGAMI_INTERFACE_TRACKER_HPP
#define INUGAMI_INTERFACE_TRACKER_HPP

namespace Inugami {
namespace Interface_detail {

class Window;

struct Tracker
{
    bool alive = true;
    Window* window = nullptr;
};

class TrackerHandle
{
    Tracker* tracker;

    public:

        TrackerHandle(Tracker* tp, Window* window);
        TrackerHandle(const TrackerHandle&) = delete;
        TrackerHandle(TrackerHandle&&);
        TrackerHandle& operator=(const TrackerHandle&) = delete;
        TrackerHandle& operator=(TrackerHandle&&);

        ~TrackerHandle();
};

} // namespace Interface_detail
} // namespace Inugami

#endif // INUGAMI_INTERFACE_TRACKER_HPP
