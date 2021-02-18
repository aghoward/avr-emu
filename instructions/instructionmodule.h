#pragma once

#include "cdif/cdif.h"

namespace avr { 
    class InstructionModule: cdif::IModule {
        public:
            void load(cdif::Container &) override;
    };
}
