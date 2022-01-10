cmake_minimum_required (VERSION 3.8)

project ("GLEngine")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /fp:fast")
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")
set(CMAKE_CXX_STANDARD 17)

find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

find_path(STB_INCLUDE_DIRS "stb.h")

file(GLOB_RECURSE SRC_FILES
    ${PROJECT_SOURCE_DIR}/src/source/*.cpp

)

add_executable (GLEngine ${SRC_FILES})

target_link_libraries(GLEngine PRIVATE glfw glad::glad glm::glm ${STB_INCLUDE_DIRS} assimp::assimp nlohmann_json nlohmann_json::nlohmann_json imgui::imgui)