#include <aquamarine/backend/DRM.hpp>
#include <cstdint>
#include "shared.hpp"

using namespace Aquamarine;

namespace Aquamarine {
    class CPresentationClockTestAccess {
      public:
        static int classify(const timespec& timestamp, const timespec& monotonicNow, const timespec& monotonicRawNow) {
            return static_cast<int>(CDRMBackend::classifyPresentationClock(timestamp, monotonicNow, monotonicRawNow));
        }

        static timespec convert(const timespec& timestamp, const timespec& monotonicNow, const timespec& monotonicRawNow) {
            return CDRMBackend::convertMonotonicRawToMonotonic(timestamp, monotonicNow, monotonicRawNow);
        }

        static int monotonic() {
            return static_cast<int>(CDRMBackend::ePresentationClock::MONOTONIC);
        }

        static int monotonicRaw() {
            return static_cast<int>(CDRMBackend::ePresentationClock::MONOTONIC_RAW);
        }

        static int unknown() {
            return static_cast<int>(CDRMBackend::ePresentationClock::UNKNOWN);
        }
    };
}

static int64_t toNs(const timespec& timestamp) {
    return timestamp.tv_sec * 1'000'000'000LL + timestamp.tv_nsec;
}

int main() {
    int            ret = 0;

    const timespec monotonicNow    = {.tv_sec = 1000, .tv_nsec = 300'000'000};
    const timespec monotonicRawNow = {.tv_sec = 1000, .tv_nsec = 0};
    const timespec rawEvent        = {.tv_sec = 999, .tv_nsec = 993'000'000};
    const timespec monotonicEvent  = {.tv_sec = 1000, .tv_nsec = 293'000'000};

    EXPECT(CPresentationClockTestAccess::classify(rawEvent, monotonicNow, monotonicRawNow), CPresentationClockTestAccess::monotonicRaw());
    EXPECT(CPresentationClockTestAccess::classify(monotonicEvent, monotonicNow, monotonicRawNow), CPresentationClockTestAccess::monotonic());

    const timespec equivalentRawNow = {.tv_sec = 1000, .tv_nsec = 299'500'000};
    EXPECT(CPresentationClockTestAccess::classify(monotonicEvent, monotonicNow, equivalentRawNow), CPresentationClockTestAccess::unknown());

    const auto converted = CPresentationClockTestAccess::convert(rawEvent, monotonicNow, monotonicRawNow);
    EXPECT(toNs(converted), toNs(monotonicEvent));

    const timespec negativeOffsetMonotonic = {.tv_sec = 999, .tv_nsec = 900'000'000};
    const auto     negativeOffset          = CPresentationClockTestAccess::convert(monotonicEvent, negativeOffsetMonotonic, monotonicNow);
    EXPECT(toNs(negativeOffset), 999'893'000'000LL);

    const timespec borrowEvent = {.tv_sec = 10, .tv_nsec = 50'000'000};
    const timespec borrowMono  = {.tv_sec = 20, .tv_nsec = 100'000'000};
    const timespec borrowRaw   = {.tv_sec = 20, .tv_nsec = 900'000'000};
    const auto     borrowed    = CPresentationClockTestAccess::convert(borrowEvent, borrowMono, borrowRaw);
    EXPECT(borrowed.tv_sec, 9);
    EXPECT(borrowed.tv_nsec, 250'000'000L);

    const timespec changedMonotonic = {.tv_sec = 1000, .tv_nsec = 301'000'000};
    const auto     changedOffset    = CPresentationClockTestAccess::convert(rawEvent, changedMonotonic, monotonicRawNow);
    EXPECT(toNs(changedOffset) - toNs(converted), 1'000'000LL);

    return ret;
}
