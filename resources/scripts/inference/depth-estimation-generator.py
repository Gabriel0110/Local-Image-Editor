import torch
import base64
from io import BytesIO
from PIL import Image
from transformers import pipeline
import sys
import json
import logging
import os
import numpy as np
import matplotlib.pyplot as plt

# Set up logging
filename = os.path.join(os.path.dirname(__file__), "depth_estimation.log")
logging.basicConfig(filename=filename, level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')

def process_image(image_base64):
    try:
        # Decode the base64 image
        image_data = base64.b64decode(image_base64)

        # Convert binary data to PIL image
        image = Image.open(BytesIO(image_data))

        # Load the depth estimation pipeline
        print("Loading depth estimation pipeline...")
        model_path = os.path.join(os.path.dirname(__file__), '../../models/depth-anything/Depth-Anything-V2-Small-hf')
        device = "cuda" if torch.cuda.is_available() else "mps" if torch.backends.mps.is_available() else "cpu"
        pipe = pipeline(task="depth-estimation", model=model_path, device=device)
        print("Depth estimation pipeline loaded successfully!")

        # Perform depth estimation
        print("Performing depth estimation...")
        depth = pipe(image)["depth"]
        print("Depth estimation completed!")

        # Convert depth to numpy array
        depth_np = np.array(depth)

        # Normalize depth values
        depth_normalized = (depth_np - depth_np.min()) / (depth_np.max() - depth_np.min()) * 255.0
        depth_normalized = depth_normalized.astype(np.uint8)

        # Create colored depth map
        cmap = plt.get_cmap('Spectral_r')
        colored_depth = (cmap(depth_normalized)[:, :, :3] * 255).astype(np.uint8)

        # Convert colored depth to PIL Image
        colored_depth_image = Image.fromarray(colored_depth)

        # Save the colored depth image locally (DEBUGGING)
        colored_depth_image.save(os.path.join(os.path.dirname(__file__), "depth_estimation_result.png"))
        print(f"Colored depth estimation result saved locally: {os.path.join(os.path.dirname(__file__), 'depth_estimation_result.png')}")

        # Convert the result image to base64 compatible to be read and decoded by C++ QByteArray
        buffer = BytesIO()
        colored_depth_image.save(buffer, format="PNG")
        result_base64 = base64.b64encode(buffer.getvalue()).decode("utf-8")

        return result_base64
    except Exception as e:
        logging.error(f"Error processing image: {str(e)}")
        raise

def main():
    try:
        input_data = sys.stdin.read()
        json_data = json.loads(input_data)
        image_base64 = json_data["image_base64"]

        result_base64 = process_image(image_base64)
        
        with open(os.path.join(os.path.dirname(__file__), "depth_estimation_result.txt"), "w") as f:
            f.write(result_base64)
    except Exception as e:
        logging.error(f"Error in main function: {str(e)}")
        raise

if __name__ == "__main__":
    main()