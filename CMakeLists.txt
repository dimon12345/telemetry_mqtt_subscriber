cmake_minimum_required(VERSION 3.14)

project(mqtt_subscriber CXX)
set(CMAKE_CXX_STANDARD 20)

find_package(PahoMqttCpp REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_check_modules(PQXX REQUIRED IMPORTED_TARGET libpqxx)

add_executable(mqtt_subscriber
    src/main.cpp
    src/app/Config.cpp
    src/app/CtrlCHandler.cpp
    src/mqtt/ConsoleOutputConsumer.cpp
    src/mqtt/MqttSubscriber.cpp
    src/mqtt/PostgreSqlConsumer.cpp
)

target_link_libraries(mqtt_subscriber PRIVATE PkgConfig::PQXX)
target_link_libraries(mqtt_subscriber PRIVATE PahoMqttCpp::paho-mqttpp3)

target_include_directories(mqtt_subscriber PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/include
    src
)
