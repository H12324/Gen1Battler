add_library(battler
  core/battle.cpp
  core/pokemon.cpp
  core/move.cpp
  engine/damage.cpp
  engine/accuracy.cpp
  data/loader.cpp
  ai/simple_ai.cpp
  ui/cli.cpp
)

target_include_directories(battler PUBLIC
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/src
)

add_executable(battler_app main.cpp)
target_link_libraries(battler_app PRIVATE battler)
