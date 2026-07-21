#include <aquamarine/backend/Backend.hpp>
#include <aquamarine/buffer/Buffer.hpp>
#include <aquamarine/output/Output.hpp>
#include <hyprutils/memory/SharedPtr.hpp>
#include <hyprutils/signal/Signal.hpp>
#include <vector>
#include "shared.hpp"

using namespace Aquamarine;
using namespace Hyprutils::Memory;
using namespace Hyprutils::Signal;
#define SP CSharedPointer

class CTestBuffer : public IBuffer {
  public:
    virtual eBufferCapability caps() {
        return BUFFER_CAPABILITY_NONE;
    }

    virtual eBufferType type() {
        return BUFFER_TYPE_MISC;
    }

    virtual void update(const Hyprutils::Math::CRegion& damage) {
        ;
    }

    virtual bool isSynchronous() {
        return true;
    }

    virtual bool good() {
        return true;
    }
};

int main() {
    int                           ret = 0;

    SBackendImplementationOptions implementation;
    implementation.backendType        = AQ_BACKEND_HEADLESS;
    implementation.backendRequestMode = AQ_BACKEND_REQUEST_MANDATORY;

    auto        backend = CBackend::create({implementation}, SBackendOptions{});
    SP<IOutput> output;
    auto        newOutput = backend->events.newOutput.listen([&output](const SP<IOutput>& eventOutput) { output = eventOutput; });

    EXPECT(backend->getImplementations().front()->createOutput("presentation-test"), true);
    EXPECT(!!output, true);

    std::vector<uint64_t> presentationIDs;
    auto                  present = output->events.present.listen([&presentationIDs](const IOutput::SPresentEvent& event) { presentationIDs.emplace_back(event.presentationID); });
    auto                  buffer  = makeShared<CTestBuffer>();

    output->state->setBuffer(buffer);
    output->state->setPresentationID(42);
    EXPECT(output->commit(), true);
    EXPECT(presentationIDs.back(), 42);

    output->state->setBuffer(buffer);
    EXPECT(output->commit(), true);
    EXPECT(presentationIDs.back(), 0);

    output->state->setBuffer(buffer);
    output->state->setPresentationID(84);
    EXPECT(output->test(), true);
    EXPECT(output->commit(), true);
    EXPECT(presentationIDs.back(), 84);

    return ret;
}
