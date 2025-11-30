file(GLOB CORE_SOURCES "core/*.cpp" "core/*.hpp")
file(GLOB ENGINE_SOURCES "engine/*.cpp" "engine/*.hpp")
file(GLOB DATA_SOURCES "data/*.cpp" "data/*.hpp")
file(GLOB AI_SOURCES "ai/*.cpp" "ai/*.hpp")
file(GLOB UI_SOURCES "ui/*.cpp" "ui/*.hpp")
file(GLOB NETWORK_SOURCES "network/*.cpp" "network/*.hpp")
file(GLOB SERVER_SOURCES "server/*.cpp" "server/*.hpp")
file(GLOB CLIENT_SOURCES "client/*.cpp" "client/*.hpp")

# Network library (platform-specific)
if(WIN32)
  set(NETWORK_LIBS ws2_32)
else()
  set(NETWORK_LIBS "")
endif()

add_library(battler
  ${CORE_SOURCES}
 ${ENGINE_SOURCES}
    ${DATA_SOURCES}
    ${AI_SOURCES}
    ${UI_SOURCES}
    ${NETWORK_SOURCES}
    ${SERVER_SOURCES}
    ${CLIENT_SOURCES}
)

target_include_directories(battler PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(battler PUBLIC nlohmann_json::nlohmann_json ${NETWORK_LIBS})

add_executable(battler_app main.cpp)
target_link_libraries(battler_app PRIVATE battler)

# Server executable
add_executable(battler_server server_main.cpp)
target_link_libraries(battler_server PRIVATE battler)

# Client executable
add_executable(battler_client client_main.cpp)
target_link_libraries(battler_client PRIVATE battler)

# Network test executable
add_executable(network_test network_test.cpp)
target_link_libraries(network_test PRIVATE battler)
