# Local Image Editor

Local Image Editor is a desktop application that provides basic and advanced image editing capabilities. This project is based on Yacine's Dingboard (dingboard.com), just not as good, but local! I like to call it Fingboard (Fake Dingboard).

## Features

- **Image Editing**: Crop, rotate, and adjust images with ease.
- **Inpainting**: Fill in missing parts of images or add new details using AI models.
- **Depth-Wise & One-Shot Background Removal**: Remove backgrounds based on depth information or simple one-shot removal.
- **Image Segmentation**: AI object segmentation with positive and negative point prompting.

## TO-DO
- **GIF Editor (IN-PROGRESS: ~50%)**: Add text, images, and layers to GIFs.
- ~~** Add AI Image Generator**~~ (DONE - OpenAI Dall-E 3 API, requires API key)
- **Replace EdgeSAM with SAM2** (possibly, depending on performance improvement)
- **Implement SkalskiP's Florence2 + SAM2 open-vocabulary and captioned image segmentation**
- **Replace Stable Diffusion 2 Inpainting with FLUX Inpainting** (possibly, TBD - FLUX is computationally expensive)
- **Implement an AI image enhancer** (possibly https://huggingface.co/spaces/finegrain/finegrain-image-enhancer)
- **Add dynamic support/option for CUDA-enabled Torch installation**

## Project Structure

```plaintext
Local-Image-Editor/
├── resources/
│   ├── images/
│   ├── models/
│   │   ├── EdgeSAM/ **(BEWARE: THIS IS DOWNLOADED AND INSTALLED DURING THE BUILD)**
│   │   └── stable-diffusion-inpainting/ **(BEWARE: THIS IS DOWNLOADED DURING THE BUILD)**
│   └── scripts/
│       ├── requirements.txt
│       └── inference/
│           ├── generate_ai_image.py
│           ├── oneshot_background_removal.py
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
- **Python (version 3.8, 3.9, 3.10, or 3.11)**
- **Git**
- **A C++ compiler (like GCC or Clang)**
- **Qt5/Qt6 (Qt5 was used in original development), OpenCV, and OpenGL**

### Build Instructions
Follow these steps to build and run the project locally.

1. Clone the repository:

```bash
git clone https://github.com/your-username/Local-Image-Editor.git
cd Local-Image-Editor
```

2. Configure and build the project:

#### On Linux/MacOS
```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

#### On Windows
If you are using Visual Studio 2022 (or any other version), you need to specify the generator and the architecture:
```bash
mkdir build
cd build
cmake -G "Visual Studio 17 2022" -A x64 ..
cmake --build . --config Release
```
- **Note: Replace "Visual Studio 17 2022" with the appropriate version, unless you are using a different generator (e.g. Ninja, etc.) then replace that entire piece with the generator of your choice. Also be sure to build for your system's architecture (x32/x64).**

3. Set up the virtual environment:

The build process will automatically create a Python virtual environment named local-image-editor-venv. It will install all necessary Python dependencies.

4. Clone and set up EdgeSAM:

The build process will also clone the EdgeSAM project, install its dependencies, and set it up for use. The EdgeSAM repository and its models will be stored in the resources/models/EdgeSAM/ directory.

5. Download Stable Diffusion Inpainting model:

The build process will download the Stable Diffusion inpainting model and store it in the resources/models/stable-diffusion-inpainting/ directory.

6. Run the application

Once the build is complete, you can run the application:

#### On Linux/MacOS
```bash
./MediaEditor
```

#### On Windows
```bash
MediaEditor.exe
```

## Advanced Configuration
If you need to customize the build process (e.g., specify a different Python version or additional flags), you can modify the CMakeLists.txt file as needed.

### Installing Dependencies Manually
If you need to install the Python dependencies manually, activate the virtual environment and install the requirements:

```bash
# Activate the virtual environment
source build/local-image-editor-venv/bin/activate  # On Linux/MacOS
# OR
build\local-image-editor-venv\Scripts\activate  # On Windows

# Install the main requirements
pip install -r resources/scripts/requirements.txt

# Install EdgeSAM requirements
pip install -r build/EdgeSAM/requirements.txt
```

## Bugs/Issues/Oddities
Below are known bugs/issues/oddities that are in the pipeline to eventually be fixed or improved.
- If you snipe on an image that already has transparency to it, the image with a hole replaces the transparency with black
- Same as above, if you inpaint on an image that already has a transparency (test via sniping then do inpaint), it will replace the transparency with black.
- **INPAINTING BUGGED**: Inpainting is returning an image with insanely worse quality and blemishes and doesn't seem to be working correctly. Maybe user error, heh.
- When rotating, the image slowly moves upward to the left
- When multi-selecting via clicking (ctrl/cmd clicking), if they images are layered on top of each other, there is some incorrect interactions - they do not get properly selected as they should and like they do with multi-select via selection box
- If you have images stacked, and you push one to front/back then click a different stacked image, it will multi-select them
- ~~When merging, it merges based on the order in which the images are selected in multi-select, but it should be based on the layer order of the images (multi-select via box selection works fine)~~
- Any tool that has additional UI that appears: if the image is too big, or the location of where the additional UI appears is off the screen, you can't see it or use it, and you have to cancel and then move the image to where you'll be able to see it, then re-do it.

## Contributing
Contributions are welcome! Please fork this repository and submit a pull request.

## Contact
If you have any questions or feedback, feel free to reach out via GitHub issues or contact me directly. You can also open an issue in the repository for bug reports or feature requests.
