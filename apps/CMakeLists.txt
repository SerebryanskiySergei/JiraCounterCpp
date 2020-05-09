add_executable(counter counter.cpp)

target_compile_features(counter PRIVATE cxx_std_11)

target_link_libraries(counter jiraclient nlohmann_json::nlohmann_json spdlog)

configure_file("params.json" "${CMAKE_CURRENT_BINARY_DIR}/params.json" COPYONLY)
