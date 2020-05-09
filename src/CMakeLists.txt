set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

set(JSON_BuildTests OFF CACHE INTERNAL "")
FetchContent_Declare(
  nlohmanjson
  GIT_REPOSITORY https://github.com/nlohmann/json
  GIT_TAG        v3.7.3
)
FetchContent_MakeAvailable(nlohmanjson)
 
set(BUILD_CPR_TESTS OFF CACHE INTERNAL "")
set(USE_SYSTEM_CURL ON CACHE INTERNAL "")
FetchContent_Declare(
  cpr
  GIT_REPOSITORY https://github.com/whoshuu/cpr
  GIT_TAG        1.4.0
)
FetchContent_MakeAvailable(cpr)

add_library(jiraclient jira_client.cpp types.cpp utils.cpp ${INCLUDE_DIR}/jira/jira_client.hpp)

target_include_directories(jiraclient PUBLIC ../include)

target_link_libraries(jiraclient PRIVATE spdlog nlohmann_json::nlohmann_json cpr)

target_compile_features(jiraclient PRIVATE cxx_std_11)