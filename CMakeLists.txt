cmake_minimum_required(VERSION 4.1.1)
project(MinecraftFishingBot LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
qt_standard_project_setup()

include(FetchContent)
FetchContent_Declare(
    EQUtilities
    GIT_REPOSITORY https://github.com/Force-quit/EQUtilities.git
    GIT_TAG        main
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
target_link_libraries(MinecraftFishingBot PRIVATE Qt6::Widgets EQUtilities)

if(MSVC)
    add_compile_options(/EHsc)
    set_target_properties(MinecraftFishingBot PROPERTIES WIN32_EXECUTABLE ON)
endif()

add_subdirectory(resources)