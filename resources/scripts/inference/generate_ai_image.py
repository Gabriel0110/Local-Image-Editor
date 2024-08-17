from openai import OpenAI
import sys
import json
import base64
from io import BytesIO
from PIL import Image
import logging
import os

# Set up logging
filename = os.path.join(os.path.dirname(__file__), "generate_ai_image.log")
logging.basicConfig(filename=filename, level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')

def generate_image(api_key, prompt):
    try:
        client = OpenAI(api_key=api_key)
    except Exception as e:
        logging.error(f"Error setting API key: {e}")
        raise ValueError(f"Error setting API key: {e}")
    
    try:
        response = client.images.generate(
            model="dall-e-3",
            prompt=prompt,
            n=1,
            size="1024x1024",
            response_format="b64_json"
        )
        image_obj = response.data[0]        
        image_data = image_obj.b64_json
        
    except Exception as e:
        logging.error(f"Error generating image: {e}")
        raise ValueError(f"Error generating image: {e}")
    
    return image_data

def main():
    input_data = sys.stdin.read()
    json_data = json.loads(input_data)
    api_key = json_data.get("api_key")
    prompt = json_data.get("prompt")

    if not api_key or not prompt:
        print("Error: API key and prompt are required.")
        logging.error("API key and prompt are required.")
        raise ValueError("API key and prompt are required.")

    image_base64 = generate_image(api_key, prompt)
    
    with open(os.path.join(os.path.dirname(__file__), "generated_ai_image.txt"), "w") as f:
        f.write(image_base64)

if __name__ == "__main__":
    main()
