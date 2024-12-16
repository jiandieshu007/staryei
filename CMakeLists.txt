cmake_minimum_required(VERSION 3.0.0)

project(syiEngine VERSION 0.1.0)

find_package(Vulkan REQUIRED)

if (UNIX)
find_package(SDL2 REQUIRED)
endif()

add_library(syiFoundation STATIC
    source/syi/foundation/array.hpp
    source/syi/foundation/assert.cpp
    source/syi/foundation/assert.hpp
    source/syi/foundation/bit.cpp
    source/syi/foundation/bit.hpp
    source/syi/foundation/blob_serialization.cpp
    source/syi/foundation/blob_serialization.hpp
    source/syi/foundation/blob.hpp
    source/syi/foundation/camera.cpp
    source/syi/foundation/camera.hpp
    source/syi/foundation/color.cpp
    source/syi/foundation/color.hpp
    source/syi/foundation/data_structures.cpp
    source/syi/foundation/data_structures.hpp
    source/syi/foundation/file.cpp
    source/syi/foundation/file.hpp
    source/syi/foundation/gltf.cpp
    source/syi/foundation/gltf.hpp
    source/syi/foundation/hash_map.hpp
    source/syi/foundation/log.cpp
    source/syi/foundation/log.hpp
    source/syi/foundation/memory_utils.hpp
    source/syi/foundation/memory.cpp
    source/syi/foundation/memory.hpp
    source/syi/foundation/numerics.cpp
    source/syi/foundation/numerics.hpp
    source/syi/foundation/platform.hpp
    source/syi/foundation/process.cpp
    source/syi/foundation/process.hpp
    source/syi/foundation/relative_data_structures.hpp
    source/syi/foundation/resource_manager.cpp
    source/syi/foundation/resource_manager.hpp
    source/syi/foundation/serialization.cpp
    source/syi/foundation/serialization.hpp
    source/syi/foundation/service_manager.cpp
    source/syi/foundation/service_manager.hpp
    source/syi/foundation/service.cpp
    source/syi/foundation/service.hpp
    source/syi/foundation/string.cpp
    source/syi/foundation/string.hpp
    source/syi/foundation/time.cpp
    source/syi/foundation/time.hpp
 )

set_property(TARGET syiFoundation PROPERTY CXX_STANDARD 17)

target_compile_definitions(syiFoundation PRIVATE
    _CRT_SECURE_NO_WARNINGS

    TRACY_ENABLE
    TRACY_ON_DEMAND
    TRACY_NO_SYSTEM_TRACING
)

target_include_directories(syiFoundation PRIVATE
    source
    source/syi
)

add_library(syiApp STATIC
    source/syi/application/application.cpp
    source/syi/application/application.hpp
    source/syi/application/game_camera.cpp
    source/syi/application/game_camera.hpp
    source/syi/application/game_application.cpp
    source/syi/application/game_application.hpp
    source/syi/application/input.cpp
    source/syi/application/input.hpp
    source/syi/application/keys.cpp
    source/syi/application/keys.hpp
    source/syi/application/window.cpp
    source/syi/application/window.hpp
)

set_property(TARGET syiApp PROPERTY CXX_STANDARD 17)

target_compile_definitions(syiApp PRIVATE
    _CRT_SECURE_NO_WARNINGS

    TRACY_ENABLE
    TRACY_ON_DEMAND
    TRACY_NO_SYSTEM_TRACING
)

target_include_directories(syiApp PRIVATE
    source
    source/syi
    )

if (WIN32)
    target_include_directories(syiApp PRIVATE
        binaries/SDL2-2.0.18/include)
else()
    target_include_directories(syiApp PRIVATE
        $${SDL2_INCLUDE_DIRS})
endif()

add_library(syiExternal STATIC
    source/external/tlsf.c

    source/external/json.hpp

    source/external/imgui/imconfig.h
    source/external/imgui/imgui_draw.cpp
    source/external/imgui/imgui_impl_sdl.cpp
    source/external/imgui/imgui_impl_sdl.h
    source/external/imgui/imgui_internal.h
    source/external/imgui/imgui_memory_editor.h
    source/external/imgui/imgui_tables.cpp
    source/external/imgui/imgui_widgets.cpp
    source/external/imgui/imgui.cpp
    source/external/imgui/imgui.h
    source/external/imgui/imstb_rectpack.h
    source/external/imgui/imstb_textedit.h
    source/external/imgui/imstb_truetype.h
    source/external/imgui/stb_image.h
    source/external/imgui/TextEditor.cpp
    source/external/imgui/TextEditor.h

    source/external/tracy/tracy/Tracy.hpp
    source/external/tracy/tracy/TracyVulkan.hpp
    source/external/tracy/TracyClient.cpp

    source/external/enkiTS/LockLessMultiReadPipe.h
    source/external/enkiTS/TaskScheduler.cpp
    source/external/enkiTS/TaskScheduler.h

    source/external/meshoptimizer/allocator.cpp
    source/external/meshoptimizer/clusterizer.cpp
    source/external/meshoptimizer/indexcodec.cpp
    source/external/meshoptimizer/indexgenerator.cpp
    source/external/meshoptimizer/meshoptimizer.h
    source/external/meshoptimizer/overdrawanalyzer.cpp
    source/external/meshoptimizer/overdrawoptimizer.cpp
    source/external/meshoptimizer/simplifier.cpp
    source/external/meshoptimizer/spatialorder.cpp
    source/external/meshoptimizer/stripifier.cpp
    source/external/meshoptimizer/vcacheanalyzer.cpp
    source/external/meshoptimizer/vcacheoptimizer.cpp
    source/external/meshoptimizer/vertexcodec.cpp
    source/external/meshoptimizer/vertexfilter.cpp
    source/external/meshoptimizer/vfetchanalyzer.cpp
    source/external/meshoptimizer/vfetchoptimizer.cpp
)

if (WIN32)
    target_sources(syiExternal PRIVATE
        source/external/StackWalker.cpp)
endif()

target_compile_definitions(syiExternal PRIVATE
    TRACY_ENABLE
    TRACY_ON_DEMAND
    TRACY_NO_SYSTEM_TRACING
)

if (WIN32)
    target_include_directories(syiExternal PRIVATE
        binaries/SDL2-2.0.18/include
        source/syi
    )
else()
    target_link_libraries(syiExternal PRIVATE
        dl
    )
    target_include_directories(syiExternal PRIVATE
        ${SDL2_INCLUDE_DIRS})
endif()

set_property(TARGET syiExternal PROPERTY CXX_STANDARD 17)

add_subdirectory(source/staryei)
