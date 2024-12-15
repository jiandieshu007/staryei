add_executable(staryei
    graphics/GpuResource.hpp
    graphics/GpuResource.cpp
    graphics/GpuEnum.hpp

    main.cpp
)


set_property(TARGET staryei PROPERTY CXX_STANDARD 20)

if (WIN32)
    target_compile_definitions(staryei PRIVATE
        _CRT_SECURE_NO_WARNINGS
        WIN32_LEAN_AND_MEAN
        NOMINMAX)
endif()

target_compile_definitions(staryei PRIVATE
    RAPTOR_WORKING_FOLDER="${CMAKE_CURRENT_SOURCE_DIR}"
    RAPTOR_SHADER_FOLDER="${CMAKE_CURRENT_SOURCE_DIR}/shaders/"
)

target_compile_definitions(staryei PRIVATE
    TRACY_ENABLE
    TRACY_ON_DEMAND
    TRACY_NO_SYSTEM_TRACING
)

target_include_directories(staryei PRIVATE
    .
    ..
    ../raptor
    ../../binaries/assimp/include
    ${Vulkan_INCLUDE_DIRS}
)

if (WIN32)
    target_link_directories(staryei PRIVATE
        ../../binaries/assimp/windows/bin
        ../../binaries/assimp/windows/lib
        ../../binaries/SDL2-2.0.18/lib/x64
    )

    target_include_directories(staryei PRIVATE
        ../../binaries/SDL2-2.0.18/include)
else()
    target_link_directories(staryei PRIVATE
        ../../binaries/assimp/linux/lib)

    target_include_directories(staryei PRIVATE
        ${SDL2_INCLUDE_DIRS})
endif()

if (WIN32)
    target_link_libraries(staryei PRIVATE
        assimp-vc142-mt
        SDL2)
else()
    target_link_libraries(staryei PRIVATE
        dl
        pthread
        assimp
        SDL2::SDL2)
endif()

target_link_libraries(staryei PRIVATE
    RaptorFoundation
    RaptorExternal
    RaptorApp
    ${Vulkan_LIBRARIES}
)

if (WIN32)
    set(DLLS_TO_COPY
        ${CMAKE_CURRENT_SOURCE_DIR}/../../binaries/SDL2-2.0.18/lib/x64/SDL2.dll
        ${CMAKE_CURRENT_SOURCE_DIR}/../../binaries/assimp/windows/bin/assimp-vc142-mt.dll
    )

    foreach(DLL ${DLLS_TO_COPY})
        add_custom_command(TARGET staryei POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy ${DLL}  $<TARGET_FILE_DIR:staryei>
            VERBATIM
        )
    endforeach()
endif()
