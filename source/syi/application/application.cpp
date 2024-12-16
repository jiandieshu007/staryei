#include "application.hpp"

namespace syi {

void Application::run( const ApplicationConfiguration& configuration ) {

    create( configuration );
    main_loop();
    destroy();
}

} // namespace syi