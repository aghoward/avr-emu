#pragma once

namespace avr
{
    class IClock
    {
        public:
            virtual ~IClock() {}
            virtual void Pulse() = 0;
            virtual void ConsumeCycle() = 0;
    };
}
