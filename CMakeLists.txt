cmake_minimum_required(VERSION 3.10)
project(MediaEditor)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

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

# Ensure compatibility with Qt5 and Qt6
#find_package(Qt6 COMPONENTS Widgets Core Gui OpenGL OpenGLWidgets QUIET)
#if (Qt6_FOUND)
#    message(STATUS "Qt6 found")
#    set(QT_FOUND TRUE)
#    set(QT_VERSION_MAJOR 6)
#    set(QT_LIBRARIES Qt6::Widgets Qt6::Core Qt6::Gui Qt6::OpenGL Qt6::OpenGLWidgets)
#else()
#    find_package(Qt5 COMPONENTS Widgets Core Gui OpenGL REQUIRED)
#    if (Qt5_FOUND)
#        message(STATUS "Qt5 found")
#        set(QT_FOUND TRUE)
#        set(QT_VERSION_MAJOR 5)
#        set(QT_LIBRARIES Qt5::Widgets Qt5::Core Qt5::Gui Qt5::OpenGL)
#    else()
#        message(FATAL_ERROR "Neither Qt5 nor Qt6 could be found.")
#    endif()
#endif()

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
set(VENV_PYTHON "${CMAKE_SOURCE_DIR}/${VENV_NAME}/Scripts/python.exe")

# Install main Python dependencies, with force-reinstall to resolve conflicts
add_custom_target(install_main_python_deps
    COMMAND ${VENV_PYTHON} -m pip install -r ${CMAKE_SOURCE_DIR}/resources/scripts/requirements.txt
)

# Step 1: Always define the clone_edgesam target
add_custom_target(clone_edgesam
    COMMAND ${CMAKE_COMMAND} -E echo "Cloning EdgeSAM repository..."
)

# Only run the git clone command if the repository doesn't exist
if (NOT EXISTS "${CMAKE_SOURCE_DIR}/resources/models/EdgeSAM/.git")
    add_custom_command(TARGET clone_edgesam POST_BUILD
        COMMAND git clone https://github.com/chongzhou96/EdgeSAM.git ${CMAKE_SOURCE_DIR}/resources/models/EdgeSAM
        COMMAND git -C ${CMAKE_SOURCE_DIR}/resources/models/EdgeSAM reset --hard 96c457a
    )
else()
    message(STATUS "EdgeSAM repository already exists. Skipping clone.")
endif()

# Step 2: Define the weights download target
add_custom_target(download_edgesam_weights
    COMMAND ${CMAKE_COMMAND} -E echo "Downloading EdgeSAM weights..."
)

# Only download the weights if they don't already exist
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

# Ensure the weights are downloaded only after the clone_edgesam target has been built
add_dependencies(download_edgesam_weights clone_edgesam)

# Step 3: Install EdgeSAM dependencies if the repo exists
add_custom_target(install_edgesam_deps
    COMMAND ${CMAKE_COMMAND} -E echo "Installing EdgeSAM dependencies..."
    #COMMAND ${VENV_PYTHON} -m pip install --force-reinstall -r ${CMAKE_SOURCE_DIR}/resources/models/EdgeSAM/requirements.txt
    COMMAND ${VENV_PYTHON} -m pip install -e ${CMAKE_SOURCE_DIR}/resources/models/EdgeSAM
    DEPENDS download_edgesam_weights
)

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
add_definitions(${Qt6Widgets_DEFINITIONS})

# Add a custom target to copy resources to the build directory
add_custom_target(copy_resources ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/resources
    ${CMAKE_BINARY_DIR}/resources
)

# Ensure resources are copied before building the executable
add_dependencies(${PROJECT_NAME} copy_resources)

# Add dependencies to the main target AFTER the executable is defined
add_dependencies(${PROJECT_NAME} install_main_python_deps install_edgesam_deps download_edgesam_weights)

# Always define the target, but conditionally run the command based on the model's existence
add_custom_target(save_stable_diffusion_model
    COMMAND ${CMAKE_COMMAND} -E echo "Stable Diffusion model already exists. Skipping download."
    COMMENT "Checking for Stable Diffusion model"
)

if (NOT EXISTS "${CMAKE_SOURCE_DIR}/resources/models/stable-diffusion-inpainting")
    add_custom_command(TARGET save_stable_diffusion_model POST_BUILD
        COMMAND ${VENV_PYTHON} -c "import torch; from diffusers import AutoPipelineForInpainting; model = AutoPipelineForInpainting.from_pretrained('runwayml/stable-diffusion-inpainting', torch_dtype=torch.float16, variant='fp16'); model.save_pretrained('${CMAKE_SOURCE_DIR}/resources/models/stable-diffusion-inpainting')"
    )
endif()

# Add dependencies to the main target for stable diffusion model
add_dependencies(${PROJECT_NAME} save_stable_diffusion_model)