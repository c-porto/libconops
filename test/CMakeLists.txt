include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.15.2
)

FetchContent_MakeAvailable(googletest)

add_executable(cmd_queue_test ./util/cmd_queue_test.cc)

target_link_libraries(cmd_queue_test
  PRIVATE
    GTest::gtest_main
    conops
)

add_test(NAME ConopsTest COMMAND cmd_queue_test)
