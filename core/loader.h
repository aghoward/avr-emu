#include "core/executioncontext.h"

namespace avr
{
    class Loader
    {
        private:
            void LoadInterruptHandlers(ExecutionContext& ctx) const;
            void LoadToMemory(ExecutionContext& ctx, const std::string& program, uint16_t address) const;

        public:
            ExecutionContext LoadProgram(const std::string& program) const;
    };
}
