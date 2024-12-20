# Load the debug and release variables
file(GLOB DATA_FILES "${CMAKE_CURRENT_LIST_DIR}/benchmark-*-data.cmake")

foreach(f ${DATA_FILES})
    include(${f})
endforeach()

# Create the targets for all the components
foreach(_COMPONENT ${benchmark_COMPONENT_NAMES} )
    if(NOT TARGET ${_COMPONENT})
        add_library(${_COMPONENT} INTERFACE IMPORTED)
        message(${benchmark_MESSAGE_MODE} "Conan: Component target declared '${_COMPONENT}'")
    endif()
endforeach()

if(NOT TARGET benchmark::benchmark_main)
    add_library(benchmark::benchmark_main INTERFACE IMPORTED)
    message(${benchmark_MESSAGE_MODE} "Conan: Target declared 'benchmark::benchmark_main'")
endif()
# Load the debug and release library finders
file(GLOB CONFIG_FILES "${CMAKE_CURRENT_LIST_DIR}/benchmark-Target-*.cmake")

foreach(f ${CONFIG_FILES})
    include(${f})
endforeach()