set(PROJECT_NAME publish-mteam)
set(OUTPUT_DIR_NAME "${CMAKE_BINARY_DIR}/../bin")

include_directories("./" ${CMAKE_SOURCE_DIR}/3rdparty/include ${CMAKE_SOURCE_DIR}/src/utils)

find_package(CURL REQUIRED)
find_package(OpenSSL REQUIRED)

file(GLOB SOURCE_SRC *.cpp ${CMAKE_SOURCE_DIR}/src/utils/*.cpp)
add_executable(${PROJECT_NAME} ${SOURCE_SRC})

if (MSVC)
    set_property(TARGET ${PROJECT_NAME} PROPERTY
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
else ()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -static -static-libgcc")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static -static-libgcc -static-libstdc++")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
endif ()

set(OUTPUT_RELEASE_DIR "${OUTPUT_DIR_NAME}/release/x86")
set(OUTPUT_DEBUG_DIR "${OUTPUT_DIR_NAME}/debug/x86")

if (MSVC)
    if (CMAKE_CL_64)
        set(OUTPUT_RELEASE_DIR "${OUTPUT_DIR_NAME}/release/x64")
        set(OUTPUT_DEBUG_DIR "${OUTPUT_DIR_NAME}/debug/x64")
    endif ()
endif ()

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(OUTPUT_RELEASE_DIR "${OUTPUT_DIR_NAME}/release/x64")
  set(OUTPUT_DEBUG_DIR "${OUTPUT_DIR_NAME}/debug/x64")
endif()

set_target_properties(${PROJECT_NAME}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_RELEASE_DIR}
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_DEBUG_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
        CURL::libcurl
)