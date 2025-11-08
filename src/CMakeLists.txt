file(GLOB CORE_SOURCES "core/*.cpp" "core/*.hpp")
file(GLOB ENGINE_SOURCES "engine/*.cpp" "engine/*.hpp")
file(GLOB DATA_SOURCES "data/*.cpp" "data/*.hpp")
file(GLOB AI_SOURCES "ai/*.cpp" "ai/*.hpp")
file(GLOB UI_SOURCES "ui/*.cpp" "ui/*.hpp")

add_library(battler
  ${CORE_SOURCES}
 ${ENGINE_SOURCES}
    ${DATA_SOURCES}
    ${AI_SOURCES}
    ${UI_SOURCES}
)

target_include_directories(battler PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

add_executable(battler_app main.cpp)
target_link_libraries(battler_app PRIVATE battler)
