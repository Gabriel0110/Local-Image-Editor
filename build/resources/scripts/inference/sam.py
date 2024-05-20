from edge_sam import SamPredictor, sam_model_registry
import numpy as np
from PIL import Image, ImageDraw

import numpy as np
import torch
import matplotlib.pyplot as plt
import cv2
import sys
import json
import base64
from io import BytesIO

def show_mask(mask, ax, random_color=False):
    if random_color:
        color = np.concatenate([np.random.random(3), np.array([0.6])], axis=0)
    else:
        color = np.array([30/255, 144/255, 255/255, 0.6])
    h, w = mask.shape[-2:]
    mask_image = mask.reshape(h, w, 1) * color.reshape(1, 1, -1)
    ax.imshow(mask_image)
    
def show_points(coords, labels, ax, marker_size=50):
    pos_points = coords[labels==1]
    neg_points = coords[labels==0]
    ax.scatter(pos_points[:, 0], pos_points[:, 1], color='green', marker='o', s=marker_size, edgecolor='white', linewidth=1.25)
    ax.scatter(neg_points[:, 0], neg_points[:, 1], color='red', marker='o', s=marker_size, edgecolor='white', linewidth=1.25)   
    
def show_box(box, ax):
    x0, y0 = box[0], box[1]
    w, h = box[2] - box[0], box[3] - box[1]
    ax.add_patch(plt.Rectangle((x0, y0), w, h, edgecolor='green', facecolor=(0,0,0,0), lw=2))

def apply_transparency(image):
    """Convert black pixels to transparent pixels in an image."""
    image = image.convert("RGBA")
    data = image.getdata()
    new_data = []
    for item in data:
        # Change all black (
        # also
        # (0,0,0,255)
        # pixels to white
        if item[0] == 0 and item[1] == 0 and item[2] == 0:
            new_data.append((255, 255, 255, 0))
        else:
            new_data.append(item)
    image.putdata(new_data)
    return image

# image = cv2.imread('/Users/gtomberlin/Documents/Pictures/DSC_3182.JPG')
# image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)

# plt.figure(figsize=(10,10))
# plt.imshow(image)
# plt.axis('on')
# plt.show()

# sam = sam_model_registry["edge_sam"](checkpoint="/Users/gtomberlin/Documents/Code/Local-Image-Editor/resources/models/EdgeSAM/weights/edge_sam_3x.pth")
# sam.to(device="cpu")
# predictor = SamPredictor(sam)

# predictor.set_image(image)

# input_point = np.array([[2000, 2000]])
# input_label = np.array([1])

# plt.figure(figsize=(10,10))
# plt.imshow(image)
# show_points(input_point, input_label, plt.gca())
# plt.axis('on')
# plt.show()

# masks, scores, logits = predictor.predict(
#     point_coords=input_point,
#     point_labels=input_label,
#     num_multimask_outputs=4,
#     use_stability_score=True
# )

# for i, (mask, score) in enumerate(zip(masks, scores)):
#     plt.figure(figsize=(10,10))
#     plt.imshow(image)
#     show_mask(mask, plt.gca())
#     show_points(input_point, input_label, plt.gca())
#     plt.title(f"Mask {i+1}, Score: {score:.3f}", fontsize=18)
#     plt.axis('off')
#     plt.show() 

# Get the mask with the highest score
# best_mask = masks[np.argmax(scores)]
# best_mask = (best_mask > 0.5).astype(np.uint8)

# plt.figure(figsize=(10,10))
# plt.imshow(image)
# show_mask(best_mask, plt.gca())
# plt.axis('off')
# plt.show()

# Now, we want to cut out the object from the image creating a hole in the original image and a new image with the object
# We will use the mask to create a hole in the image
# mask = Image.fromarray(best_mask)
# mask = mask.resize((image.shape[1], image.shape[0]))
# mask = np.array(mask)

# Create a hole in the image
# image_hole = image.copy()
# image_hole[mask==1] = 0

# plt.figure(figsize=(10,10))
# plt.imshow(image_hole)
# plt.axis('off')
# plt.show()

# Create a new image with the object, but the image size should only be the size of the object
# image_object = image.copy()
# image_object[mask==0] = 0

# plt.figure(figsize=(10,10))
# plt.imshow(image_object)
# plt.axis('off')
# plt.show()

# pil_image = Image.fromarray(image_object)
# pil_image = apply_transparency(pil_image)
# pil_image.save("object.png")

import logging

# Set up logging
log_file = "snipe.log"
logging.basicConfig(
    filename=log_file,
    level=logging.INFO,
    format="%(asctime)s - %(levelname)s - %(message)s"
)

def get_images(original_image, mask_image, pos_points, neg_points):
    try:
        # Convert the mask to a numpy array
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

        original_image = base64.b64decode(json_data["original_image"])
        logging.info("Decoded base64 data.")

        image = cv2.imdecode(np.frombuffer(original_image, np.uint8), cv2.IMREAD_COLOR)
        image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
        logging.info("Loaded and processed original image.")

        sam = sam_model_registry["edge_sam"](checkpoint="/Users/gtomberlin/Documents/Code/Local-Image-Editor/resources/models/EdgeSAM/weights/edge_sam_3x.pth")
        sam.to(device="cpu")
        predictor = SamPredictor(sam)
        predictor.set_image(image)
        logging.info("Initialized SAM model and set image.")

        # Combine the positive and negative points into a single array
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

        # Overlay the mask over the original image and save it
        plt.figure(figsize=(10,10))
        plt.imshow(image)
        show_mask(best_mask, plt.gca())
        plt.axis('off')
        plt.savefig("mask.png", bbox_inches='tight', pad_inches=0)
        #plt.show()
        logging.info("Selected best mask based on highest score.")

        image_hole, image_object = get_images(image, best_mask, pos_points, neg_points)

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
        image_with_mask = Image.open("mask.png")
        image_with_mask.save(buffer, format="PNG")
        image_with_mask_base64 = base64.b64encode(buffer.getvalue()).decode("utf-8")
        logging.info("Saved image with mask as PNG and encoded to base64.")

        # Save the strings to respctive files
        with open("image_hole.txt", "w") as f:
            f.write(image_hole_base64)
        with open("image_object.txt", "w") as f:
            f.write(image_object_base64)
        with open("image_with_mask.txt", "w") as f:
            f.write(image_with_mask_base64)
        logging.info("Saved base64 strings to files.")
    except Exception as e:
        logging.error(f"Error in main block: {str(e)}")
        raise
