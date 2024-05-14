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

# Add executable
add_executable(${PROJECT_NAME} src/main.cpp src/MyOpenGLWidget.cpp src/ImageToolbar.cpp)

# Use the Widgets module from Qt 5
target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::Core Qt5::Gui Qt5::OpenGL ${OpenCV_LIBS} ${OPENGL_LIBRARIES})

# Define additional compile flags
add_definitions(${Qt5Widgets_DEFINITIONS})

