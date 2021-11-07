cmake_minimum_required(VERSION 3.5.1)
project(riemer-doorbell-cpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(Boost_USE_MULTITHREADED ON)

find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Boost COMPONENTS system filesystem REQUIRED)
find_package(CURL)

include_directories(/usr/local/include ${OPENSSL_INCLUDE_DIR} ${Boost_INCLUDE_DIR} ../tgbot-cpp/include)
link_directories(../tgbot-cpp)

if (CURL_FOUND)
    include_directories(${CURL_INCLUDE_DIRS})
    add_definitions(-DHAVE_CURL)
endif()

option(WITH_AZURE_EVENTHUB "Add support for Azure Event Hub" ON)

set(optional_SOURCES)

if(WITH_AZURE_EVENTHUB)
    find_package(ProtonCpp REQUIRED)

    add_definitions(-DWITH_AZURE_EVENTHUB)

    set(optional_SOURCES ${optional_SOURCES} sink-azure.cpp sink-azure.h)
endif()

add_executable(riemer-doorbell
    main.cpp
    gpiopin.h gpiopin.cpp
    knownchats.h knownchats.cpp
    sink.h sink.cpp
    sink-telegram.h sink-telegram.cpp
    ${optional_SOURCES}
)

target_link_libraries(riemer-doorbell
    TgBot
    ${CMAKE_THREAD_LIBS_INIT}
    ${OPENSSL_LIBRARIES}
    ${Boost_LIBRARIES}
    ${CURL_LIBRARIES}
    ${ProtonCpp_LIBRARIES}
)
