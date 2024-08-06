set(UR_OFFLOAD_ADAPTER_DIR "${CMAKE_CURRENT_SOURCE_DIR}" CACHE PATH "Offload adapter directory") # TODO

set(TARGET_NAME ur_adapter_offload)

set(UR_OFFLOAD_LIB_DIR "" CACHE PATH "Path to the directory containing libomptarget.so etc")
if (UR_OFFLOAD_LIB_DIR STREQUAL "")
    message(FATAL_ERROR "UR_OFFLOAD_LIB_DIR must be defined for the Offload adapter")
endif()

add_ur_adapter(${TARGET_NAME}
        SHARED
        ${CMAKE_CURRENT_SOURCE_DIR}/adapter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ur_interface_loader.cpp
)

set_target_properties(${TARGET_NAME} PROPERTIES
        VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}"
        SOVERSION "${PROJECT_VERSION_MAJOR}"
)

target_link_libraries(${TARGET_NAME} PRIVATE
        ${PROJECT_NAME}::headers
        ${PROJECT_NAME}::common
        ${PROJECT_NAME}::umf
        ${UR_OFFLOAD_LIB_DIR}/libomptarget.so
)

target_include_directories(${TARGET_NAME} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/../../"
)
