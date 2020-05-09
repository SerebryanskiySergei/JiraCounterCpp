# Unit-test library
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.10.0
)
FetchContent_MakeAvailable(googletest)

add_executable(jira_unit jira-unit.cpp)
target_link_libraries(jira_unit PRIVATE jiraclient gtest gtest_main)

add_test(NAME jira-test COMMAND jira_unit)