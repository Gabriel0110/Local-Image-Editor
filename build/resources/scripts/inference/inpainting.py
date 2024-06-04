import torch
import base64
from io import BytesIO
from PIL import Image
from diffusers import AutoPipelineForInpainting, LCMScheduler
from diffusers.utils import load_image
import sys
import json
import logging

# Set up logging
filename = "inpainting.log"
logging.basicConfig(filename=filename, level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')

def process_images(init_image_base64, mask_image_base64, user_prompt="Seamlessly edited and blended image, masterful photoshop job"):
    try:
        # Decode the base64 images
        init_image_data = base64.b64decode(init_image_base64)
        mask_image_data = base64.b64decode(mask_image_base64)

        # Convert binary data to PIL images
        init_image = Image.open(BytesIO(init_image_data))
        mask_image = Image.open(BytesIO(mask_image_data))

        # Convert to RGB if not already
        if init_image.mode != "RGB":
            init_image = init_image.convert("RGB")
        if mask_image.mode != "RGB":
            mask_image = mask_image.convert("RGB")

        # Load the inpainting pipeline
        pipe = AutoPipelineForInpainting.from_pretrained(
            "/Users/gtomberlin/Documents/Code/Local-Image-Editor/resources/models/stable-diffusion-inpainting",
            torch_dtype=torch.float32,
            variant="fp32",
        ).to("mps")

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
        
        with open("inpainting_result.txt", "w") as f:
            f.write(result_base64)
    except Exception as e:
        logging.error(f"Error in main function: {str(e)}")
        raise

if __name__ == "__main__":
    main()
