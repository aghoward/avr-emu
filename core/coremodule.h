#pragma once

#include "cdif/cdif.h"

namespace avr
{
    class CoreModule : public cdif::IModule
    {
        public:
            void load(cdif::Container& ctx) override;
    };
}
