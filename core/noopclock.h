#pragma once

#include "core/iclock.h"

namespace avr {
    class NoopClock : public IClock
    {
        public:
            void Pulse() override;
            void ConsumeCycle() override;
    };
}
