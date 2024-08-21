cmake_minimum_required(VERSION 3.10)
project(MediaEditor)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Debug)

if (MSVC)
    add_compile_options(/Zc:__cplusplus /permissive-)
endif()

find_package(Qt5 COMPONENTS Widgets Core Gui OpenGL QUIET)
if (Qt5_FOUND)
    message(STATUS "Qt5 found")
    set(QT_FOUND TRUE)
    set(QT_VERSION_MAJOR 5)
    set(QT_LIBRARIES Qt5::Widgets Qt5::Core Qt5::Gui Qt5::OpenGL)
else()
    find_package(Qt6 COMPONENTS Widgets Core Gui OpenGL OpenGLWidgets REQUIRED)
    if (Qt6_FOUND)
        message(STATUS "Qt6 found")
        set(QT_FOUND TRUE)
        set(QT_VERSION_MAJOR 6)
        set(QT_LIBRARIES Qt6::Widgets Qt6::Core Qt6::Gui Qt6::OpenGL Qt6::OpenGLWidgets)
    else()
        message(FATAL_ERROR "Neither Qt5 nor Qt6 could be found.")
    endif()
endif()

# Find OpenGL
find_package(OpenGL REQUIRED)

# Find Python
find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
if (NOT Python3_FOUND)
    message(FATAL_ERROR "Python3 interpreter not found. Please install Python3 and try again.")
endif()

# Set up a virtual environment for Python dependencies
set(VENV_NAME "local-image-editor-venv")

if(NOT EXISTS "${CMAKE_SOURCE_DIR}/${VENV_NAME}")
    execute_process(COMMAND ${Python3_EXECUTABLE} -m venv ${CMAKE_SOURCE_DIR}/${VENV_NAME})
endif()

# Determine the correct path for the Python executable based on the platform
if (WIN32)
    set(VENV_PYTHON "${CMAKE_SOURCE_DIR}/${VENV_NAME}/Scripts/python.exe")
else()
    set(VENV_PYTHON "${CMAKE_SOURCE_DIR}/${VENV_NAME}/bin/python")
endif()

# Install main Python dependencies
add_custom_target(install_main_python_deps
    COMMAND ${VENV_PYTHON} -m pip install --upgrade pip
    COMMAND ${VENV_PYTHON} -m pip install -r ${CMAKE_SOURCE_DIR}/resources/scripts/requirements.txt
)

# Clone EdgeSAM only if it doesn't exist
add_custom_target(clone_edgesam
    COMMAND ${CMAKE_COMMAND} -E echo "Cloning EdgeSAM repository..."
)

if (NOT EXISTS "${CMAKE_SOURCE_DIR}/resources/models/EdgeSAM")
    add_custom_command(TARGET clone_edgesam POST_BUILD
        COMMAND git clone https://github.com/chongzhou96/EdgeSAM.git ${CMAKE_SOURCE_DIR}/resources/models/EdgeSAM
        COMMAND git -C ${CMAKE_SOURCE_DIR}/resources/models/EdgeSAM reset --hard 96c457a
    )
else()
    message(STATUS "EdgeSAM repository already exists. Skipping clone.")
endif()

# Download EdgeSAM weights only if they don't exist
add_custom_target(download_edgesam_weights
    COMMAND ${CMAKE_COMMAND} -E echo "Downloading EdgeSAM weights..."
)

if (NOT EXISTS "${CMAKE_SOURCE_DIR}/resources/models/EdgeSAM/weights/edge_sam.pth" OR NOT EXISTS "${CMAKE_SOURCE_DIR}/resources/models/EdgeSAM/weights/edge_sam_3x.pth")
    if (WIN32)
        add_custom_command(TARGET download_edgesam_weights POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_SOURCE_DIR}/resources/models/EdgeSAM/weights"
            COMMAND curl -L -o "${CMAKE_SOURCE_DIR}/resources/models/EdgeSAM/weights/edge_sam.pth" https://huggingface.co/spaces/chongzhou/EdgeSAM/resolve/main/weights/edge_sam.pth
            COMMAND curl -L -o "${CMAKE_SOURCE_DIR}/resources/models/EdgeSAM/weights/edge_sam_3x.pth" https://huggingface.co/spaces/chongzhou/EdgeSAM/resolve/main/weights/edge_sam_3x.pth
        )
    else()
        add_custom_command(TARGET download_edgesam_weights POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_SOURCE_DIR}/resources/models/EdgeSAM/weights"
            COMMAND wget -P "${CMAKE_SOURCE_DIR}/resources/models/EdgeSAM/weights/" https://huggingface.co/spaces/chongzhou/EdgeSAM/resolve/main/weights/edge_sam.pth
            COMMAND wget -P "${CMAKE_SOURCE_DIR}/resources/models/EdgeSAM/weights/" https://huggingface.co/spaces/chongzhou/EdgeSAM/resolve/main/weights/edge_sam_3x.pth
        )
    endif()
endif()

add_dependencies(download_edgesam_weights clone_edgesam)

# Install EdgeSAM dependencies only if they aren't installed
add_custom_target(install_edgesam_deps
    COMMAND ${CMAKE_COMMAND} -E echo "Installing EdgeSAM dependencies..."
)

if (NOT EXISTS "${CMAKE_SOURCE_DIR}/resources/models/EdgeSAM/edge_sam.egg-info")
    add_custom_command(TARGET install_edgesam_deps POST_BUILD
        COMMAND ${VENV_PYTHON} -m pip install -e ${CMAKE_SOURCE_DIR}/resources/models/EdgeSAM
        DEPENDS download_edgesam_weights
    )
else()
    message(STATUS "EdgeSAM dependencies already installed. Skipping installation.")
endif()

# Save Stable Diffusion model if it doesn't exist
add_custom_target(save_stable_diffusion_model
    COMMENT "Checking for Stable Diffusion model"
)

if (NOT EXISTS "${CMAKE_SOURCE_DIR}/resources/models/stable-diffusion-2-inpainting")
    add_custom_command(TARGET save_stable_diffusion_model POST_BUILD
        COMMAND ${VENV_PYTHON} ${CMAKE_SOURCE_DIR}/resources/scripts/download_sd_model.py ${VENV_PYTHON} ${CMAKE_SOURCE_DIR}/resources/models/stable-diffusion-2-inpainting
        DEPENDS install_main_python_deps install_edgesam_deps
    )
else()
    message(STATUS "Stable Diffusion model already exists. Skipping download.")
endif()

# Add executable
add_executable(${PROJECT_NAME} 
    src/main.cpp 
    src/MyOpenGLWidget.cpp 
    src/ImageToolbar.cpp 
    src/MainWindow.cpp 
    src/CustomConfirmationDialog.cpp
)

# Link libraries
target_link_libraries(${PROJECT_NAME} 
    ${QT_LIBRARIES} 
    ${OpenCV_LIBS} 
    ${OPENGL_LIBRARIES} 
    ${Python3_LIBRARIES}
)

# Conditionally link OpenGL32.lib for Windows
if (WIN32)
    target_link_libraries(${PROJECT_NAME} OpenGL32.lib)
endif()

# Define additional compile flags
if (Qt6_FOUND)
    add_definitions(${Qt6Widgets_DEFINITIONS})
endif()

# Add a custom target to copy resources to the build directory
add_custom_target(copy_resources ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/resources
    ${CMAKE_BINARY_DIR}/resources
)

# Ensure resources are copied before building the executable
add_dependencies(${PROJECT_NAME} copy_resources)

# Add dependencies to the main target AFTER the executable is defined
add_dependencies(${PROJECT_NAME} install_main_python_deps install_edgesam_deps save_stable_diffusion_model)
