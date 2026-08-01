#include <aquamarine/backend/FrameScheduler.hpp>
#include "shared.hpp"

using namespace Aquamarine;

int main() {
    int             ret = 0;

    CFrameScheduler scheduler;
    EXPECT(scheduler.takeDeferredSchedule(), false);

    {
        CFrameRunningGuard frameRunning(scheduler);
        scheduler.deferSchedule();
        scheduler.deferSchedule();
        EXPECT(scheduler.canSchedule(), false);
    }

    EXPECT(scheduler.takeDeferredSchedule(), true);
    EXPECT(scheduler.takeDeferredSchedule(), false);
    EXPECT(scheduler.canSchedule(), true);

    scheduler.setFrameScheduled(true);
    scheduler.deferSchedule();
    EXPECT(scheduler.takeDeferredSchedule() && scheduler.canSchedule(), false);
    EXPECT(scheduler.takeDeferredSchedule(), false);
    scheduler.setFrameScheduled(false);

    scheduler.onFrameSubmitted();
    scheduler.deferSchedule();
    EXPECT(scheduler.takeDeferredSchedule() && scheduler.canSchedule(), false);
    EXPECT(scheduler.takeDeferredSchedule(), false);

    scheduler.deferSchedule();
    scheduler.invalidate();
    EXPECT(scheduler.takeDeferredSchedule(), false);

    return ret;
}
