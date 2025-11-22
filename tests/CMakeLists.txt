include(${CMAKE_SOURCE_DIR}/cmake/FetchCatch2.cmake)

add_executable(tests
  main_test.cpp
  test_damage.cpp
  test_accuracy.cpp
  test_battle.cpp
)

target_include_directories(tests PRIVATE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(tests PRIVATE battler Catch2)

add_test(NAME run_tests COMMAND tests)
