# https://www.npmjs.com/package/cmake-js#general
if (CMAKE_JS_SRC)
    add_library(cmake_js STATIC ${CMAKE_JS_SRC})
else()
    add_library(cmake_js INTERFACE)
endif()
target_include_directories(cmake_js INTERFACE ${CMAKE_JS_INC})
target_link_libraries(cmake_js ${CMAKE_JS_LIB})

# https://www.npmjs.com/package/cmake-js#n-api-and-node-addon-api
add_library(node_addon_api INTERFACE)
find_program(NODE_EXECUTABLE NAMES node HINTS /usr)
execute_process(COMMAND ${NODE_EXECUTABLE} -p "require('node-addon-api').include"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE NODE_ADDON_API_DIR
        )
string(REPLACE "\n" "" NODE_ADDON_API_DIR "${NODE_ADDON_API_DIR}")
string(REPLACE "\"" "" NODE_ADDON_API_DIR "${NODE_ADDON_API_DIR}")
target_include_directories(node_addon_api INTERFACE ${NODE_ADDON_API_DIR})
target_link_libraries(node_addon_api INTERFACE cmake_js)

# Supports Node version >= 12.11.0 https://nodejs.org/api/n-api.html#n_api_n_api_version_matrix
target_compile_definitions(node_addon_api INTERFACE NAPI_VERSION=5)

add_library(node_js_converter node_js_converter.cpp node_js_converter.hpp)
set_target_properties(node_js_converter PROPERTIES PREFIX "" SUFFIX ".node")
target_link_libraries(node_js_converter PUBLIC node_addon_api hawktracer_client)
