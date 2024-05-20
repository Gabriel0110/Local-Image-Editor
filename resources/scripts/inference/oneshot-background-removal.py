import sys
import json
import base64
import logging
from io import BytesIO
from rembg import remove, new_session
from PIL import Image

# Set up logging
logging.basicConfig(filename="oneshot_background_removal.log", level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')

if __name__ == "__main__":
    try:
        logging.info("Starting oneshot background removal process.")

        model_name = "u2netp"
        rembg_session = new_session(model_name)

        input_data = sys.stdin.read()
        json_data = json.loads(input_data)
        logging.info("Loaded input data.")

        input_img = Image.open(BytesIO(base64.b64decode(json_data["original_image"])))
        logging.info("Loaded input image.")

        output_img = remove(input_img, session=rembg_session, post_process_mask=True)
        logging.info("Performed background removal.")

        buffer = BytesIO()
        output_img.save(buffer, format="PNG")
        output_base64 = base64.b64encode(buffer.getvalue()).decode("utf-8")
        logging.info("Converted output image to base64.")

        with open("oneshot_removal_result.txt", "w") as f:
            f.write(output_base64)
    except Exception as e:
        logging.error(f"Error in main function: {str(e)}")
        raise