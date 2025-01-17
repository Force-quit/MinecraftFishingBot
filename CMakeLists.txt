cmake_minimum_required(VERSION 3.26)
project(MinecraftFishingBot LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ENV{PATH} "C:/Qt/6.8.1/msvc2022_64/bin;$ENV{PATH}")
set(CMAKE_PREFIX_PATH "C:/Qt/6.8.1/msvc2022_64")
add_compile_options(/EHsc)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
qt_standard_project_setup()

include(FetchContent)
FetchContent_Declare(
    EUtilities
    URL https://github.com/Force-quit/EUtilities/archive/refs/tags/v1.0.1.zip
)
FetchContent_MakeAvailable(EUtilities)
FetchContent_Declare(
    EQUtilities
    URL https://github.com/Force-quit/EQUtilities/archive/refs/tags/v1.1.1.zip
)
FetchContent_MakeAvailable(EQUtilities)

set(SOURCE
    src/EQMinecraftFishingBot.h
    src/EQMinecraftFishingBot.cpp
    src/EQMinecraftFishingBotWorker.h
    src/EQMinecraftFishingBotWorker.cpp
    src/main.cpp
)

qt_add_executable(MinecraftFishingBot ${SOURCE})
target_link_libraries(MinecraftFishingBot PRIVATE Qt6::Widgets EUtilities EQUtilities)
set_target_properties(MinecraftFishingBot PROPERTIES
    WIN32_EXECUTABLE ON
)

add_subdirectory(resources)