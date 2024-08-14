import sys
import json
import base64
import logging
from io import BytesIO
from rembg import remove, new_session
from PIL import Image
import os

# Set up logging
log_file_path = os.path.join(os.path.dirname(__file__), "oneshot_background_removal.log")
logging.basicConfig(filename=log_file_path, level=logging.DEBUG, format='%(asctime)s - %(levelname)s - %(message)s')

if __name__ == "__main__":
    try:
        logging.info("Starting oneshot background removal process.")

        # Get the output directory from the command-line argument
        if len(sys.argv) > 1:
            output_dir = sys.argv[1]
            logging.info(f"Using output directory: {output_dir}")
        else:
            output_dir = '.'
            logging.info("No output directory provided, using current directory.")

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

        # Ensure the output directory exists
        if not os.path.exists(output_dir):
            os.makedirs(output_dir)
            logging.info(f"Created output directory: {output_dir}")

        # Save the result to the specified output directory
        output_path = os.path.join(output_dir, "oneshot_removal_result.txt")
        with open(output_path, "w") as f:
            f.write(output_base64)
        logging.info(f"Saved output to {output_path}")
    except Exception as e:
        logging.error(f"Error in main function: {str(e)}")
        raise
