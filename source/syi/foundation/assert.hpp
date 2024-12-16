#pragma once

#include "foundation/log.hpp"

namespace syi {

    #define RASSERT( condition )      if (!(condition)) { rprint(syi_FILELINE("FALSE\n")); syi_DEBUG_BREAK }
#if defined(_MSC_VER)
    #define RASSERTM( condition, message, ... ) if (!(condition)) { rprint(syi_FILELINE(syi_CONCAT(message, "\n")), __VA_ARGS__); syi_DEBUG_BREAK }
#else
    #define RASSERTM( condition, message, ... ) if (!(condition)) { rprint(syi_FILELINE(syi_CONCAT(message, "\n")), ## __VA_ARGS__); syi_DEBUG_BREAK }
#endif

} // namespace syi
