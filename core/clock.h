#pragma once

#include "core/iclock.h"

#include <condition_variable>

namespace avr {
    class Clock : public IClock
    {
        private:
            std::mutex _m;
            std::condition_variable _cv;

        public:
            void Pulse() override;
            void ConsumeCycle() override;
    };
}
