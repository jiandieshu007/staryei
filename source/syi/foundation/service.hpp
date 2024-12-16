#pragma once

#include "foundation/platform.hpp"

namespace syi {

    struct Service {

        virtual void                        init( void* configuration ) { }
        virtual void                        shutdown() { }

    }; // struct Service

    #define syi_DECLARE_SERVICE(Type)        static Type* instance();

} // namespace syi
