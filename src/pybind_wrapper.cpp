#include "pybind_wrapper.h"
#include <pybind11/embed.h>
#include <iostream>
namespace py = pybind11;

// Define the embedded Python script
// const char* image_processor_script = R"(
// import base64
// from PIL import Image
// from io import BytesIO

// def process_images(image1_base64, image2_base64):
//     image1_data = base64.b64decode(image1_base64)
//     image2_data = base64.b64decode(image2_base64)
//     image1 = Image.open(BytesIO(image1_data))
//     image2 = Image.open(BytesIO(image2_data))
//     result_image = Image.blend(image1, image2, alpha=0.5)
//     buffered = BytesIO()
//     result_image.save(buffered, format="PNG")
//     result_image_base64 = base64.b64encode(buffered.getvalue()).decode("utf-8")
//     return result_image_base64
// )";

const char* image_processor_script = R"(
import torch
import base64
from io import BytesIO
from PIL import Image
from diffusers import AutoPipelineForInpainting, LCMScheduler
from diffusers.utils import load_image

def process_images(init_image_base64, mask_image_base64, user_prompt="Seamlessly edited and blended image, masterful photoshop job"):
    # Decode the base64 images
    init_image_data = base64.b64decode(init_image_base64)
    mask_image_data = base64.b64decode(mask_image_base64)

    # Convert binary data to PIL images
    init_image = Image.open(BytesIO(init_image_data))
    mask_image = Image.open(BytesIO(mask_image_data))

    # Load the inpainting pipeline
    pipe = AutoPipelineForInpainting.from_pretrained(
        "/Users/gtomberlin/Documents/Code/Local-Image-Editor/resources/models/stable-diffusion-inpainting",
        torch_dtype=torch.float16,
        variant="fp16",
    ).to("mps")

    print("Model loaded")

    # Set scheduler
    pipe.scheduler = LCMScheduler.from_config(pipe.scheduler.config)

    # Load LCM-LoRA weights
    pipe.load_lora_weights("latent-consistency/lcm-lora-sdv1-5")

    print("Weights loaded")

    # Set the prompt and generator
    prompt = user_prompt
    generator = torch.manual_seed(0)

    print("Performing inpainting")

    # Perform inpainting
    result = pipe(
        prompt=prompt,
        image=init_image,
        mask_image=mask_image,
        generator=generator,
        num_inference_steps=4,
        guidance_scale=4,
    ).images[0]

    print("Inpainting complete")

    # Convert the result image to base64
    buffer = BytesIO()
    result.save(buffer, format="PNG")
    result_base64 = base64.b64encode(buffer.getvalue()).decode("utf-8")

    return result_base64
)";

// Function to process images using the embedded Python script
std::string process_images(const std::string& image1_base64, const std::string& image2_base64, const std::string& user_prompt) {
    try {
        py::scoped_interpreter guard{}; // Start the Python interpreter
        py::exec(image_processor_script); // Execute the embedded script
        
        // Access the main module and the process_images function
        py::module_ image_processor = py::module_::import("__main__");
        py::object process_images_func = image_processor.attr("process_images");
        
        // Call the process_images function with base64 encoded images
        py::object result;
        if (user_prompt.empty() || user_prompt == "") {
            result = process_images_func(image1_base64, image2_base64);
        } else {
            result = process_images_func(image1_base64, image2_base64, user_prompt);
        }

        if (result.is_none()) {
            throw std::runtime_error("Error processing images");
        }
        
        // Cast the result to a std::string and return
        return result.cast<std::string>();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return "";
    }
}
