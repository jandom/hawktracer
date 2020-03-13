add_library(node_js_converter node_js_converter.cpp node_js_converter.hpp)
target_link_libraries(node_js_converter hawktracer_client)

find_program(NODE_EXECUTABLE NAMES node HINTS /usr)
execute_process(COMMAND ${NODE_EXECUTABLE} -p "require('node-addon-api').include"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE NODE_ADDON_API_DIR
        )
string(REPLACE "\n" "" NODE_ADDON_API_DIR "${NODE_ADDON_API_DIR}")
string(REPLACE "\"" "" NODE_ADDON_API_DIR "${NODE_ADDON_API_DIR}")
message(STATUS ")target_include_directories(node_js_converter PRIVATE ${NODE_ADDON_API_DIR})")
target_include_directories(node_js_converter PRIVATE ${NODE_ADDON_API_DIR} ${NODE_ADDON_API_DIR}/external-napi)