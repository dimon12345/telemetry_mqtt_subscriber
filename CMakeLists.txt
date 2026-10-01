cmake_minimum_required(VERSION 3.14)

project(mqtt_subscriber CXX)
set(CMAKE_CXX_STANDARD 17)

find_package(PahoMqttCpp REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_check_modules(PQXX REQUIRED IMPORTED_TARGET libpqxx)


include(FetchContent)
FetchContent_Declare(
  google_benchmark
  GIT_REPOSITORY https://github.com/google/benchmark
  GIT_TAG        v1.9.5
)

set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(google_benchmark)

set(COMMON_SOURCES
    src/app/SignalManager.cpp
    src/mqtt/MqttSubscriber.cpp
    src/mqtt/consumers/PostgreSqlConsumer.cpp
    src/mqtt/consumers/ConsoleOutputConsumer.cpp
)

add_executable(mqtt_subscriber
    src/main.cpp
    src/app/ArgParseConfig.cpp
    ${COMMON_SOURCES}
)

target_link_libraries(mqtt_subscriber PRIVATE PkgConfig::PQXX)
target_link_libraries(mqtt_subscriber PRIVATE PahoMqttCpp::paho-mqttpp3)

target_include_directories(mqtt_subscriber PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/include
    src
)

add_executable(bench
    test/bench.cpp
    ${COMMON_SOURCES}
)
target_link_libraries(bench PRIVATE benchmark::benchmark_main)
target_link_libraries(bench PRIVATE PkgConfig::PQXX)
target_link_libraries(bench PRIVATE PahoMqttCpp::paho-mqttpp3)

target_include_directories(bench PRIVATE
    src
)
