import torch
import base64
from io import BytesIO
from PIL import Image
from diffusers import AutoPipelineForInpainting, LCMScheduler
from diffusers.utils import load_image
import sys
import json
import warnings

def process_images(init_image_base64, mask_image_base64, user_prompt="Seamlessly edited and blended image, masterful photoshop job"):
    # Decode the base64 images
    init_image_data = base64.b64decode(init_image_base64)
    mask_image_data = base64.b64decode(mask_image_base64)

    # Convert binary data to PIL images
    init_image = Image.open(BytesIO(init_image_data))
    mask_image = Image.open(BytesIO(mask_image_data))

    # Save images
    init_image.save("init_image.png")
    mask_image.save("mask_image.png")

    # Load the inpainting pipeline
    pipe = AutoPipelineForInpainting.from_pretrained(
        "/Users/gtomberlin/Documents/Code/Local-Image-Editor/resources/models/stable-diffusion-inpainting",
        torch_dtype=torch.float16,
        variant="fp16",
    ).to("mps")

    # Set scheduler
    pipe.scheduler = LCMScheduler.from_config(pipe.scheduler.config)

    # Load LCM-LoRA weights
    pipe.load_lora_weights("latent-consistency/lcm-lora-sdv1-5")

    # Set the prompt and generator
    prompt = user_prompt
    generator = torch.manual_seed(0)

    # Perform inpainting
    result = pipe(
        prompt=prompt,
        image=init_image,
        mask_image=mask_image,
        generator=generator,
        num_inference_steps=4,
        guidance_scale=4,
    ).images[0]

    # Save the result image
    result.save("result.png")

    # Convert the result image to base64 compatible to be read and decoded by C++ QByteArray
    buffer = BytesIO()
    result.save(buffer, format="PNG")
    result_base64 = base64.b64encode(buffer.getvalue()).decode("utf-8")

    return result_base64

def main():
    warnings.filterwarnings("ignore")  # Ignore all warnings

    input_data = sys.stdin.read()
    json_data = json.loads(input_data)
    init_image_base64 = json_data["init_image_base64"]
    mask_image_base64 = json_data["mask_image_base64"]
    user_prompt = json_data.get("user_prompt", "")

    result_base64 = process_images(init_image_base64, mask_image_base64, user_prompt)
    
    with open("inpainting_result.txt", "w") as f:
        f.write(result_base64)

if __name__ == "__main__":
    main()
