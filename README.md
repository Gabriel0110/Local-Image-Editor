# Local Image Editor

Local Image Editor is a powerful desktop application that provides advanced image editing capabilities (if desired - need the models locally). This project is based on Yacine's Dingboard (dingboard.com) - I like to call it Fingboard (Fake Dingboard).

## Features

- **Image Editing**: Crop, rotate, and adjust images with ease.
- **Inpainting**: Fill in missing parts of images using AI models.
- **Depth-Wise Background Removal**: Remove backgrounds based on depth information.
- **GIF Editing (IN-PROGRESS)**: Add text, images, and layers to GIFs.
- **AI Models Integration**: EdgeSAM for advanced segmentation and Stable Diffusion for inpainting.

## Project Structure

```plaintext
Local-Image-Editor/
├── build/
├── resources/
│   ├── images/
│   ├── models/
│   │   ├── EdgeSAM/
│   │   └── stable-diffusion-inpainting/
│   └── scripts/
│       ├── requirements.txt
│       └── inference/
│           ├── inpainting.py
│           └── sam.py
├── src/
│   ├── CustomConfirmationDialog.h
│   ├── CustomConfirmationDialog.cpp
│   ├── ImageObject.h
│   ├── ImageObject.cpp
│   ├── ImageToolbar.h
│   ├── ImageToolbar.cpp
│   ├── main.cpp
│   ├── MainWindow.cpp
│   ├── MainWindow.h
│   ├── MyOpenGLWidget.cpp
│   └── MyOpenGLWidget.h
└── CMakeLists.txt
```

## Getting Started
### Prerequisites
Before you begin, ensure you have the following installed on your system:
- **CMake (version 3.10 or higher)**
- **Python (version 3.8 or higher)**
- **Git**
- **A C++ compiler (like GCC or Clang)**
- **Qt5, OpenCV, and OpenGL**

### Build Instructions
Follow these steps to build and run the project locally.

1. Clone the repository:

```bash
git clone https://github.com/your-username/Local-Image-Editor.git
cd Local-Image-Editor
```

2. Configure and build the project:

```bash
mkdir build
cd build
cmake ..
make
```

3. Set up the virtual environment:

The build process will automatically create a Python virtual environment named local-image-editor-venv. It will install all necessary Python dependencies.

4. Clone and set up EdgeSAM:

The build process will also clone the EdgeSAM project, install its dependencies, and set it up for use. The EdgeSAM repository and its models will be stored in the resources/models/EdgeSAM/ directory.

5. Download Stable Diffusion Inpainting model:

The build process will download the Stable Diffusion inpainting model and store it in the resources/models/stable-diffusion-inpainting/ directory.

6. Run the application

Once the build is complete, you can run the application:

```bash
./MediaEditor
```

## Advanced Configuration
If you need to customize the build process (e.g., specify a different Python version or additional flags), you can modify the CMakeLists.txt file as needed.

### Installing Dependencies Manually
If you need to install the Python dependencies manually, activate the virtual environment and install the requirements:

```bash
source build/local-image-editor-venv/bin/activate
pip install -r resources/scripts/requirements.txt
pip install -r build/EdgeSAM/requirements.txt
```

## Contributing
Contributions are welcome! Please fork this repository and submit a pull request.

## Acknowledgements
- EdgeSAM
- Stable Diffusion
- OpenCV
- Qt

## Contact
If you have any questions or feedback, feel free to reach out via GitHub issues or contact me directly. You can also open an issue in the repository for bug reports or feature requests.
