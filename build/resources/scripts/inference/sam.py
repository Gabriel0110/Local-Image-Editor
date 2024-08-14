from edge_sam import SamPredictor, sam_model_registry
import numpy as np
from PIL import Image
import torch
import matplotlib.pyplot as plt
import sys
import json
import base64
from io import BytesIO
import logging
import os

# Set up logging
log_file = os.path.join(os.path.dirname(__file__), "snipe.log")
logging.basicConfig(
    filename=log_file,
    level=logging.INFO,
    format="%(asctime)s - %(levelname)s - %(message)s"
)

def show_mask(mask, ax, random_color=False):
    if random_color:
        color = np.concatenate([np.random.random(3), np.array([0.6])], axis=0)
    else:
        color = np.array([30/255, 144/255, 255/255, 0.6])
    h, w = mask.shape[-2:]
    mask_image = mask.reshape(h, w, 1) * color.reshape(1, 1, -1)
    ax.imshow(mask_image)

def get_images(original_image, mask_image, pos_points, neg_points):
    try:
        mask = np.array(mask_image)
        mask = (mask > 0.5).astype(np.uint8)
        logging.info("Converted mask image to numpy array.")
        
        image_hole = original_image.copy()

        # Ensure the original image has an alpha channel for transparency
        if image_hole.shape[2] == 3:  # Check if the image has three channels (RGB)
            image_hole = np.concatenate((image_hole, np.ones((image_hole.shape[0], image_hole.shape[1], 1), dtype=image_hole.dtype) * 255), axis=2)

        # Set alpha channel to 0 where mask is 1
        image_hole[mask == 1, 3] = 0

        # Crop the image to the bounding box of the hole
        hole_indices = np.where(mask == 0)
        y_min, y_max = hole_indices[0].min(), hole_indices[0].max()
        x_min, x_max = hole_indices[1].min(), hole_indices[1].max()
        image_hole = image_hole[y_min:y_max+1, x_min:x_max+1]
        logging.info("Created image with hole based on mask.")
        
        image_object = original_image.copy()

        # Ensure the original image has an alpha channel for transparency
        if image_object.shape[2] == 3:  # Check if the image has three channels (RGB)
            image_object = np.concatenate((image_object, np.ones((image_object.shape[0], image_object.shape[1], 1), dtype=image_object.dtype) * 255), axis=2)

        # Set alpha channel to 0 where mask is 0
        image_object[mask == 0, 3] = 0

        # Crop the image to the bounding box of the object
        object_indices = np.where(mask == 1)
        y_min, y_max = object_indices[0].min(), object_indices[0].max()
        x_min, x_max = object_indices[1].min(), object_indices[1].max()
        image_object = image_object[y_min:y_max+1, x_min:x_max+1]
        logging.info("Created and cropped object image with transparency based on mask.")
    except Exception as e:
        logging.error(f"Error in get_images: {str(e)}")
        raise

    return image_hole, image_object

if __name__ == "__main__":
    try:
        logging.info("Starting Snipe SAM process.")
        input_data = sys.stdin.read()
        json_data = json.loads(input_data)
        logging.info("Loaded input data.")

        pos_points = [[point["x"], point["y"]] for point in json_data["positive_points"]]
        neg_points = [[point["x"], point["y"]] for point in json_data["negative_points"]]

        # Combine the two lists of points into a single numpy array
        combined_points = np.array(pos_points + neg_points)
        logging.info(f"Combined positive and negative points: {combined_points}")

        original_image_data = base64.b64decode(json_data["original_image"])
        logging.info("Decoded base64 data.")

        original_image = Image.open(BytesIO(original_image_data)).convert("RGBA")
        alpha_channel = np.array(original_image)[..., 3]
        rgb_image = np.array(original_image.convert("RGB"))
        logging.info("Loaded and processed original image with alpha channel.")

        model_path = os.path.join(os.path.dirname(__file__), '../../models/EdgeSAM/weights/edge_sam_3x.pth')
        sam = sam_model_registry["edge_sam"](checkpoint=model_path)
        sam.to(device="cuda" if torch.cuda.is_available() else "cpu")
        predictor = SamPredictor(sam)
        predictor.set_image(rgb_image)
        logging.info("Initialized SAM model and set image.")

        input_point = combined_points
        logging.info(f"Input point: {input_point}")
        input_label = np.array([1] * len(pos_points) + [0] * len(neg_points))
        logging.info(f"Input label: {input_label}")

        masks, scores, logits = predictor.predict(
            point_coords=input_point,
            point_labels=input_label,
            num_multimask_outputs=4,
            use_stability_score=True
        )
        logging.info("Performed prediction to generate masks and scores.")

        best_mask = masks[np.argmax(scores)]

        plt.figure(figsize=(10,10))
        plt.imshow(rgb_image)
        show_mask(best_mask, plt.gca())
        plt.axis('off')
        mask_path = os.path.join(os.path.dirname(__file__), "mask.png")
        plt.savefig(mask_path, bbox_inches='tight', pad_inches=0)
        logging.info("Selected best mask based on highest score.")

        image_hole, image_object = get_images(np.array(original_image), best_mask, pos_points, neg_points)

        buffer = BytesIO()
        image_hole = Image.fromarray(image_hole)
        image_hole.save(buffer, format="PNG")
        image_hole_base64 = base64.b64encode(buffer.getvalue()).decode("utf-8")
        logging.info("Saved image hole as PNG and encoded to base64.")

        buffer = BytesIO()
        image_object = Image.fromarray(image_object)
        image_object.save(buffer, format="PNG")
        image_object_base64 = base64.b64encode(buffer.getvalue()).decode("utf-8")
        logging.info("Saved image object as PNG and encoded to base64.")

        buffer = BytesIO()
        image_with_mask = Image.open(mask_path)
        image_with_mask.save(buffer, format="PNG")
        image_with_mask_base64 = base64.b64encode(buffer.getvalue()).decode("utf-8")
        logging.info("Saved image with mask as PNG and encoded to base64.")

        with open(os.path.join(os.path.dirname(__file__), "image_hole.txt"), "w") as f:
            f.write(image_hole_base64)
        with open(os.path.join(os.path.dirname(__file__), "image_object.txt"), "w") as f:
            f.write(image_object_base64)
        with open(os.path.join(os.path.dirname(__file__), "image_with_mask.txt"), "w") as f:
            f.write(image_with_mask_base64)
        logging.info("Saved base64 strings to files.")
    except Exception as e:
        logging.error(f"Error in main block: {str(e)}")
        raise
