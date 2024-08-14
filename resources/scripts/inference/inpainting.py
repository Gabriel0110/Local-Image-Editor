import torch
import base64
from io import BytesIO
from PIL import Image
from diffusers import AutoPipelineForInpainting, LCMScheduler
import sys
import json
import logging
import os

# Set up logging
filename = os.path.join(os.path.dirname(__file__), "inpainting.log")
logging.basicConfig(filename=filename, level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')

def process_images(init_image_base64, mask_image_base64, user_prompt="Seamlessly edited and blended image, masterful photoshop job"):
    try:
        # Decode the base64 images
        init_image_data = base64.b64decode(init_image_base64)
        mask_image_data = base64.b64decode(mask_image_base64)

        # Convert binary data to PIL images
        init_image = Image.open(BytesIO(init_image_data))
        mask_image = Image.open(BytesIO(mask_image_data))

        # Convert to RGB if there's an alpha channel
        if init_image.mode == "RGBA":
            init_image = init_image.convert("RGB")
        if mask_image.mode == "RGBA":
            mask_image = mask_image.convert("RGB")

        # Load the inpainting pipeline
        model_path = os.path.join(os.path.dirname(__file__), '../../models/stable-diffusion-inpainting')
        
        device = "cuda" if torch.cuda.is_available() else "mps" if torch.backends.mps.is_available() else "cpu"
        pipe = AutoPipelineForInpainting.from_pretrained(
            model_path,
            torch_dtype=torch.float32,
            variant="fp32",
            safety_checker=None,
        ).to(device)

        # Set scheduler
        pipe.scheduler = LCMScheduler.from_config(pipe.scheduler.config)

        # Load LCM-LoRA weights
        pipe.load_lora_weights("latent-consistency/lcm-lora-sdv1-5")

        # Set the prompt and generator
        prompt = user_prompt
        generator = torch.manual_seed(0)

        # Ensure tensor sizes match for the model's expected input
        if init_image.size != mask_image.size:
            logging.error("Initial and mask images sizes do not match.")
            raise ValueError("Initial and mask images sizes do not match.")

        # Perform inpainting
        result = pipe(
            prompt=prompt,
            image=init_image,
            mask_image=mask_image,
            generator=generator,
            num_inference_steps=4,
            guidance_scale=4,
        ).images[0]

        # Save the image locally
        result.save(os.path.join(os.path.dirname(__file__), "inpainting_result.png"))

        # Convert the result image to base64 compatible to be read and decoded by C++ QByteArray
        buffer = BytesIO()
        result.save(buffer, format="PNG")
        result_base64 = base64.b64encode(buffer.getvalue()).decode("utf-8")

        return result_base64
    except Exception as e:
        logging.error(f"Error processing images: {str(e)}")
        raise

def main():
    try:
        input_data = sys.stdin.read()
        json_data = json.loads(input_data)
        init_image_base64 = json_data["init_image_base64"]
        mask_image_base64 = json_data["mask_image_base64"]
        user_prompt = json_data.get("user_prompt", "")

        result_base64 = process_images(init_image_base64, mask_image_base64, user_prompt)
        
        with open(os.path.join(os.path.dirname(__file__), "inpainting_result.txt"), "w") as f:
            f.write(result_base64)
    except Exception as e:
        logging.error(f"Error in main function: {str(e)}")
        raise

if __name__ == "__main__":
    main()
