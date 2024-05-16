cmake_minimum_required(VERSION 3.10)
project(MediaEditor)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find packages
set(QT5_DIR "/opt/homebrew/opt/qt@5")
set(OPENCV_DIR "/opt/homebrew/Cellar/opencv/4.9.0_8")
set(CMAKE_PREFIX_PATH ${QT5_DIR} ${OPENCV_DIR})
find_package(Qt5 COMPONENTS Widgets Core Gui OpenGL REQUIRED)
find_package(OpenCV REQUIRED)
find_package(OpenGL REQUIRED)  # Find the OpenGL package

# Include directories
include_directories(${Qt5Widgets_INCLUDES} ${OpenCV_INCLUDE_DIRS})

# Specify the directories for the resources
set(RESOURCES_DIR "${CMAKE_SOURCE_DIR}/resources")

# Add executable
add_executable(${PROJECT_NAME} src/main.cpp src/MyOpenGLWidget.cpp src/ImageToolbar.cpp src/MainWindow.cpp)

# Link libraries (has to come after adding the executable)
target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::Core Qt5::Gui Qt5::OpenGL ${OpenCV_LIBS} ${OPENGL_LIBRARIES})

# Define additional compile flags
add_definitions(${Qt5Widgets_DEFINITIONS})

# Add a custom target to copy resources to the build directory
add_custom_target(copy_resources ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${RESOURCES_DIR}
    ${CMAKE_BINARY_DIR}/resources
)

# Ensure resources are copied before building the executable
add_dependencies(${PROJECT_NAME} copy_resources)